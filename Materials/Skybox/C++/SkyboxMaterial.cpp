#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <utils.h>
#include <SkyboxMaterial.h>


/**
 * Constructeur
 * @param skybox_basename : chemin d'accès aux 6 images
 */
SkyboxMaterial::SkyboxMaterial(std::string skybox_basename) : Material("SkyboxMaterial")
{
    // créer une texture de type TEXTURE_CUBE_MAP
    m_Texture = new TextureCube(skybox_basename);
    m_TextureLoc = -1;

    // compiler le shader
    compileShader();
}


/**
 * retourne le source du Vertex Shader
 */
std::string SkyboxMaterial::getVertexShader()
{
    const char* srcVertexShader =
        "#version 300 es\n"
        "in vec3 glVertex;\n"
        "uniform mat4 mat4ModelView;\n"
        "uniform mat4 mat4Projection;\n"
        "out vec3 frgDirection;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = mat4Projection * mat4ModelView * vec4(glVertex, 1.0);\n"
        "    frgDirection = glVertex.xyz;\n"
        "}";
    return srcVertexShader;
}


/**
 * retourne le source du Fragment Shader
 */
std::string SkyboxMaterial::getFragmentShader()
{
    const char* srcFragmentShader =
        "#version 300 es\n"
        "precision mediump float;\n"
        "uniform samplerCube txCube;\n"
        "in vec3 frgDirection;\n"
        "out vec4 glFragColor;\n"
        "void main()\n"
        "{\n"
        "    glFragColor = texture(txCube, frgDirection);\n"
        "}";
    return srcFragmentShader;
}


/**
 * compile ou recompile le shader
 */
void SkyboxMaterial::compileShader()
{
    // appeler la méthode de la superclasse
    Material::compileShader();

    // déterminer où sont les variables uniform
    m_MatPloc    = glGetUniformLocation(m_ShaderId, "mat4Projection");
    m_MatMVloc   = glGetUniformLocation(m_ShaderId, "mat4ModelView");
    m_TextureLoc = glGetUniformLocation(m_ShaderId, "txCube");
}


/**
 * Cette méthode active le matériau : met en place son shader,
 * fournit les variables uniform qu'il demande
 * @param mat4Projection : mat4 contenant la projection
 * @param mat4ModelView : mat4 contenant la transformation vers la caméra
 */
void SkyboxMaterial::enable(mat4 mat4Projection, mat4 mat4ModelView)
{
    // appeler la méthode de la superclasse
    Material::enable(mat4Projection, mat4ModelView);

    // activer la texture sur l'unité 0
    m_Texture->setTextureUnit(GL_TEXTURE0, m_TextureLoc);
}


/**
 * Cette méthode désactive le matériau
 */
void SkyboxMaterial::disable()
{
    // désactiver les textures
    m_Texture->setTextureUnit(GL_TEXTURE0);

    // appeler la méthode de la superclasse
    Material::disable();
}


/**
 * Cette méthode supprime les ressources allouées
 */
SkyboxMaterial::~SkyboxMaterial()
{
    delete m_Texture;
}
