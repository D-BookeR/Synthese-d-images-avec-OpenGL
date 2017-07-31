#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <utils.h>
#include <GroundMaterial.h>


/**
 * Constructeur
 */
GroundMaterial::GroundMaterial() : Material("GroundMaterial")
{
    // créer la texture contenant le bruit de Perlin
    m_NoiseTexture = new NoiseValue2D(256, 256, 6);
    m_NoiseTextureLoc = -1;

    // compiler le shader
    compileShader();
}


/**
 * retourne le source du Vertex Shader
 */
std::string GroundMaterial::getVertexShader()
{
    const char* srcVertexShader =
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
    return srcVertexShader;
}


/**
 * retourne le source du Fragment Shader
 */
std::string GroundMaterial::getFragmentShader()
{
    const char* srcFragmentShader =
        "#version 300 es\n"
        "precision mediump float;\n"
        "in vec2 frgTexCoord;\n"
        "uniform sampler2D txBruit;\n"
        "const float pi = 3.14159;\n"
        "const vec3 darkwood = vec3(0.7, 0.3, 0.2);\n"
        "const vec3 lightwood = vec3(0.9, 0.7, 0.4);\n"
        "const float frequency = 50.0;\n"
        "const float narrowness = 10.0;\n"
        "const float turbulence = 0.1;\n"
        "out vec4 glFragColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    // valeur du bruit en ce point\n"
        "    float noise = texture(txBruit, frgTexCoord).r * turbulence;\n"
        "    // rayon des vagues = distance au centre + bruit\n"
        "    float radius = distance(frgTexCoord, vec2(0.5, 0.5)) + noise;\n"
        "    // on construit des vagues à partir du rayon\n"
        "    float rate = pow(abs(sin(frequency*radius*pi)), narrowness);\n"
        "    // la couleur varie entre clair et foncé\n"
        "    glFragColor = vec4(mix(lightwood, darkwood, rate), 1.0);\n"
        "}";
    return srcFragmentShader;
}


/**
 * compile ou recompile le shader
 */
void GroundMaterial::compileShader()
{
    // appeler la méthode de la superclasse
    Material::compileShader();

    // déterminer où sont les variables uniform
    m_NoiseTextureLoc = glGetUniformLocation(m_ShaderId, "txBruit");
}


/**
 * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
 * @return le VBOset du matériau
 */
VBOset* GroundMaterial::createVBOset()
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
void GroundMaterial::enable(mat4 mat4Projection, mat4 mat4ModelView)
{
    // appeler la méthode de la superclasse
    Material::enable(mat4Projection, mat4ModelView);

    // activer les textures chacune sur son unité
    m_NoiseTexture->setTextureUnit(GL_TEXTURE0, m_NoiseTextureLoc);
}


/**
 * Cette méthode désactive le matériau
 */
void GroundMaterial::disable()
{
    // désactiver les textures
    m_NoiseTexture->setTextureUnit(GL_TEXTURE0);

    // appeler la méthode de la superclasse
    Material::disable();
}


/**
 * Cette méthode supprime les ressources allouées
 */
GroundMaterial::~GroundMaterial()
{
    delete m_NoiseTexture;
}
