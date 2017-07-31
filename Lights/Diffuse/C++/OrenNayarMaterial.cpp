#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <utils.h>
#include <OrenNayarMaterial.h>

/**
 * Cette classe définit un matériau pour dessiner le maillage
 * @param lights_count : nombre de lampes à gérer
 * @param diffuse : fournir un vec3 définissant la couleur diffuse du matériau
 * @param sigma : float 0..1 aspect rugueux ou lisse du matériau
 */
OrenNayarMaterial::OrenNayarMaterial(int lights_count, vec3 diffuse, float sigma) :
    Material("OrenNayarMaterial")
{
    // caractéristiques du matériau
    m_DiffuseColor = diffuse;
    m_Sigma2 = sigma * sigma;

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
void OrenNayarMaterial::setLightPositionColor(int num, vec4 position, vec3 color)
{
    vec4::copy(m_LightsPositions[num], position);
    vec3::copy(m_LightsColors[num], color);
}


/**
 * retourne le source du Vertex Shader
 */
std::string OrenNayarMaterial::getVertexShader()
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
std::string OrenNayarMaterial::getFragmentShader()
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
    srcFragmentShader << "const float sigma2 = "<<m_Sigma2<<";\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "void main()\n";
    srcFragmentShader << "{\n";
    srcFragmentShader << "    // normale normalisée (après interpolation)\n";
    srcFragmentShader << "    vec3 N = normalize(frgNormal);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "    // direction du vecteur Vue au niveau du fragment\n";
    srcFragmentShader << "    vec3 V = normalize(-frgPosition.xyz);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "    // produit scalaire entre V et N = cosinus de l'angle entre V et N\n";
    srcFragmentShader << "    float dotNV = dot(N,V);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "    // angle entre V et N\n";
    srcFragmentShader << "    float angleNV = acos(dotNV);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "    // projection de V dans le plan (T,B) de Frenet\n";
    srcFragmentShader << "    vec3 Vtb = normalize(V - N*dotNV);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "    // distribution gaussienne\n";
    srcFragmentShader << "    float a = 1.0 - 0.5 * sigma2 / (sigma2 + 0.57);\n";
    srcFragmentShader << "    float b = 0.45 * sigma2/(sigma2 + 0.09);\n";
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
    srcFragmentShader << "        // produit scalaire entre L et N = cosinus de l'angle entre L et N\n";
    srcFragmentShader << "        float dotNL = dot(N, L);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "        // angle entre L et N\n";
    srcFragmentShader << "        float angleNL = acos(dotNL);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "        // composante diffuse d'Oren-Nayar\n";
    srcFragmentShader << "        float alpha = max(angleNV, angleNL);\n";
    srcFragmentShader << "        float beta  = min(angleNV, angleNL);\n";
    srcFragmentShader << "        float c = sin(alpha) * tan(beta);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "        // projection de L dans le plan (T,B) de Frenet\n";
    srcFragmentShader << "        vec3 Ltb = normalize(L - N*dotNL);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "        // cosinus de l'angle entre les azimuts de V et L\n";
    srcFragmentShader << "        float gamma = max(0.0, dot(Vtb, Ltb));\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "        // cumul des contributions lumineuses\n";
    srcFragmentShader << "        sum += LightsColors[i] * ( Kd * clamp(dotNL,0.0,1.0) * (a + b*gamma*c) );\n";
    srcFragmentShader << "    }\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "    glFragColor = vec4(sum, 1.0);\n";
    srcFragmentShader << "}";
    return srcFragmentShader.str();
}


/**
 * compile ou recompile le shader
 */
void OrenNayarMaterial::compileShader()
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
VBOset* OrenNayarMaterial::createVBOset()
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
void OrenNayarMaterial::enable(mat4 mat4Projection, mat4 mat4ModelView)
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
OrenNayarMaterial::~OrenNayarMaterial()
{
}
