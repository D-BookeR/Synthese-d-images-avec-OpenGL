#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <utils.h>
#include <FBOMaterial.h>

/**
 * Constructeur
 * @param idtexture : fournir l'identifiant d'une texture OpenGL
 */
FBOMaterial::FBOMaterial(GLint idtexture) :
    Material("FBOMaterial")
{
    // mémoriser la texture
    m_IdTexture = idtexture;
    m_TextureLoc = -1;

    // compiler le shader
    compileShader();
}


/**
 * retourne le source du Vertex Shader
 */
std::string FBOMaterial::getVertexShader()
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
std::string FBOMaterial::getFragmentShader()
{
    const char* srcFragmentShader =
        "#version 300 es\n"
        "precision mediump float;\n"
        "in vec2 frgTexCoord;\n"
        "uniform sampler2D txColor;\n"
        "out vec4 glFragColor;\n"
        "void main()\n"
        "{\n"
        "    vec3 color = texture(txColor, frgTexCoord).rgb;\n"
        "    color *= clamp(1.2-distance(frgTexCoord, vec2(0.5, 0.5)), 0.6, 1.0);\n"
        "    glFragColor = vec4(color, 1.0);\n"
        "}";
    return srcFragmentShader;
}


/**
 * compile ou recompile le shader
 */
void FBOMaterial::compileShader()
{
    // appeler la méthode de la superclasse
    Material::compileShader();

    // déterminer où sont les variables uniform
    m_TextureLoc = glGetUniformLocation(m_ShaderId, "txColor");
}


/**
 * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
 * @return le VBOset du matériau
 */
VBOset* FBOMaterial::createVBOset()
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
void FBOMaterial::enable(mat4 mat4Projection, mat4 mat4ModelView)
{
    // appeler la méthode de la superclasse
    Material::enable(mat4Projection, mat4ModelView);

    // activer la texture sur l'unité 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_IdTexture);
    glUniform1i(m_TextureLoc, GL_TEXTURE0-GL_TEXTURE0);
}


/**
 * Cette méthode désactive le matériau
 */
void FBOMaterial::disable()
{
    // désactiver la texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // appeler la méthode de la superclasse
    Material::disable();
}


/**
 * Cette méthode supprime les ressources allouées
 */
FBOMaterial::~FBOMaterial()
{
}
