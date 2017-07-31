#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>
#include <iomanip>

#include <utils.h>
#include <Texture3LightsMaterial.h>

/**
 * Cette classe définit un matériau pour dessiner le maillage
 * @param texture : fournir une Texture2D définissant la couleur diffuse du matériau
 * @param specular : fournir un vec3 définissant la couleur spéculaire du matériau
 * @param shininess : aspect rugueux ou lisse
 */
Texture3LightsMaterial::Texture3LightsMaterial(Texture2D* texture, vec3 specular, float shininess) :
    Material("Texture3LightsMaterial")
{
    // caractéristiques du matériau
    m_Texture = texture;
    m_SpecularColor = specular;
    m_Shininess = shininess;

    // coordonnées des sources de lumières dans le repère global
    m_LightsPositions.push_back(vec3::fromValues( 8.0,  7.0,  1.0));
    m_LightsPositions.push_back(vec3::fromValues(-6.0,  5.0, -4.0));
    m_LightsPositions.push_back(vec3::fromValues( 2.0,  2.0, -9.0));

    // intensités des sources de lumière
    m_LightsColors.push_back(vec3::fromValues(0.8, 0.8, 0.8));
    m_LightsColors.push_back(vec3::fromValues(0.6, 0.6, 0.6));
    m_LightsColors.push_back(vec3::fromValues(0.5, 0.5, 0.5));

    // compiler le shader
    compileShader();
}


/**
 * retourne le source du Vertex Shader
 */
std::string Texture3LightsMaterial::getVertexShader()
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
        "in vec2 glTexCoord;\n"
        "\n"
        "// interpolation vers les fragments\n"
        "out vec4 frgPosition;\n"
        "out vec3 frgNormal;\n"
        "out vec2 frgTexCoord;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    frgPosition = mat4ModelView * vec4(glVertex, 1.0);\n"
        "    gl_Position = mat4Projection * frgPosition;\n"
        "    frgNormal = mat3Normal * glNormal;\n"
        "    frgTexCoord = glTexCoord;\n"
        "}";
    return srcVertexShader;
}


/**
 * retourne le source du Fragment Shader
 */
std::string Texture3LightsMaterial::getFragmentShader()
{
    std::ostringstream srcFragmentShader;
    srcFragmentShader << "#version 300 es\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "// interpolation vers les fragments\n";
    srcFragmentShader << "precision mediump float;\n";
    srcFragmentShader << "in vec4 frgPosition;\n";
    srcFragmentShader << "in vec3 frgNormal;\n";
    srcFragmentShader << "in vec2 frgTexCoord;\n";
    srcFragmentShader << "out vec4 glFragColor;\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "// sources de lumière\n";
    srcFragmentShader << "uniform vec3 LampesPositions["<<m_LightsPositions.size()<<"];\n";
    srcFragmentShader << "uniform vec3 LampesCouleurs["<<m_LightsPositions.size()<<"];\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "// caractéristiques du matériau\n";
    srcFragmentShader << "uniform sampler2D txColor;\n";
    srcFragmentShader << "const vec3 Ks = "<<vec3::str(m_SpecularColor)<<";\n";
    srcFragmentShader << "const float ns = "<<std::setprecision(1)<<std::fixed<<m_Shininess<<";\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "void main()\n";
    srcFragmentShader << "{\n";
    srcFragmentShader << "    // récupération de la couleur diffuse\n";
    srcFragmentShader << "    vec3 Kd = texture(txColor, frgTexCoord).rgb;\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "    // normale normalisée (après interpolation)\n";
    srcFragmentShader << "    vec3 N = normalize(frgNormal);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "    // direction opposée du vecteur Vue au niveau du fragment\n";
    srcFragmentShader << "    vec3 mV = normalize(frgPosition.xyz);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "    // direction du reflet du vecteur Vue\n";
    srcFragmentShader << "    vec3 R = reflect(mV, N);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "    // couleur finale = cumul des éclairements\n";
    srcFragmentShader << "    vec3 sum = vec3(0.0, 0.0, 0.0);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "    // boucle sur les deux lampes\n";
    srcFragmentShader << "    for (int i=0; i<"<<m_LightsPositions.size()<<"; i++) {\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "        // vecteur allant du point à la lumière\n";
    srcFragmentShader << "        vec3 L = normalize(LampesPositions[i] - frgPosition.xyz);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "        // produit scalaire entre L et la normale\n";
    srcFragmentShader << "        float dotNL = clamp(dot(N, L), 0.0, 1.0);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "        // alignement du reflet de la vue avec la lampe\n";
    srcFragmentShader << "        float dotRL = clamp(dot(R,L), 0.0, 1.0);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "        // cumul des contributions lumineuses\n";
    srcFragmentShader << "        sum += LampesCouleurs[i] * (Kd*dotNL + Ks*pow(dotRL, ns));\n";
    srcFragmentShader << "    }\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "    glFragColor = vec4(sum, 1.0);\n";
    srcFragmentShader << "}";
    return srcFragmentShader.str();
}


/**
 * compile ou recompile le shader
 */
void Texture3LightsMaterial::compileShader()
{
    // appeler la méthode de la superclasse
    Material::compileShader();

    // déterminer où sont les variables uniform
    m_LightsPositionsLoc = glGetUniformLocation(m_ShaderId, "LampesPositions");
    m_LightsColorsLoc    = glGetUniformLocation(m_ShaderId, "LampesCouleurs");
    m_TextureLoc         = glGetUniformLocation(m_ShaderId, "txColor");
}


/**
 * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
 * @return le VBOset du matériau
 */
VBOset* Texture3LightsMaterial::createVBOset()
{
    // créer le VBOset et spécifier les noms des attribute nécessaires à ce matériau
    VBOset* vboset = Material::createVBOset();
    vboset->addAttribute(MeshVertex::ID_ATTR_NORMAL,   Utils::VEC3, "glNormal");
    vboset->addAttribute(MeshVertex::ID_ATTR_TEXCOORD, Utils::VEC2, "glTexCoord");
    return vboset;
}


/**
 * Cette méthode active le matériau : met en place son shader,
 * fournit les variables uniform qu'il demande
 * @param mat4Projection : mat4 contenant la projection
 * @param mat4ModelView : mat4 contenant la transformation vers la caméra
 */
void Texture3LightsMaterial::enable(mat4 mat4Projection, mat4 mat4ModelView)
{
    // appeler la méthode de la superclasse
    Material::enable(mat4Projection, mat4ModelView);

    // initialisation des lampes
    vec3::glUniform(m_LightsPositionsLoc, m_LightsPositions);
    vec3::glUniform(m_LightsColorsLoc,  m_LightsColors);

    // activer la texture sur l'unité 0
    m_Texture->setTextureUnit(GL_TEXTURE0, m_TextureLoc);
}


/**
 * Cette méthode désactive le matériau
 */
void Texture3LightsMaterial::disable()
{
    // désactiver les textures
    m_Texture->setTextureUnit(GL_TEXTURE0);

    // appeler la méthode de la superclasse
    Material::disable();
}


/**
 * Cette méthode supprime les ressources allouées
 */
Texture3LightsMaterial::~Texture3LightsMaterial()
{
}
