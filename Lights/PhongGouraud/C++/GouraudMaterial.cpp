#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>
#include <iomanip>

#include <utils.h>
#include <GouraudMaterial.h>

/**
 * Cette classe définit un matériau pour dessiner le maillage
 * @param lights_count : nombre de lampes à gérer
 * @param diffuse : fournir un vec3 définissant la couleur diffuse du matériau
 * @param specular : fournir un vec3 définissant la couleur spéculaire du matériau
 * @param shininess : aspect rugueux ou lisse
 */
GouraudMaterial::GouraudMaterial(int lights_count, vec3 diffuse, vec3 specular, float shininess) :
    Material("GouraudMaterial")
{
    // caractéristiques du matériau
    m_DiffuseColor = diffuse;
    m_SpecularColor = specular;
    m_Shininess = shininess;

    // coordonnées et couleurs des sources de lumières
    for (int i=0; i<lights_count; i++) {
        m_LightsPositions.push_back(vec4::create());
        m_LightsColors.push_back(vec3::create());
    }

    // compiler le shader
    compileShader();
}


/**
 * définit la position d'une lampe
 * @param num : numéro de la lampe
 * @param position : vec4 indiquant où elle se trouve dans l'espace caméra
 * @param color : vec3 indiquant sa couleur et son intensité
 */
void GouraudMaterial::setLightPositionColor(int num, vec4 position, vec3 color)
{
    vec4::copy(m_LightsPositions[num], position);
    vec3::copy(m_LightsColors[num], color);
}


/**
 * retourne le source du Vertex Shader
 */
std::string GouraudMaterial::getVertexShader()
{
    int lights_count = m_LightsPositions.size();

    std::ostringstream srcVertexShader;
    srcVertexShader << "#version 300 es\n";
    srcVertexShader << "\n";
    srcVertexShader << "// paramètres uniform\n";
    srcVertexShader << "uniform mat4 mat4ModelView;\n";
    srcVertexShader << "uniform mat4 mat4Projection;\n";
    srcVertexShader << "uniform mat3 mat3Normal;\n";
    srcVertexShader << "\n";
    srcVertexShader << "// attributs de sommets\n";
    srcVertexShader << "in vec3 glVertex;\n";
    srcVertexShader << "in vec3 glNormal;\n";
    srcVertexShader << "\n";
    srcVertexShader << "// interpolation vers les fragments\n";
    srcVertexShader << "out vec3 frgColor;\n";
    srcVertexShader << "\n";
    srcVertexShader << "// sources de lumière\n";
    srcVertexShader << "uniform vec4 LightsPositions["<<lights_count<<"];\n";
    srcVertexShader << "uniform vec3 LightsColors["<<lights_count<<"];\n";
    srcVertexShader << "\n";
    srcVertexShader << "// caractéristiques du matériau\n";
    srcVertexShader << "const vec3 Kd = "<<vec3::str(m_DiffuseColor)<<";\n";
    srcVertexShader << "const vec3 Ks = "<<vec3::str(m_SpecularColor)<<";\n";
    srcVertexShader << "const float ns = "<<std::setprecision(1)<<std::fixed<<m_Shininess<<";\n";
    srcVertexShader << "\n";
    srcVertexShader << "void main()\n";
    srcVertexShader << "{\n";
    srcVertexShader << "    // position 3D par rapport à la caméra\n";
    srcVertexShader << "    vec4 position = mat4ModelView * vec4(glVertex, 1.0);\n";
    srcVertexShader << "    gl_Position = mat4Projection * position;\n";
    srcVertexShader << "\n";
    srcVertexShader << "    // normale en ce sommet\n";
    srcVertexShader << "    vec3 N = normalize(mat3Normal * glNormal);\n";
    srcVertexShader << "\n";
    srcVertexShader << "    // direction opposée du vecteur Vue au niveau du fragment\n";
    srcVertexShader << "    vec3 mV = normalize(position.xyz);\n";
    srcVertexShader << "\n";
    srcVertexShader << "    // direction du reflet du vecteur Vue\n";
    srcVertexShader << "    vec3 R = reflect(mV, N);\n";
    srcVertexShader << "\n";
    srcVertexShader << "    // couleur finale = cumul des éclairements\n";
    srcVertexShader << "    vec3 sum = vec3(0.0, 0.0, 0.0);\n";
    srcVertexShader << "\n";
    srcVertexShader << "    // boucle sur les lampes\n";
    srcVertexShader << "    for (int i=0; i<"<<lights_count<<"; i++) {\n";
    srcVertexShader << "\n";
    srcVertexShader << "        // vecteur allant du point à la lumière\n";
    srcVertexShader << "        vec3 L = normalize(LightsPositions[i].xyz - position.xyz);\n";
    srcVertexShader << "\n";
    srcVertexShader << "        // produit scalaire entre L et la normale = cosinus de l'angle entre L et N\n";
    srcVertexShader << "        float dotNL = clamp(dot(N, L), 0.0, 1.0);\n";
    srcVertexShader << "\n";
    srcVertexShader << "        // alignement du reflet de la vue avec la lampe\n";
    srcVertexShader << "        float dotRL = clamp(dot(R,L), 0.0, 1.0);\n";
    srcVertexShader << "\n";
    srcVertexShader << "        // cumul des contributions lumineuses\n";
    srcVertexShader << "        sum += LightsColors[i] * ( Kd*dotNL + Ks*pow(dotRL, ns) );\n";
    srcVertexShader << "    }\n";
    srcVertexShader << "\n";
    srcVertexShader << "    frgColor = sum;\n";
    srcVertexShader << "}";
    return srcVertexShader.str();
}


/**
 * retourne le source du Fragment Shader
 */
std::string GouraudMaterial::getFragmentShader()
{
    const char* srcFragmentShader =
        "#version 300 es\n"
        "\n"
        "// interpolation vers les fragments\n"
        "precision mediump float;\n"
        "in vec3 frgColor;\n"
        "out vec4 glFragColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    glFragColor = vec4(frgColor, 1.0);\n"
        "}";
    return srcFragmentShader;
}


/**
 * compile ou recompile le shader
 */
void GouraudMaterial::compileShader()
{
    // appeler la méthode de la superclasse
    Material::compileShader();

    // déterminer où sont les variables uniform
    m_LightsPositionsLoc = glGetUniformLocation(m_ShaderId, "LightsPositions");
    m_LightsColorsLoc  = glGetUniformLocation(m_ShaderId, "LightsColors");
}


/**
 * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
 * @return le VBOset du matériau
 */
VBOset* GouraudMaterial::createVBOset()
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
void GouraudMaterial::enable(mat4 mat4Projection, mat4 mat4ModelView)
{
    // appeler la méthode de la superclasse
    Material::enable(mat4Projection, mat4ModelView);

    // initialisation des lampes
    vec4::glUniform(m_LightsPositionsLoc, m_LightsPositions);
    vec3::glUniform(m_LightsColorsLoc,  m_LightsColors);
}


/** destructeur */
GouraudMaterial::~GouraudMaterial()
{
}
