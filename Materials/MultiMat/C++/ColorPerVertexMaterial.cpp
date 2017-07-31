#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <utils.h>
#include <ColorPerVertexMaterial.h>


/**
 * Constructeur
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
        "uniform float Time;\n"
        "in vec3 frgColor;\n"
        "out vec4 glFragColor;\n"
        "void main()\n"
        "{\n"
        "    // désaturer la couleur en fonction du temps\n"
        "    const vec3 coefs = vec3(0.29,0.58,0.11);\n"
        "    float lum = dot(frgColor, coefs);\n"
        "    vec3 gray = vec3(lum, lum, lum);\n"
        "    float k = 0.65 + 0.35*sin(Time*2.0);\n"
        "    vec3 desat = mix(gray, frgColor, k);\n"
        "    glFragColor = vec4(desat, 1.0);\n"
        "}";
    return srcFragmentShader;
}


/**
 * compile ou recompile le shader
 */
void ColorPerVertexMaterial::compileShader()
{
    // appeler la méthode de la superclasse
    Material::compileShader();

    // déterminer où sont les variables uniform
    m_TimeLoc = glGetUniformLocation(m_ShaderId, "Time");
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


/**
 * Cette méthode active le matériau : met en place son shader,
 * fournit les variables uniform qu'il demande
 * @param mat4Projection : mat4 contenant la projection
 * @param mat4ModelView : mat4 contenant la transformation vers la caméra
 */
void ColorPerVertexMaterial::enable(mat4 mat4Projection, mat4 mat4ModelView)
{
    // appeler la méthode de la superclasse
    Material::enable(mat4Projection, mat4ModelView);

    // fournir le temps
    glUniform1f(m_TimeLoc, Utils::Time);
}


/**
 * Cette méthode supprime les ressources allouées
 */
ColorPerVertexMaterial::~ColorPerVertexMaterial()
{
    if (m_ShaderId > 0) Utils::deleteShaderProgram(m_ShaderId);
}
