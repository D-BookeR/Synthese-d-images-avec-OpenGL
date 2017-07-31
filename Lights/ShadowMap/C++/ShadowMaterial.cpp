// Définition de la classe ShadowMaterial, une spécialisation de Material
// Ce matériau permet de dessiner des ombres


#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <utils.h>
#include <ShadowMaterial.h>

/**
 * constructeur
 * @param color couleur du matériau
 */
ShadowMaterial::ShadowMaterial(vec4 color) :
    Material("ShadowMaterial")
{
    // initialisations
    m_Color = vec4::clone(color);
    m_ShadowMap = nullptr;

    // compiler le shader
    compileShader();
}


/**
 * retourne le source du Vertex Shader
 */
std::string ShadowMaterial::getVertexShader()
{
    const char* srcVertexShader =
        "#version 300 es\n"
        "in vec3 glVertex;\n"
        "in vec3 glNormal;\n"
        "uniform mat4 mat4ModelView;\n"
        "uniform mat4 mat4Projection;\n"
        "uniform mat3 mat3Normal;\n"
        "out vec4 frgPosition;\n"
        "out vec3 frgNormal;\n"
        "void main()\n"
        "{\n"
        "    frgPosition = mat4ModelView * vec4(glVertex, 1.0);\n"
        "    gl_Position = mat4Projection * frgPosition;\n"
        "    frgNormal = mat3Normal * glNormal;\n"
        "}";
    return srcVertexShader;
}


/**
 * retourne le source du Fragment Shader
 */
std::string ShadowMaterial::getFragmentShader()
{
    std::string srcFragmentShader =
        "#version 300 es\n"
        "precision mediump float;\n"
        "in vec4 frgPosition;\n"
        "in vec3 frgNormal;\n"
        "out vec4 glFragColor;\n"
        "\n"
        "// position de la lampe\n"
        "uniform vec3 LightPosition;\n"
        "\n"
        "// shadow map et matrice de retour pour cette lampe\n"
        "uniform sampler2D ShadowMap;\n"
        "uniform mat4 mat4Shadow;\n"
        "\n"
        "// rustine pour limiter l'acné de surface ou les murs volants\n"
        "const float DECALAGE = 0.0; // -0.005;\n"
        "\n"
        "// retourne 1.0 si le point est éclairé, 0.0 s'il est dans l'ombre\n"
        "float isIlluminated(vec4 position)\n"
        "{\n"
        "    // calculer les coordonnées du vertex dans la shadow map\n"
        "    vec4 posshadow = mat4Shadow * position;\n"
        "\n"
        "    // normaliser les coordonnées homogènes\n"
        "    posshadow /= posshadow.w;\n"
        "\n"
        "    // distance entre ce fragment et la lumière\n"
        "    float distancePointLight = posshadow.z + DECALAGE;\n"
        "\n"
        "    // comparer la valeur donnée par la ShadowMap avec la distance du fragment à la lumière\n"
        "    float distanceObstacleLight = texture(ShadowMap, posshadow.xy).r;\n"
        "    if (distanceObstacleLight < distancePointLight) {\n"
        "        // un objet opaque est entre nous et la lumière\n"
        "        return 0.4;\n"
        "    } else {\n"
        "        // pas d'ombre\n"
        "        return 1.0;\n"
        "    }\n"
        "}\n"
        "\n"
        "void main()\n"
        "{\n"
        "    vec4 Kd = "+vec4::str(m_Color)+";\n"
        "\n"
        "    // déterminer la direction de la lumière d'après la matrice d'ombre\n"
        "    vec3 L = normalize(LightPosition - frgPosition.xyz);\n"
        "\n"
        "    // vecteur normal\n"
        "    vec3 N = normalize(frgNormal);\n"
        "    float dotNL = clamp(dot(N, L), 0.4, 1.0);\n"
        "\n"
        "    glFragColor = min(isIlluminated(frgPosition), dotNL) * Kd;\n"
        "}";
    return srcFragmentShader;
}


/**
 * recompile le shader du matériau
 */
void ShadowMaterial::compileShader()
{
    // appeler la méthode de la superclasse
    Material::compileShader();

    // déterminer où sont les variables uniform
    m_ShadowMapLoc     = glGetUniformLocation(m_ShaderId, "ShadowMap");
    m_ShadowMatrixLoc  = glGetUniformLocation(m_ShaderId, "mat4Shadow");
    m_LightPositionLoc = glGetUniformLocation(m_ShaderId, "LightPosition");
}


/**
 * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
 * @return le VBOset du matériau
 */
VBOset* ShadowMaterial::createVBOset()
{
    // créer le VBOset et spécifier les noms des attribute nécessaires à ce matériau
    VBOset* vboset = Material::createVBOset();
    vboset->addAttribute(MeshVertex::ID_ATTR_NORMAL, Utils::VEC3, "glNormal");
    return vboset;
}


/**
 * fournit la shadow map et sa matrice
 * @param shadowmatrix à utiliser
 * @param positioncamera position de la lampe dans le repère caméra
 * @param shadowmap à utiliser pour les ombres, null si désactivée (pour le dessin dans elle-même)
 */
void ShadowMaterial::setShadowMap(mat4& shadowmatrix, vec3& positioncamera, ShadowMap* shadowmap)
{
    m_ShadowMap = shadowmap;
    m_ShadowMatrix = shadowmatrix;
    m_PositionCamera = positioncamera;
}


/**
 * Cette méthode active le matériau : met en place son shader,
 * fournit les variables uniform qu'il demande
 * @param mat4Projection : mat4 contenant la projection
 * @param mat4ModelView : mat4 contenant la transformation vers la caméra
 */
void ShadowMaterial::enable(mat4 mat4Projection, mat4 mat4ModelView)
{
    // appeler la méthode de la superclasse
    Material::enable(mat4Projection, mat4ModelView);

    if (m_ShadowMap != nullptr) {

        // associer la shadowmap à l'unité 0
        m_ShadowMap->setTextureUnit(GL_TEXTURE0, m_ShadowMapLoc);

        // fournir la matrice d'ombre
        mat4::glUniformMatrix(m_ShadowMatrixLoc, m_ShadowMatrix);

        // position de la lampe
        vec3::glUniform(m_LightPositionLoc, m_PositionCamera);
    }
}


/**
 * Cette méthode désactive le matériau
 */
void ShadowMaterial::disable()
{
    // désactiver la texture associée à la shadow map
    if (m_ShadowMap != nullptr) {
        m_ShadowMap->setTextureUnit(GL_TEXTURE0);
    }

    // appeler la méthode de la superclasse
    Material::disable();
}


/**
 * Cette méthode supprime les ressources allouées
 */
ShadowMaterial::~ShadowMaterial()
{
}
