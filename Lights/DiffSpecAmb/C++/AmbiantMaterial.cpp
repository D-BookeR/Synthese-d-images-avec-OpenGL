#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <utils.h>
#include <AmbiantMaterial.h>

/**
 * Cette classe définit un matériau pour dessiner le maillage
 * @param lights_count : nombre de lampes à gérer
 * @param ambiant : fournir un vec3 définissant la couleur ambiante du matériau
 * @param diffuse : fournir une texture définissant la couleur diffuse du matériau
 */
AmbiantMaterial::AmbiantMaterial(int lights_count, vec3 ambiant, Texture2D* diffuse) :
    Material("AmbiantMaterial")
{
    // caractéristiques du matériau
    m_AmbiantColor = ambiant;
    m_DiffuseTexture = diffuse;

    // coordonnées et couleurs des sources de lumières
    for (int i=0; i<lights_count; i++) {
        m_LightsColors.push_back(vec3::create());
    }

    // compiler le shader
    compileShader();
}


/**
 * définit la position et l'intensité d'une lampe
 * @param num : numéro de la lampe
 * @param position : vec4 indiquant où elle se trouve dans l'espace caméra
 * @param color : vec3 indiquant sa couleur et son intensité
 */
void AmbiantMaterial::setLightPositionColor(int num, vec4 position, vec3 color)
{
    vec3::copy(m_LightsColors[num], color);
}


/**
 * retourne le source du Vertex Shader
 */
std::string AmbiantMaterial::getVertexShader()
{
    const char* srcVertexShader =
        "#version 300 es\n"
        "\n"
        "// paramètres uniform\n"
        "uniform mat4 mat4ModelView;\n"
        "uniform mat4 mat4Projection;\n"
        "\n"
        "// attributs de sommets\n"
        "in vec3 glVertex;\n"
        "in vec2 glTexCoord;\n"
        "\n"
        "// interpolation vers les fragments\n"
        "out vec2 frgTexCoord;\n"
        "\n"
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
std::string AmbiantMaterial::getFragmentShader()
{
    int lights_count = m_LightsColors.size();

    std::ostringstream srcFragmentShader;
    srcFragmentShader << "#version 300 es\n";
    srcFragmentShader << "// interpolation vers les fragments\n";
    srcFragmentShader << "precision mediump float;\n";
    srcFragmentShader << "in vec2 frgTexCoord;\n";
    srcFragmentShader << "out vec4 glFragColor;\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "// sources de lumière\n";
    srcFragmentShader << "uniform vec3 LightsColors["<<lights_count<<"];\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "// caractéristiques du matériau\n";
    srcFragmentShader << "const vec3 Ka = "<<vec3::str(m_AmbiantColor)<<";\n";
    srcFragmentShader << "uniform sampler2D txColor;\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "void main()\n";
    srcFragmentShader << "{\n";
    srcFragmentShader << "    // récupération de la couleur diffuse\n";
    srcFragmentShader << "    vec3 Kd = texture(txColor, frgTexCoord).rgb;\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "    // couleur finale = cumul des éclairements\n";
    srcFragmentShader << "    vec3 sum = vec3(0.0, 0.0, 0.0);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "    // boucle sur les lampes\n";
    srcFragmentShader << "    for (int i=0; i<"<<lights_count<<"; i++) {\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "        // cumul des contributions lumineuses\n";
    srcFragmentShader << "        sum += LightsColors[i] * Ka * Kd;\n";
    srcFragmentShader << "    }\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "    glFragColor = vec4(sum, 1.0);\n";
    srcFragmentShader << "}";
    return srcFragmentShader.str();
}


/**
 * compile ou recompile le shader
 */
void AmbiantMaterial::compileShader()
{
    // appeler la méthode de la superclasse
    Material::compileShader();

    // déterminer où sont les variables uniform
    m_LightsColorsLoc  = glGetUniformLocation(m_ShaderId, "LightsColors");
    m_TextureLoc         = glGetUniformLocation(m_ShaderId, "txColor");
}


/**
 * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
 * @return le VBOset du matériau
 */
VBOset* AmbiantMaterial::createVBOset()
{
    // créer le VBOset et spécifier les noms des attribute nécessaires à ce matériau
    VBOset* vboset = Material::createVBOset();
    vboset->addAttribute(MeshVertex::ID_ATTR_TEXCOORD, Utils::VEC2, "glTexCoord");
    return vboset;
}


/**
 * Cette méthode active le matériau : met en place son shader,
 * fournit les variables uniform qu'il demande
 * @param mat4Projection : mat4 contenant la projection
 * @param mat4ModelView : mat4 contenant la transformation vers la caméra
 */
void AmbiantMaterial::enable(mat4 mat4Projection, mat4 mat4ModelView)
{
    // appeler la méthode de la superclasse
    Material::enable(mat4Projection, mat4ModelView);

    // initialisation des lampes
    vec3::glUniform(m_LightsColorsLoc,  m_LightsColors);

    // activer la texture sur l'unité 0
    m_DiffuseTexture->setTextureUnit(GL_TEXTURE0, m_TextureLoc);
}


/**
 * Cette méthode désactive le matériau
 */
void AmbiantMaterial::disable()
{
    // désactiver les textures
    m_DiffuseTexture->setTextureUnit(GL_TEXTURE0);

    // appeler la méthode de la superclasse
    Material::disable();
}


/**
 * Cette méthode supprime les ressources allouées
 */
AmbiantMaterial::~AmbiantMaterial()
{
}
