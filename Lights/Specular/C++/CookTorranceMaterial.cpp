#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <utils.h>
#include <CookTorranceMaterial.h>

/**
 * Cette classe définit un matériau pour dessiner le maillage
 * @param lights_count : nombre de lampes à gérer
 * @param diffuse : fournir un vec3 définissant la couleur diffuse du matériau
 * @param specular : fournir un vec3 définissant la couleur spéculaire du matériau
 * @param m : float >0..1 définissant l'aspect lisse (poche de 0) ou rugueux (proche de 0.5)
 * @param f0 : reflectance à la verticale
 */
CookTorranceMaterial::CookTorranceMaterial(int lights_count, vec3 diffuse, vec3 specular, float m, float f0) :
    Material("CookTorranceMaterial")
{
    // caractéristiques du matériau
    m_DiffuseColor = diffuse;
    m_SpecularColor = specular;
    m_M = m;
    m_F0 = f0;

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
void CookTorranceMaterial::setLightPositionColor(int num, vec4 position, vec3 color)
{
    vec4::copy(m_LightsPositions[num], position);
    vec3::copy(m_LightsColors[num], color);
}


/**
 * retourne le source du Vertex Shader
 */
std::string CookTorranceMaterial::getVertexShader()
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
std::string CookTorranceMaterial::getFragmentShader()
{
    int lights_count = m_LightsPositions.size();

    std::ostringstream srcFragmentShader;
    srcFragmentShader << "#version 300 es\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "// interpolation vers les fragments\n";
    srcFragmentShader << "precision mediump float;\n";
    srcFragmentShader << "in vec4 frgPosition;\n";
    srcFragmentShader << "in vec3 frgNormal;\n";
    srcFragmentShader << "out vec4 glFragColor;\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "// sources de lumière\n";
    srcFragmentShader << "uniform vec4 LightsPositions["<<lights_count<<"];\n";
    srcFragmentShader << "uniform vec3 LightsColors["<<lights_count<<"];\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "// caractéristiques du matériau\n";
    srcFragmentShader << "const vec3 Kd = "<<vec3::str(m_DiffuseColor)<<";\n";
    srcFragmentShader << "const vec3 Ks = "<<vec3::str(m_SpecularColor)<<";\n";
    srcFragmentShader << "const float m = "<<m_M<<";\n";
    srcFragmentShader << "const float f0 = "<<m_F0<<";\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "void main()\n";
    srcFragmentShader << "{\n";
    srcFragmentShader << "    // normale normalisée (après interpolation)\n";
    srcFragmentShader << "    vec3 N = normalize(frgNormal);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "    // direction vers l'oeil au niveau du fragment\n";
    srcFragmentShader << "    vec3 V = -normalize(frgPosition.xyz);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "    // couleur finale = cumul des éclairements\n";
    srcFragmentShader << "    vec3 sum = vec3(0.0, 0.0, 0.0);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "    // boucle sur les lampes\n";
    srcFragmentShader << "    for (int i=0; i<"<<lights_count<<"; i++) {\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "        // vecteur allant du point à la lumière\n";
    srcFragmentShader << "        vec3 L = normalize(LightsPositions[i].xyz - frgPosition.xyz);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "        // produit scalaire entre L et la normale = cosinus de l'angle entre L et N\n";
    srcFragmentShader << "        float dotNL = clamp(dot(N, L), 0.0, 1.0);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "        // cos de l'angle entre la normale et la vue\n";
    srcFragmentShader << "        float dotNV = clamp(dot(N, V), 0.0, 1.0);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "        // demi-vecteur H entre L et V, normalisé\n";
    srcFragmentShader << "        vec3 H = normalize(L + V);\n";
    srcFragmentShader << "        float dotNH = clamp(dot(N, H), 0.0, 1.0);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "        // terme de rugosité\n";
    srcFragmentShader << "        float dotNH2 = dotNH * dotNH;\n";
    srcFragmentShader << "        float m2 = m * m;\n";
    srcFragmentShader << "        float a = 1.0 / (4.0 * m2 * dotNH2 * dotNH2);\n";
    srcFragmentShader << "        float b = (dotNH2 - 1.0) / (m2 * dotNH2);\n";
    srcFragmentShader << "        float R = a * exp(b);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "        // terme géométrique\n";
    srcFragmentShader << "        float dotVH = clamp(dot(V,H), 0.0, 1.0);\n";
    srcFragmentShader << "        float k = 2.0 * dotNH / dotVH;\n";
    srcFragmentShader << "        float g1 = k * dotNV;\n";
    srcFragmentShader << "        float g2 = k * dotNL;\n";
    srcFragmentShader << "        float G = clamp(min(g1, g2), 0.0, 1.0);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "        // terme de Fresnel\n";
    srcFragmentShader << "        float F = f0 + (1.0 - f0) * pow(1.0 - dotVH, 5.0);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "        // coefficient spéculaire complet\n";
    srcFragmentShader << "        float rs = clamp(F * R * G / (dotNV * dotNL), 0.0, 1.0);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "        // cumul des contributions lumineuses\n";
    srcFragmentShader << "        sum += LightsColors[i] * ( dotNL * (Kd + Ks * rs) );\n";
    srcFragmentShader << "    }\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "    glFragColor = vec4(sum, 1.0);\n";
    srcFragmentShader << "}";
    return srcFragmentShader.str();
}


/**
 * compile ou recompile le shader
 */
void CookTorranceMaterial::compileShader()
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
VBOset* CookTorranceMaterial::createVBOset()
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
void CookTorranceMaterial::enable(mat4 mat4Projection, mat4 mat4ModelView)
{
    // appeler la méthode de la superclasse
    Material::enable(mat4Projection, mat4ModelView);

    // initialisation des lampes
    vec4::glUniform(m_LightsPositionsLoc, m_LightsPositions);
    vec3::glUniform(m_LightsColorsLoc,  m_LightsColors);
}


/**
 * Cette méthode supprime les ressources allouées
 */
CookTorranceMaterial::~CookTorranceMaterial()
{
}
