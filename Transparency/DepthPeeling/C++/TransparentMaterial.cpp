/**
 * Définition de la classe TransparentMaterial, une spécialisation de DeferredShadingMaterial
 * Ce matériau est destiné à dessiner en mode Multiple Render Targets
 */

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

#include <utils.h>
#include <TransparentMaterial.h>


/**
 * Constructeur de la classe TransparentMaterial. Ce matériau mémorise dans un FBO MRT les informations
 * nécessaires pour appliquer le modèle de Phong ultérieurement : position, normale, couleur.
 * @param Kd : un vec4(r,g,b,a) ou une texture donnant la couleur diffuse et la transparence
 */
TransparentMaterial::TransparentMaterial(vec4 Kd) :
    DeferredShadingMaterial(Kd)
{
    init();
}


/**
 * constructeur
 * @param Ks : un vec3(r,g,b) donnant la couleur spéculaire
 * @param Ns : poli du matériau
 */
TransparentMaterial::TransparentMaterial(vec3 Ks, float Ns) :
    DeferredShadingMaterial(Ks, Ns)
{
    init();
}


/**
 * constructeur
 * @param Kd : un vec4(r,g,b,a) ou une texture donnant la couleur diffuse et la transparence
 * @param Ks : un vec3(r,g,b) donnant la couleur spéculaire
 * @param Ns : poli du matériau
 */
TransparentMaterial::TransparentMaterial(vec4 Kd, vec3 Ks, float Ns) :
    DeferredShadingMaterial(Kd, Ks, Ns)
{
    init();
}


/**
 * constructeur
 * @param diffuse : nom d'une texture
 * @param Ks : un vec3(r,g,b) donnant la couleur spéculaire
 * @param Ns : poli du matériau
 */
TransparentMaterial::TransparentMaterial(std::string diffuse, vec3 Ks, float Ns) :
    DeferredShadingMaterial(diffuse, Ks, Ns)
{
    init();
}


void TransparentMaterial::init()
{
    // identifiants des textures associées aux sampler2D DepthMap*
    m_DepthMapFar = 0;
    m_DepthMapNear = 0;

    // dimensions de la fenêtre pour normaliser gl_FragCoord.xy
    m_WindowSize = vec2::fromValues(1.0, 1.0);

    // recompiler le shader
    compileShader();
}


/**
 * retourne le source du Fragment Shader
 */
std::string TransparentMaterial::getFragmentShader()
{
    std::ostringstream srcFragmentShader;
    srcFragmentShader.setf(std::ios::fixed, std::ios::floatfield);
    srcFragmentShader << "#version 300 es\n";
    srcFragmentShader << "precision mediump float;\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "// coordonnées et normale du fragment\n";
    srcFragmentShader << "in vec4 frgPosition;\n";
    srcFragmentShader << "in vec3 frgNormal;\n";

    // coordonnées de texture s'il y a une texture
    if (m_TxDiffuse != nullptr || m_TxSpecular != nullptr) {
        // coordonnées de texture interpolées
        srcFragmentShader << "in vec2 frgTexCoord;\n";
    }
    srcFragmentShader << "out vec4 glFragData[4];\n";

    srcFragmentShader << "\n";
    srcFragmentShader << "// caractéristiques du matériau\n";

    // couleur ou texture diffuse
    if (m_TxDiffuse != nullptr) {
        srcFragmentShader << "uniform sampler2D txDiffuse;\n";
    } else if (m_KdIsInterpolated) {
        srcFragmentShader << "in vec4 frgColor;\n";
    } else {
        srcFragmentShader << "const vec4 Kd = "<<vec4::str(m_Kd)<<";\n";
    }

    // couleur ou texture spéculaire
    if (m_Ns >= 0.0) {
        if (m_TxSpecular != nullptr) {
            srcFragmentShader << "uniform sampler2D txSpecular;\n";
        } else {
            srcFragmentShader << "const vec3 Ks = "<<vec3::str(m_Ks)<<";\n";
        }
        srcFragmentShader << "const float Ns = "<<m_Ns<<";\n";
    }

    // depth maps à prendre en compte (précédentes distances pour rester devant les opaques, mais derrière les transparents)
    srcFragmentShader << "uniform sampler2D DepthMapFar;\n";
    srcFragmentShader << "uniform sampler2D DepthMapNear;\n";
    srcFragmentShader << "uniform vec2 WindowSize;\n";

    // plan de coupe
    if (m_ClipPlaneOn) {
        srcFragmentShader << "\n";
        srcFragmentShader << "// plan de coupe\n";
        srcFragmentShader << "uniform vec4 ClipPlane;\n";
    }

    srcFragmentShader << "\n";
    srcFragmentShader << "void main()\n";
    srcFragmentShader << "{\n";

    // plan de coupe
    if (m_ClipPlaneOn) {
        srcFragmentShader << "    if (dot(frgPosition, ClipPlane) < 0.0) discard;\n";
    }

    // test du depth buffer pour le depth peeling
    srcFragmentShader << "    // coordonnées dans les depth maps\n";
    srcFragmentShader << "    vec2 fragcoords = gl_FragCoord.xy/WindowSize;\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "    // comparaison avec la distance lointaine\n";
    srcFragmentShader << "    float depth = texture(DepthMapFar, fragcoords).x;\n";
    srcFragmentShader << "    if (gl_FragCoord.z >= depth) discard;\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "    // comparaison avec la distance proche\n";
    srcFragmentShader << "    depth = texture(DepthMapNear, fragcoords).x;\n";
    srcFragmentShader << "    if (gl_FragCoord.z <= depth) discard;\n";
    srcFragmentShader << "\n";

    // couleur diffuse du fragment
    if (m_TxDiffuse != nullptr) {
        srcFragmentShader << "    vec4 Kd = texture(txDiffuse, frgTexCoord);\n";
    } else if (m_KdIsInterpolated) {
        srcFragmentShader << "    vec4 Kd = frgColor;\n";
    }

    // couleur spéculaire du fragment
    if (m_Ns >= 0.0 && m_TxSpecular != nullptr) {
        srcFragmentShader << "    vec3 Ks = texture(txSpecular, frgTexCoord).rgb;\n";
    }

    // remplir les buffers MRT avec les informations nécessaires pour Phong plus tard
    srcFragmentShader << "    // remplir les buffers avec les informations\n";
    srcFragmentShader << "    glFragData[0] = vec4(Kd.rgb * Kd.a, Kd.a); // prémultiplication par alpha\n";
    if (m_Ns >= 0.0) {
        srcFragmentShader << "    glFragData[1] = vec4(Ks * Kd.a, Ns);\n";
    } else {
        srcFragmentShader << "    glFragData[1] = vec4(0.0);\n";
    }
    srcFragmentShader << "    glFragData[2] = vec4(frgPosition.xyz, 1.0);\n";
    srcFragmentShader << "    glFragData[3] = vec4(normalize(frgNormal), 0.0);\n";
    srcFragmentShader << "}";
    return srcFragmentShader.str();
}


/**
 * recompile le shader du matériau
 */
void TransparentMaterial::compileShader()
{
    // appeler la méthode de la superclasse
    DeferredShadingMaterial::compileShader();

    // déterminer où sont les variables uniform spécifiques
    m_DepthMapFarLoc  = glGetUniformLocation(m_ShaderId, "DepthMapFar");
    m_DepthMapNearLoc = glGetUniformLocation(m_ShaderId, "DepthMapNear");
    m_WindowSizeLoc   = glGetUniformLocation(m_ShaderId, "WindowSize");
}


/**
 * affecte la texture DepthMap du shader
 * @param far : depth buffer qui limite les dessins arrière de ce matériau
 * @param near : depth buffer qui limite les dessins avant de ce matériau
 */
void TransparentMaterial::setDepthMaps(GLuint far, GLuint near)
{
    m_DepthMapFar = far;
    m_DepthMapNear = near;
}


/**
 * spécifie la taille de la fenêtre
 * @param width : largeur en nombre de pixels de la fenêtre
 * @param height : hauteur en nombre de pixels de la fenêtre
 */
void TransparentMaterial::setWindowDimensions(int width, int height)
{
    m_WindowSize = vec2::fromValues(width, height);
}


/**
 * Cette méthode active le matériau : met en place son shader,
 * fournit les variables uniform qu'il demande
 * @param mat4Projection : mat4 contenant la projection
 * @param mat4ModelView : mat4 contenant la transformation vers la caméra
 */
void TransparentMaterial::enable(mat4 mat4Projection, mat4 mat4ModelView)
{
    // appeler la méthode de la superclasse
    DeferredShadingMaterial::enable(mat4Projection, mat4ModelView);

    // fournir et activer la DepthMapFar dans l'unité 2
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_DepthMapFar);
    glUniform1i(m_DepthMapFarLoc, GL_TEXTURE2-GL_TEXTURE0);

    // fournir et activer la DepthMapNear dans l'unité 3
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_DepthMapNear);
    glUniform1i(m_DepthMapNearLoc, GL_TEXTURE3-GL_TEXTURE0);

    // fournir les dimensions de la fenêtre
    vec2::glUniform(m_WindowSizeLoc, m_WindowSize);
}


/**
 * Cette méthode désactive le matériau
 * @note le shader doit être activé
 */
void TransparentMaterial::disable()
{
    // désactiver les depth maps
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, 0);

    // appeler la méthode de la superclasse
    DeferredShadingMaterial::disable();
}


/**
 * Cette méthode supprime les ressources allouées
 */
TransparentMaterial::~TransparentMaterial()
{
}
