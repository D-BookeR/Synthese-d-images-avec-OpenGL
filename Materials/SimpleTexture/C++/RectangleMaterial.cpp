#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <utils.h>
#include <RectangleMaterial.h>
#include <VBOset.h>

/**
 * constructeur
 * @param texture du matériau
 */
RectangleMaterial::RectangleMaterial() : Material("RectangleMaterial")
{
    // charger la texture
    //m_Texture = new Texture2D("data/textures/grille.png");
    //m_Texture = new Texture2D("data/textures/earth/earth_map.jpg", GL_LINEAR, GL_CLAMP_TO_EDGE);
    m_Texture = new Texture2D("data/textures/hill-meadow-tree-green-2048.jpg", GL_LINEAR, GL_CLAMP_TO_EDGE);
    m_TextureLoc = -1;

    // compiler le shader
    compileShader();
}


/**
 * retourne le source du Vertex Shader
 */
std::string RectangleMaterial::getVertexShader()
{
    const char* srcVertexShader =
        "#version 300 es\n"
        "in vec3 glVertex;\n"
        "in vec2 glTexCoord;\n"
        "out vec2 frgTexCoord;\n"
        "uniform mat4 mat4ModelView;\n"
        "uniform mat4 mat4Projection;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = mat4Projection * mat4ModelView * vec4(glVertex, 1.0);\n"
        "    frgTexCoord = glTexCoord;\n"
        "}";
    return srcVertexShader;
}


/**
 * retourne le source du Fragment Shader
 */
std::string RectangleMaterial::getFragmentShader()
{
    const char* srcFragmentShader =
        "#version 300 es\n"
        "precision mediump float;\n"
        "in vec2 frgTexCoord;\n"
        "uniform sampler2D txColor;\n"
        "out vec4 glFragColor;\n"
        "void main()\n"
        "{\n"
        "    glFragColor = texture(txColor, frgTexCoord);\n"
        "}";
    return srcFragmentShader;
}


/**
 * compile ou recompile le shader
 */
void RectangleMaterial::compileShader()
{
    // appeler la méthode de la superclasse
    Material::compileShader();

    // déterminer où sont les variables uniform
    m_TextureLoc = glGetUniformLocation(m_ShaderId, "txColor");
}


/**
 * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
 * @return le VBOset du matériau, voir Mesh.h
 */
VBOset* RectangleMaterial::createVBOset()
{
    // créer le VBOset et spécifier les noms des attribute nécessaires à ce matériau
    VBOset* vboset = Material::createVBOset();
    vboset->addAttribute(MeshVertex::ID_ATTR_TEXCOORD, Utils::VEC2, "glTexCoord");

    return vboset;
}


/**
 * Cette méthode active le matériau : met en place son shader,
 * fournit les variables uniform qu'il demande
 * @param mat4Projection : mat4 contenant la projection
 * @param mat4ModelView : mat4 contenant la transformation vers la caméra
 */
void RectangleMaterial::enable(mat4 mat4Projection, mat4 mat4ModelView)
{
    // appeler la méthode de la superclasse
    Material::enable(mat4Projection, mat4ModelView);

    // activer la texture sur l'unité 0
    m_Texture->setTextureUnit(GL_TEXTURE0, m_TextureLoc);
}


/**
 * Cette méthode désactive le matériau
 */
void RectangleMaterial::disable()
{
    // désactiver les textures
    m_Texture->setTextureUnit(GL_TEXTURE0);

    // appeler la méthode de la superclasse
    Material::disable();
}


/**
 * Cette méthode supprime les ressources allouées
 */
RectangleMaterial::~RectangleMaterial()
{
    delete m_Texture;
}
