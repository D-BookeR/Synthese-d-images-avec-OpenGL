#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <utils.h>
#include <AppleMaterial.h>


/**
 * Constructeur
 */
AppleMaterial::AppleMaterial() : Material("AppleMaterial")
{
    // créer la texture contenant le bruit de Perlin
    m_NoiseTexture = new NoiseValue2D(256, 256, 5);
    m_NoiseTextureLoc = -1;

    // charger la texture contenant le gradient
    m_TextureGradient = new Texture2D("data/textures/gradients/ciel.png");
    m_TextureGradientLoc = -1;

    // compiler le shader
    compileShader();
}


/**
 * retourne le source du Vertex Shader
 */
std::string AppleMaterial::getVertexShader()
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
std::string AppleMaterial::getFragmentShader()
{
    const char* srcFragmentShader =
        "#version 300 es\n"
        "precision mediump float;\n"
        "in vec2 frgTexCoord;\n"
        "uniform sampler2D txBruit;\n"
        "uniform sampler2D txGradient;\n"
        "out vec4 glFragColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    float noise = texture(txBruit, frgTexCoord).r;\n"
        "    glFragColor = texture(txGradient, vec2(noise,0));\n"
        "}";
    return srcFragmentShader;
}


/**
 * compile ou recompile le shader
 */
void AppleMaterial::compileShader()
{
    // appeler la méthode de la superclasse
    Material::compileShader();

    // déterminer où sont les variables uniform
    m_NoiseTextureLoc    = glGetUniformLocation(m_ShaderId, "txBruit");
    m_TextureGradientLoc = glGetUniformLocation(m_ShaderId, "txGradient");
}


/**
 * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
 * @return le VBOset du matériau
 */
VBOset* AppleMaterial::createVBOset()
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
void AppleMaterial::enable(mat4 mat4Projection, mat4 mat4ModelView)
{
    // appeler la méthode de la superclasse
    Material::enable(mat4Projection, mat4ModelView);

    // activer les textures sur des unités différentes
    m_NoiseTexture->setTextureUnit(GL_TEXTURE0, m_NoiseTextureLoc);
    m_TextureGradient->setTextureUnit(GL_TEXTURE1, m_TextureGradientLoc);
}


/**
 * Cette méthode désactive le matériau
 */
void AppleMaterial::disable()
{
    // désactiver les textures
    m_NoiseTexture->setTextureUnit(GL_TEXTURE0);
    m_TextureGradient->setTextureUnit(GL_TEXTURE1);

    // appeler la méthode de la superclasse
    Material::disable();
}


/**
 * Cette méthode supprime les ressources allouées
 */
AppleMaterial::~AppleMaterial()
{
    delete m_TextureGradient;
    delete m_NoiseTexture;
}
