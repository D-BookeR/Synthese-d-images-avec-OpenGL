#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <utils.h>
#include <AppleMaterial.h>


/**
 * Constructeur
 */
AppleMaterial::AppleMaterial() : Material("AppleMaterial")
{
    // compiler le shader
    compileShader();
}


/**
 * retourne le source du Vertex Shader
 */
std::string AppleMaterial::getVertexShader()
{
    return
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
}


/**
 * retourne le source du Fragment Shader
 */
std::string AppleMaterial::getFragmentShader()
{
    return
        "#version 300 es\n"
        "precision mediump float;\n"
        "in vec2 frgTexCoord;\n"
        "out vec4 glFragColor;\n"
        "\n"
        "const float PI = 3.14159;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    // rotation de 25° des coordonnées de texture\n"
        "    const float angle = 25.0 * PI/180.0;\n"
        "    float k = sin(angle)*frgTexCoord.s - cos(angle)*frgTexCoord.t;\n"
        "    // 8 ondulations par unité de distance\n"
        "    float f = 0.5*sin(2.0*PI*(8.0*k))+0.5;\n"
        "    // action sur le vert et le bleu => cyan un peu sombre\n"
        "    glFragColor = vec4(0, f*0.8, f, 1.0);\n"
        "}";
}


/**
 * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
 * @return le VBOset du matériau
 */
VBOset* AppleMaterial::createVBOset()
{
    // créer le VBOset et spécifier les noms des attribute nécessaires à ce matériau
    VBOset* vboset = Material::createVBOset();
    vboset->addAttribute(MeshVertex::ID_ATTR_TEXCOORD, Utils::VEC2, "glTexCoord");
    return vboset;
}


/**
 * Cette méthode supprime les ressources allouées
 */
AppleMaterial::~AppleMaterial()
{
}
