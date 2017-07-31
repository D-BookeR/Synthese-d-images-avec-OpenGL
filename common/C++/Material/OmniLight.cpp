/**
 * Définition de la classe OmniLight, une base pour construire des éclairages diffus et localisés
 */

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <gl-matrix.h>
#include <utils.h>

#include <OmniLight.h>
#include <SceneBase.h>

/**
 * constructeur
 * C'est une lampe toute simple, positionnelle ou directionnelle, sans ombre
 */
OmniLight::OmniLight():
    Light()
{
    m_Name = "OmniLight";

    // initialisation des variables membre spécifiques
    m_PositionScene = vec4::fromValues(0,0,1,0);
    m_PositionCamera = vec4::clone(m_PositionScene);

    // compiler le shader
    m_LightPositionLoc = 0;
    compileShader();
}


/**
 * destructeur
 */
OmniLight::~OmniLight()
{
}


/**
 * indique si cette lampe a une position (true) ou est ambiante (false)
 * @return false pour une lampe ambiante
 */
bool OmniLight::hasPosition()
{
    return true;
}


/**
 * Cette méthode définit la position de la lampe
 * @param position : vec4 indiquant où se trouve la lampe
 */
void OmniLight::setPosition(vec4 position)
{
    vec4::copy(m_PositionScene, position);
    vec4::copy(m_PositionCamera, position);
}


/**
 * retourne la position de la lampe dans le repère de la scène
 * @return position
 */
vec4 OmniLight::getPosition()
{
    return m_PositionScene;
}


/**
 * applique une matrice sur la position de la lampe afin d'obtenir la position caméra
 * @param matrix : mat4
 */
void OmniLight::transformPosition(mat4& matrix)
{
    // calculer les coordonnées dans le repère caméra
    vec4::transformMat4(m_PositionCamera, m_PositionScene, matrix);
}


/**
 * Cette méthode définit la position visée par la lampe
 * NB : Cette méthode ne fait rien dans le cas d'une lampe omnidirectionnelle
 * @param target : vec4 indiquant le point ou l'axe que vise la lampe
 * NB : Cette méthode ne fait rien dans le cas d'une lampe omnidirectionnelle
 * NB : l'une des deux informations position et cible doivent être des points
 */
void OmniLight::setTarget(vec4 target)
{
}


/**
 * applique une matrice sur la cible de la lampe afin d'obtenir la position caméra
 * NB : cette méthode ne fait rien dans le cas d'une lampe omnidirectionnelle
 * @param matrix : mat4
 */
void OmniLight::transformTarget(mat4& matrix)
{
}


/**
 * applique une matrice sur la position et la cible de la lampe
 * @param matrix : mat4
 */
void OmniLight::transform(mat4& matrix)
{
    transformPosition(matrix);
    transformTarget(matrix);
}


/**
 * retourne la position de la lampe dans le repère caméra
 */
vec4 OmniLight::getPositionCamera()
{
    return m_PositionCamera;
}


/**
 * construit le Fragment Shader qui calcule l'éclairement de cette lampe
 * @return source du shader
 */
std::string OmniLight::getFragmentShader()
{
    return
        "#version 300 es\n"
        "precision mediump float;\n"
        "in vec2 frgTexCoord;\n"
        "out vec4 glFragColor;\n"
        "\n"
        "uniform sampler2D MapPosition;\n"
        "uniform sampler2D MapNormale;\n"
        "uniform sampler2D MapDiffuse;\n"
        "uniform sampler2D MapSpecular;\n"
        "uniform sampler2D MapDepth;\n"
        "uniform vec3 LightColor;\n"
        "uniform vec4 LightPosition;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    // récupérer les infos du g-buffer\n"
        "    vec4 position = texture(MapPosition, frgTexCoord);\n"
        "    if (position.w != 1.0) discard;\n"
        "    gl_FragDepth = texture(MapDepth, frgTexCoord).r;\n"
        "    vec4 normal  = texture(MapNormale, frgTexCoord);\n"
        "    vec4 Kd = texture(MapDiffuse, frgTexCoord);\n"
        "\n"
        "    if (normal.w != 0.0) {\n"
        "        // éclairement diffus uniquement\n"
        "        glFragColor = vec4(LightColor * Kd.rgb, 1.0) * Kd.a;\n"
        "    } else {\n"
        "        // calculer la direction et l'intensité de la lampe\n"
        "        vec3 L;\n"
        "        vec3 color;\n"
        "        if (LightPosition.w != 0.0) {\n"
        "            // positionnelle\n"
        "            L = LightPosition.xyz - position.xyz;\n"
        "            float distance = length(L);\n"
        "            // diviser la couleur par la distance au carré\n"
        "            color = LightColor / (distance*distance);\n"
        "            L = L / distance;\n"
        "        } else {\n"
        "            // directionnelle\n"
        "            L = normalize(LightPosition.xyz);\n"
        "            color = LightColor;\n"
        "        }\n"
        "        // direction de la normale et produit scalaire\n"
        "        vec3 N = normal.xyz;\n"
        "        float dotNL = clamp(dot(N,L), 0.0, 1.0);\n"
        "\n"
        "        // couleur spéculaire et coefficient ns\n"
        "        vec4 Ks = texture(MapSpecular, frgTexCoord);\n"
        "        float ns = Ks.a;\n"
        "        if (ns > 0.0) {\n"
        "            // reflet spéculaire\n"
        "            vec3 R = reflect(normalize(position.xyz), N);\n"
        "            float dotRL = clamp(dot(R,L), 0.0, 1.0);\n"
        "            // éclairement diffus et reflet spéculaire\n"
        "            glFragColor = vec4(color * (Kd.rgb*dotNL + Ks.rgb*pow(dotRL, ns)), 1.0) * Kd.a;\n"
        "        } else {\n"
        "                // éclairement diffus sans reflet spéculaire\n"
        "            glFragColor = vec4(color * (Kd.rgb*dotNL), 1.0) * Kd.a;\n"
        "        }\n"
        "    }\n"
        "}";
}


/**
 * détermine où sont les variables uniform spécifiques de cette lampe
 */
void OmniLight::findUniformLocations()
{
    // obtenir les emplacements de la superclasse
    Light::findUniformLocations();

    // emplacement de la position de la lampe
    m_LightPositionLoc = glGetUniformLocation(m_ShaderId, "LightPosition");
}


/**
 * applique l'éclairement défini par cette lampe
 * @param gbuffer : FBO MRT contenant toutes les informations de la scène à éclairer
 */
void OmniLight::startProcess(FrameBufferObject* gbuffer)
{
    // préparer le shader pour le traitement
    Light::startProcess(gbuffer);

    // fournir la position de la lampe (la couleur est fournie par la superclasse)
    vec4::glUniform(m_LightPositionLoc, m_PositionCamera);
}
