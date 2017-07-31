#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>
#include <iomanip>

#include <utils.h>
#include <LightMaterial2.h>

/**
 * Cette classe définit un matériau pour dessiner le maillage
 * @param diffuse : fournir un vec3 définissant la couleur diffuse du matériau
 * @param specular : fournir un vec3 définissant la couleur spéculaire du matériau
 * @param shininess : aspect rugueux ou lisse
 */
LightMaterial2::LightMaterial2(vec3 diffuse, vec3 specular, float shininess) :
    Material("LightMaterial2")
{
    // caractéristiques du matériau
    m_DiffuseColor = diffuse;
    m_SpecularColor = specular;
    m_Shininess = shininess;

    // compiler le shader
    compileShader();
}


/**
 * retourne le source du Vertex Shader
 */
std::string LightMaterial2::getVertexShader()
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
        "    frgNormal = mat3Normal * glNormal;\n"
        "}";
    return srcVertexShader;
}


/**
 * retourne le source du Fragment Shader
 */
std::string LightMaterial2::getFragmentShader()
{
    std::ostringstream srcFragmentShader;
    srcFragmentShader << "#version 300 es\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "// interpolation vers les fragments\n";
    srcFragmentShader << "precision mediump float;\n";
    srcFragmentShader << "in vec4 frgPosition;\n";
    srcFragmentShader << "in vec3 frgNormal;\n";
    srcFragmentShader << "out vec4 glFragColor;\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "// source de lumière (coordonnées globales !)\n";
    srcFragmentShader << "vec3 LightPosition = vec3(5.0, 5.0, -2.0);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "// caractéristiques du matériau\n";
    srcFragmentShader << "const vec3 Kd = "<<vec3::str(m_DiffuseColor)<<";\n";
    srcFragmentShader << "const vec3 Ks = "<<vec3::str(m_SpecularColor)<<";\n";
    srcFragmentShader << "const float ns = "<<std::setprecision(1)<<std::fixed<<m_Shininess<<";\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "void main()\n";
    srcFragmentShader << "{\n";
    srcFragmentShader << "    // normale normalisée (après interpolation)\n";
    srcFragmentShader << "    vec3 N = normalize(frgNormal);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "    // vecteur allant du point à la lumière\n";
    srcFragmentShader << "    vec3 L = normalize(LightPosition - frgPosition.xyz);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "    // produit scalaire entre L et la normale\n";
    srcFragmentShader << "    float dotNL = clamp(dot(N, L), 0.0, 1.0);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "    // direction opposée du vecteur Vue au niveau du fragment\n";
    srcFragmentShader << "    vec3 mV = normalize(frgPosition.xyz);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "    // direction du reflet du vecteur Vue\n";
    srcFragmentShader << "    vec3 R = reflect(mV, N);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "    // alignement du reflet de la vue avec la lampe\n";
    srcFragmentShader << "    float dotRL = clamp(dot(R,L), 0.0, 1.0);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "    // couleur résultante\n";
    srcFragmentShader << "    glFragColor = vec4( Kd*dotNL + Ks*pow(dotRL, ns), 1.0);\n";
    srcFragmentShader << "}";
    return srcFragmentShader.str();
}


/**
 * compile ou recompile le shader
 */
void LightMaterial2::compileShader()
{
    // appeler la méthode de la superclasse
    Material::compileShader();
}


/**
 * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
 * @return le VBOset du matériau
 */
VBOset* LightMaterial2::createVBOset()
{
    // créer le VBOset et spécifier les noms des attribute nécessaires à ce matériau
    VBOset* vboset = Material::createVBOset();
    vboset->addAttribute(MeshVertex::ID_ATTR_NORMAL,   Utils::VEC3, "glNormal");
    return vboset;
}


/**
 * Cette méthode active le matériau : met en place son shader,
 * fournit les variables uniform qu'il demande
 * @param mat4Projection : mat4 contenant la projection
 * @param mat4ModelView : mat4 contenant la transformation vers la caméra
 */
void LightMaterial2::enable(mat4 mat4Projection, mat4 mat4ModelView)
{
    // appeler la méthode de la superclasse
    Material::enable(mat4Projection, mat4ModelView);
}


/**
 * Cette méthode supprime les ressources allouées
 */
LightMaterial2::~LightMaterial2()
{
}
