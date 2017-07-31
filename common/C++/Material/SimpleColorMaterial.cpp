// Définition de la classe SimpleColorMaterial, une spécialisation de Material
// Ce matériau permet de dessiner en mode Multiple Render Targets

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <utils.h>
#include <SimpleColorMaterial.h>
#include <VBOset.h>


/**
 * constructeur
 * @param color : vec4(r,v,b,a) qui donne la couleur du matériau
 */
SimpleColorMaterial::SimpleColorMaterial(vec4 color) : Material("SimpleColorMaterial")
{
    // couleur propre imposée
    vec4::copy(m_Color, color);

    // compiler le shader
    compileShader();
}


/**
 * constructeur, crée une couleur aléatoire
 */
SimpleColorMaterial::SimpleColorMaterial() : Material("SimpleColorMaterial")
{
    // couleur propre aléatoire
    m_Color = vec4::create();
    vec4::random(m_Color, 1.0);

    // compiler le shader
    compileShader();
}


/** destructeur */
SimpleColorMaterial::~SimpleColorMaterial()
{
}


/**
 * retourne le source du Vertex Shader
 */
std::string SimpleColorMaterial::getVertexShader()
{
    return
        "#version 300 es\n"
        "\n"
        "// paramètres uniform\n"
        "uniform mat4 mat4ModelView;\n"
        "uniform mat4 mat4Projection;\n"
        "\n"
        "// attributs de sommets\n"
        "in vec3 glVertex;\n"
        "\n"
        "// interpolation vers les fragments\n"
        "out vec4 frgPosition;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    frgPosition = mat4ModelView * vec4(glVertex, 1.0);\n"
        "    gl_Position = mat4Projection * frgPosition;\n"
        "}";
}


/**
 * retourne le source du Fragment Shader
 */
std::string SimpleColorMaterial::getFragmentShader()
{
    std::ostringstream srcFragmentShader;
    srcFragmentShader << "#version 300 es\n";
    srcFragmentShader << "precision mediump float;\n";
    if (m_ClipPlaneOn) {
        srcFragmentShader << "\n";
        srcFragmentShader << "// plan de coupe\n";
        srcFragmentShader << "uniform vec4 ClipPlane;\n";
    }
    srcFragmentShader << "\n";
    srcFragmentShader << "in vec4 frgPosition;\n";
    srcFragmentShader << "out vec4 glFragData[4];\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "void main()\n";
    srcFragmentShader << "{\n";
    if (m_ClipPlaneOn) {
        srcFragmentShader << "    // plan de coupe\n";
        srcFragmentShader << "    if (dot(frgPosition, ClipPlane) < 0.0) discard;\n";
        srcFragmentShader << "\n";
    }
    srcFragmentShader << "    glFragData[0] = "<<vec4::str(m_Color)<<";\n";
    srcFragmentShader << "    glFragData[1] = vec4(0.0);\n";
    srcFragmentShader << "    glFragData[2] = vec4(frgPosition.xyz, 1.0);\n";
    srcFragmentShader << "    glFragData[3] = vec4(0.0,0.0,0.0, 1.0);\n";
    srcFragmentShader << "}";
    return srcFragmentShader.str();
}
