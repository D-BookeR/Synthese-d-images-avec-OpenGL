#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <utils.h>
#include <ColorPerVertexMaterial.h>

/**
 * Cette fonction définit un matériau pour dessiner le maillage
 */
ColorPerVertexMaterial::ColorPerVertexMaterial() : Material("ColorPerVertexMaterial")
{
    // compiler le shader
    compileShader();
}


/**
 * retourne le source du Vertex Shader
 */
std::string ColorPerVertexMaterial::getVertexShader()
{
    const char* srcVertexShader =
        "#version 300 es\n"
        "in vec3 glVertex;\n"
        "in vec3 glColor;\n"
        "out vec3 frgColor;\n"
        "uniform mat4 mat4ModelView;\n"
        "uniform mat4 mat4Projection;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = mat4Projection * mat4ModelView * vec4(glVertex, 1.0);\n"
        "    frgColor = glColor;\n"
        "}";
    return srcVertexShader;
}


/**
 * retourne le source du Fragment Shader
 */
std::string ColorPerVertexMaterial::getFragmentShader()
{
    const char* srcFragmentShader =
        "#version 300 es\n"
        "precision mediump float;\n"
        "in vec3 frgColor;\n"
        "out vec4 glFragColor;\n"
        "void main()\n"
        "{\n"
        "    glFragColor = vec4(frgColor, 1.0);\n"
        "}";
    return srcFragmentShader;
}


/**
 * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
 * @return le VBOset du matériau
 */
VBOset* ColorPerVertexMaterial::createVBOset()
{
    // créer le VBOset et spécifier les noms des attribute nécessaires à ce matériau
    VBOset* vboset = Material::createVBOset();
    vboset->addAttribute(MeshVertex::ID_ATTR_COLOR,  Utils::VEC3, "glColor");

    return vboset;
}


ColorPerVertexMaterial::~ColorPerVertexMaterial()
{
    if (m_ShaderId > 0) Utils::deleteShaderProgram(m_ShaderId);
}
