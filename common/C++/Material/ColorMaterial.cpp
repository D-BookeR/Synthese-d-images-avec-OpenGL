// Définition de la classe ColorMaterial, une spécialisation de Material
// Ce matériau permet de dessiner en mode Multiple Render Targets

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <utils.h>
#include <ColorMaterial.h>
#include <VBOset.h>

/**
 * constructeur pour une couleur aléatoire
 */
ColorMaterial::ColorMaterial() : Material("ColorMaterial")
{
    // couleur propre aléatoire
    m_Color = vec4::create();
    vec4::random(m_Color, 1.0);

    // compiler le shader
    compileShader();
}


/**
 * constructeur
 * @param color : vec4(r,v,b,a) qui donne la couleur du matériau
 */
ColorMaterial::ColorMaterial(vec4 color) : Material("ColorMaterial")
{
    // couleur propre imposée
    vec4::copy(m_Color, color);

    // compiler le shader
    compileShader();
}


/** destructeur */
ColorMaterial::~ColorMaterial()
{
}


/**
 * retourne le source du Vertex Shader
 */
std::string ColorMaterial::getVertexShader()
{
    const char* srcVertexShader =
        "#version 300 es\n"
        "\n"
        "// paramètres uniform\n"
        "uniform mat4 mat4ModelView;\n"
        "uniform mat4 mat4Projection;\n"
        "uniform mat3 mat3Normal;\n"
        "\n"
        "// attributs de sommets\n"
        "in vec3 glVertex;\n"
        "in vec3 glNormal;\n"
        "\n"
        "// interpolation vers les fragments\n"
        "out vec4 frgPosition;\n"
        "out vec3 frgNormal;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    frgPosition = mat4ModelView * vec4(glVertex, 1.0);\n"
        "    gl_Position = mat4Projection * frgPosition;\n"
        "    frgNormal   = mat3Normal * glNormal;\n"
        "}";
    return srcVertexShader;
}


/**
 * retourne le source du Fragment Shader
 */
std::string ColorMaterial::getFragmentShader()
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
    srcFragmentShader << "in vec3 frgNormal;\n";
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
    srcFragmentShader << "    glFragData[3] = vec4(frgNormal, 0.0);\n";
    srcFragmentShader << "}";
    return srcFragmentShader.str();
}


/**
 * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
 * @return le VBOset du matériau
 */
VBOset* ColorMaterial::createVBOset()
{
    // créer le VBOset et spécifier les noms des attribute nécessaires à ce matériau
    VBOset* vboset = Material::createVBOset();
    vboset->addAttribute(MeshVertex::ID_ATTR_NORMAL, Utils::VEC3, "glNormal");
    return vboset;
}
