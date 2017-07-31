#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <utils.h>
#include <WardMaterial.h>

/**
 * Cette classe définit un matériau pour dessiner le maillage
 * @param lights_count : nombre de lampes à gérer
 * @param diffuse : fournir un vec3 définissant la couleur diffuse du matériau
 * @param specular : fournir un vec3 définissant la couleur spéculaire du matériau
 * @param alpha : vec2 coefficient de Ward
 */
WardMaterial::WardMaterial(int lights_count, vec3 diffuse, vec3 specular, vec2 alpha) : Material("WardMaterial")
{
    // caractéristiques du matériau
    m_DiffuseColor = diffuse;
    m_SpecularColor = specular;
    m_Alpha = alpha;

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
void WardMaterial::setLightPositionColor(int num, vec4 position, vec3 color)
{
    vec4::copy(m_LightsPositions[num], position);
    vec3::copy(m_LightsColors[num], color);
}


/**
 * retourne le source du Vertex Shader
 */
std::string WardMaterial::getVertexShader()
{
    const char* srcVertexShader =
        "#version 300 es\n"
        "in vec3 glVertex;\n"
        "in vec3 glNormal;\n"
        "in vec3 glTangent;\n"
        "out vec4 frgPosition;\n"
        "out vec3 frgNormal;\n"
        "out vec3 frgTangent;\n"
        "uniform mat4 mat4ModelView;\n"
        "uniform mat4 mat4Projection;\n"
        "uniform mat3 mat3Normal;\n"
        "void main()\n"
        "{\n"
        "    frgPosition = mat4ModelView * vec4(glVertex, 1.0);\n"
        "    gl_Position = mat4Projection * frgPosition;\n"
        "    frgNormal = mat3Normal * glNormal;\n"
        "    frgTangent = mat3Normal * glTangent;\n"
        "}";
    return srcVertexShader;
}


/**
 * retourne le source du Fragment Shader
 */
std::string WardMaterial::getFragmentShader()
{
    int lights_count = m_LightsPositions.size();

    std::ostringstream srcFragmentShader;
    srcFragmentShader << "#version 300 es\n";
    srcFragmentShader << "precision mediump float;\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "// informations géométriques\n";
    srcFragmentShader << "in vec4 frgPosition;\n";
    srcFragmentShader << "in vec3 frgNormal;\n";
    srcFragmentShader << "in vec3 frgTangent;\n";
    srcFragmentShader << "out vec4 glFragColor;\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "// sources de lumière\n";
    srcFragmentShader << "uniform vec4 LightsPositions["<<lights_count<<"];\n";
    srcFragmentShader << "uniform vec3 LightsColors["<<lights_count<<"];\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "// caractéristiques du matériau\n";
    srcFragmentShader << "const vec3 Kd = "<<vec3::str(m_DiffuseColor)<<";\n";
    srcFragmentShader << "const vec3 Ks = "<<vec3::str(m_SpecularColor)<<";\n";
    srcFragmentShader << "const vec2 alpha = "<<vec2::str(m_Alpha)<<";\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "const float pi = 3.14159;\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "void main()\n";
    srcFragmentShader << "{\n";
    srcFragmentShader << "    // reconstruire le repère de Frenet normalisé\n";
    srcFragmentShader << "    vec3 N = normalize(frgNormal);\n";
    srcFragmentShader << "    vec3 T = normalize(frgTangent);\n";
    srcFragmentShader << "    vec3 B = cross(N, T);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "    // direction du vecteur Vue au niveau du fragment\n";
    srcFragmentShader << "    vec3 V = normalize(-frgPosition.xyz);\n";
    srcFragmentShader << "    float dotNV = clamp(dot(N, V), 0.0, 1.0);\n";
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
    srcFragmentShader << "        // modulation de l'éclairement type Lambert\n";
    srcFragmentShader << "        float dotNL = clamp(dot(N, L), 0.0, 1.0);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "        // demi-vecteur H entre L et V, normalisé (!)\n";
    srcFragmentShader << "        vec3 H = normalize(L + V);\n";
    srcFragmentShader << "        float dotNH = clamp(dot(N, H), 0.0, 1.0);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "        // éclairement spéculaire Ward\n";
    srcFragmentShader << "        float hts = dot(H, T) / alpha.s;\n";
    srcFragmentShader << "        float hbs = dot(H, B) / alpha.t;\n";
    srcFragmentShader << "        float beta = -2.0 * (hts*hts + hbs*hbs)/(1.0 + dotNH);\n";
    srcFragmentShader << "        float s = exp(beta) / (4.0 * pi * alpha.s * alpha.t * sqrt(dotNL*dotNV));\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "        // cumul des contributions lumineuses\n";
    srcFragmentShader << "        sum += LightsColors[i] * ( (Kd + Ks * s) * dotNL );\n";
    srcFragmentShader << "    }\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "    glFragColor = vec4(sum, 1.0);\n";
    srcFragmentShader << "}";
    return srcFragmentShader.str();
}


/**
 * recompile le shader du matériau
 */
void WardMaterial::compileShader()
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
VBOset* WardMaterial::createVBOset()
{
    // créer le VBOset et spécifier les noms des attribute nécessaires à ce matériau
    VBOset* vboset = Material::createVBOset();
    vboset->addAttribute(MeshVertex::ID_ATTR_NORMAL,   Utils::VEC3, "glNormal");
    vboset->addAttribute(MeshVertex::ID_ATTR_TANGENT,  Utils::VEC3, "glTangent");
    return vboset;
}


/**
 * Cette méthode active le matériau : met en place son shader,
 * fournit les variables uniform qu'il demande
 * @param mat4Projection : mat4 contenant la projection
 * @param mat4ModelView : mat4 contenant la transformation vers la caméra
 */
void WardMaterial::enable(mat4 mat4Projection, mat4 mat4ModelView)
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
WardMaterial::~WardMaterial()
{
    Utils::deleteShaderProgram(m_ShaderId);
}
