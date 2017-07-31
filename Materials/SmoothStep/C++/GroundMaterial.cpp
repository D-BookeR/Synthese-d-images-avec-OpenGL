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
std::string GroundMaterial::getFragmentShader()
{
    return
        "#version 300 es\n"
        "precision mediump float;\n"
        "in vec2 frgTexCoord;\n"
        "out vec4 glFragColor;\n"
        "\n"
        "const vec4 bleu = vec4(0.0, 0.0, 1.0, 1.0);\n"
        "const vec4 cyan = vec4(0.2, 0.7, 1.0, 1.0);\n"
        "\n"
        "void main()\n"
        "{\n"
        "    // distance du fragment par rapport à (0.5, 0.5)\n"
        "    const vec2 center = vec2(0.5, 0.5);\n"
        "    float dist = distance(frgTexCoord.st, center);\n"
        "    float q = smoothstep(0.3, 0.5, dist);\n"
        "    // couleur = mélange en fonction de la distance\n"
        "    glFragColor = mix(cyan, bleu, q);\n"
        "}";
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
