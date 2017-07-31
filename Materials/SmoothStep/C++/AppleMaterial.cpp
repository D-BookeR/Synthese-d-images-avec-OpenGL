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
        "const vec4 vert = vec4(0.2, 1.0, 0.3, 1.0);\n"
        "const vec4 jaune = vec4(0.9, 0.7, 0.1, 1.0);\n"
        "\n"
        "void main()\n"
        "{\n"
        "    // \"hauteur relative\" du fragment dans les coordonnées de texture\n"
        "    float qt = smoothstep(0.4, 0.6, frgTexCoord.t);\n"
        "    // la couleur est un mélange en fonction de cette hauteur\n"
        "    glFragColor = mix(jaune, vert, qt);\n"
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
