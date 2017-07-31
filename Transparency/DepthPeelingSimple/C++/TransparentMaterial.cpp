/**
 * Définition de la classe TransparentMaterial, une spécialisation de DeferredShadingMaterial
 * Ce matériau est destiné à dessiner en mode Multiple Render Targets
 */

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

#include <utils.h>
#include <VBOset.h>
#include <TransparentMaterial.h>


/**
 * Constructeur de la classe TransparentMaterial.
 * @param Kd : un vec4(r,g,b,a) donnant la couleur diffuse et la transparence
 */
TransparentMaterial::TransparentMaterial(vec4 Kd) :
    Material("TransparentMaterial")
{
    // propriétés
    m_Kd = Kd;

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
    srcFragmentShader << "\n";
    srcFragmentShader << "// coordonnées du fragment\n";
    srcFragmentShader << "precision mediump float;\n";
    srcFragmentShader << "in vec4 frgPosition;\n";
    srcFragmentShader << "out vec4 glFragData[4];\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "// caractéristiques du matériau\n";
    srcFragmentShader << "const vec4 Kd = "<<vec4::str(m_Kd)<<";\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "// depth maps à prendre en compte\n";
    srcFragmentShader << "uniform sampler2D DepthMapFar;\n";
    srcFragmentShader << "uniform sampler2D DepthMapNear;\n";
    srcFragmentShader << "uniform vec2 WindowSize;\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "void main()\n";
    srcFragmentShader << "{\n";
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
    srcFragmentShader << "    // couleur finale = celle du matériau\n";
    srcFragmentShader << "    glFragData[0] = vec4(Kd.rgb * Kd.a, Kd.a); // prémultiplication par alpha\n";
    srcFragmentShader << "    glFragData[1] = vec4(0.0);\n";
    srcFragmentShader << "    glFragData[2] = vec4(frgPosition.xyz, 1.0);\n";
    srcFragmentShader << "    glFragData[3] = vec4(1.0, 1.0, 1.0, 1.0);\n";
    srcFragmentShader << "}\n";
    return srcFragmentShader.str();
}


/**
 * recompile le shader du matériau
 */
void TransparentMaterial::compileShader()
{
    // appeler la méthode de la superclasse
    Material::compileShader();

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
    Material::enable(mat4Projection, mat4ModelView);

    // fournir et activer la DepthMapFar dans l'unité 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_DepthMapFar);
    glUniform1i(m_DepthMapFarLoc, GL_TEXTURE0-GL_TEXTURE0);

    // fournir et activer la DepthMapNear dans l'unité 1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_DepthMapNear);
    glUniform1i(m_DepthMapNearLoc, GL_TEXTURE1-GL_TEXTURE0);

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
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    // appeler la méthode de la superclasse
    Material::disable();
}


/**
 * Cette méthode supprime les ressources allouées
 */
TransparentMaterial::~TransparentMaterial()
{
}
