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
 * Constructeur de la classe TransparentMaterial.
 * @param texture Texture2D à appliquer
 * @param alpha transparence du matériau
 */
TransparentMaterial::TransparentMaterial(Texture2D* texture, float alpha) :
    Material("TransparentMaterial")
{
    // paramètres
    m_Texture = texture;
    m_TextureLoc = -1;
    m_Alpha = alpha;

    // identifiants des textures associées aux sampler2D DepthMap*
    m_DepthMapFar = 0;
    m_DepthMapNear = 0;

    // dimensions de la fenêtre pour normaliser gl_FragCoord.xy
    m_WindowSize = vec2::fromValues(1.0, 1.0);

    // recompiler le shader
    compileShader();
}


/**
 * retourne le source du Vertex Shader
 */
std::string TransparentMaterial::getVertexShader()
{
    std::ostringstream srcVertexShader;
    srcVertexShader << "#version 300 es\n";
    srcVertexShader << "in vec3 glVertex;\n";
    srcVertexShader << "in vec2 glTexCoord;\n";
    srcVertexShader << "out vec2 frgTexCoord;\n";
    srcVertexShader << "uniform mat4 mat4ModelView;\n";
    srcVertexShader << "uniform mat4 mat4Projection;\n";
    srcVertexShader << "void main()\n";
    srcVertexShader << "{\n";
    srcVertexShader << "    gl_Position = mat4Projection * mat4ModelView * vec4(glVertex, 1.0);\n";
    srcVertexShader << "    frgTexCoord = glTexCoord;\n";
    srcVertexShader << "}";
    return srcVertexShader.str();
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
    srcFragmentShader << "in vec2 frgTexCoord;\n";
    srcFragmentShader << "uniform sampler2D txColor;\n";
    srcFragmentShader << "uniform sampler2D DepthMapFar;\n";
    srcFragmentShader << "uniform sampler2D DepthMapNear;\n";
    srcFragmentShader << "uniform vec2 WindowSize;\n";
    srcFragmentShader << "out vec4 glFragColor;\n";
    srcFragmentShader << "void main()\n";
    srcFragmentShader << "{\n";
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
    srcFragmentShader << "    // retourner la couleur de la texture prémultipliée par la transparence\n";
    srcFragmentShader << "    glFragColor = texture(txColor, frgTexCoord) * "<<m_Alpha<<";\n";
    srcFragmentShader << "}";
    return srcFragmentShader.str();
}


/**
 * compile le shader du matériau
 */
void TransparentMaterial::compileShader()
{
    // appeler la méthode de la superclasse
    Material::compileShader();

    // déterminer où sont les variables uniform spécifiques
    m_TextureLoc      = glGetUniformLocation(m_ShaderId, "txColor");
    m_DepthMapFarLoc  = glGetUniformLocation(m_ShaderId, "DepthMapFar");
    m_DepthMapNearLoc = glGetUniformLocation(m_ShaderId, "DepthMapNear");
    m_WindowSizeLoc   = glGetUniformLocation(m_ShaderId, "WindowSize");
}


/**
 * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
 * @return le VBOset du matériau
 */
VBOset* TransparentMaterial::createVBOset()
{
    // créer le VBOset et spécifier les noms des attribute nécessaires à ce matériau
    VBOset* vboset = Material::createVBOset();
    vboset->addAttribute(MeshVertex::ID_ATTR_TEXCOORD, Utils::VEC3, "glTexCoord");
    return vboset;
}


/**
 * affecte la texture DepthMap du shader
 * @param far : depth buffer qui limite les dessins arrière de ce matériau
 * @param near : depth buffer qui limite les dessins avant de ce matériau
 */
void TransparentMaterial::setDepthMaps(GLuint far, GLuint near)
{
    m_DepthMapFar  = far;
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

    // activer la texture sur l'unité 0
    m_Texture->setTextureUnit(GL_TEXTURE0, m_TextureLoc);

    // fournir et activer la DepthMapFar dans l'unité 1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_DepthMapFar);
    glUniform1i(m_DepthMapFarLoc, GL_TEXTURE1-GL_TEXTURE0);

    // fournir et activer la DepthMapNear dans l'unité 2
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_DepthMapNear);
    glUniform1i(m_DepthMapNearLoc, GL_TEXTURE2-GL_TEXTURE0);

    // fournir les dimensions de la fenêtre
    vec2::glUniform(m_WindowSizeLoc, m_WindowSize);
}


/**
 * Cette méthode désactive le matériau
 * @note le shader doit être activé
 */
void TransparentMaterial::disable()
{
    // désactiver les textures
    m_Texture->setTextureUnit(GL_TEXTURE0);

    // désactiver les depth maps
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE3);
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
