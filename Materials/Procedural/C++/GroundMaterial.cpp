#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <utils.h>
#include <GroundMaterial.h>


/**
 * Constructeur
 */
GroundMaterial::GroundMaterial() : Material("GroundMaterial")
{
    // compiler le shader
    compileShader();
}


/**
 * retourne le source du Vertex Shader
 */
std::string GroundMaterial::getVertexShader()
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
std::string GroundMaterial::getFragmentShader()
{
    const char* srcFragmentShader =
        "#version 300 es\n"
        "precision mediump float;\n"
        "in vec2 frgTexCoord;\n"
        "out vec4 glFragColor;\n"
        "const float PI = 3.14159;\n"
        "void main()\n"
        "{\n"
        "    // multiplier (s,t) pour une répétition de 6 fois\n"
        "    vec2 texCoord = frgTexCoord * 6.0;\n"
        "    // appliquer une fonction sinus pour obtenir des vagues\n"
        "    texCoord = abs(sin(texCoord * PI));\n"
        "    // appliquer une fonction puissance pour «durcir» la variation\n"
        "    float red = pow(texCoord.s, 8.0);\n"
        "    float blue  = pow(texCoord.t, 8.0);\n"
        "    // couleur = fonction des coordonnées de texture\n"
        "    glFragColor = vec4(red, 0.7, blue, 1.0);\n"
        "}";
    return srcFragmentShader;
}


/**
 * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
 * @return le VBOset du matériau
 */
VBOset* GroundMaterial::createVBOset()
{
    // créer le VBOset et spécifier les noms des attribute nécessaires à ce matériau
    VBOset* vboset = Material::createVBOset();
    vboset->addAttribute(MeshVertex::ID_ATTR_TEXCOORD, Utils::VEC2, "glTexCoord");
    return vboset;
}


/**
 * Cette méthode supprime les ressources allouées
 */
GroundMaterial::~GroundMaterial()
{
}
