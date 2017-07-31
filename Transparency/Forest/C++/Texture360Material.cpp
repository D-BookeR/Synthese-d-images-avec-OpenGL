#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <utils.h>
#include <Texture360Material.h>


/**
 * Constructeur
 * @param texture360 sur laquelle est basé le matériau
 */
Texture360Material::Texture360Material(Texture360* texture360) :
    Material("Texture360Material")
{
    // charger la texture
    m_Texture360 = texture360;
    m_TextureLoc = -1;
    m_Texture = nullptr;

    // modification de la couleur et transparence
    m_ColorCoefficient = 1.0;
    m_AlphaCoefficient = 1.0;

    // compiler le shader
    compileShader();
}


/**
 * retourne le source du Vertex Shader
 */
std::string Texture360Material::getVertexShader()
{
    const char* srcVertexShader =
        "#version 300 es\n"
        "in vec3 glVertex;\n"
        "in vec2 glTexCoord;\n"
        "uniform mat4 mat4ModelView;\n"
        "uniform mat4 mat4Projection;\n"
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
std::string Texture360Material::getFragmentShader()
{
    const char* srcFragmentShader =
        "#version 300 es\n"
        "precision mediump float;\n"
        "in vec2 frgTexCoord;\n"
        "\n"
        "uniform sampler2D txColor;\n"
        "uniform float colorCoefficient;\n"
        "uniform float alphaCoefficient;\n"
        "\n"
        "out vec4 glFragColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    // accès à la texture\n"
        "    vec4 color = texture(txColor, frgTexCoord);\n"
        "    // modulation de la couleur et la transparence\n"
        "    glFragColor = vec4(color.rgb*colorCoefficient, color.a*alphaCoefficient);\n"
        "}";
    return srcFragmentShader;
}


/**
 * compile ou recompile le shader
 */
void Texture360Material::compileShader()
{
    // appeler la méthode de la superclasse
    Material::compileShader();

    // déterminer où sont les variables uniform
    m_TextureLoc          = glGetUniformLocation(m_ShaderId, "txColor");
    m_ColorCoefficientLoc = glGetUniformLocation(m_ShaderId, "colorCoefficient");
    m_AlphaCoefficientLoc = glGetUniformLocation(m_ShaderId, "alphaCoefficient");
}


/**
 * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
 * @return le VBOset du matériau
 */
VBOset* Texture360Material::createVBOset()
{
    // créer le VBOset et spécifier les noms des attribute nécessaires à ce matériau
    VBOset* vboset = Material::createVBOset();
    vboset->addAttribute(MeshVertex::ID_ATTR_TEXCOORD, Utils::VEC2, "glTexCoord");
    return vboset;
}


/**
 * modifie les coefficients de couleur et de transparence
 * @param colorCoefficient : float de 0 à 1 multiplié par la couleur
 * @param alphaCoefficient : float de 0 à 1 multiplié par le coefficient alpha
 */
void Texture360Material::setCoefficients(float colorCoefficient, float alphaCoefficient)
{
    m_ColorCoefficient = colorCoefficient;
    m_AlphaCoefficient   = alphaCoefficient;
}


/**
 * sélectionne la Texture2D correspondant à l'angle
 * @param angle : float entre 0 et 1
 */
void Texture360Material::select(float angle)
{
    m_Texture = m_Texture360->select(angle);
}


/**
 * Cette méthode active le matériau : met en place son shader,
 * fournit les variables uniform qu'il demande
 * @param mat4Projection : mat4 contenant la projection
 * @param mat4ModelView : mat4 contenant la transformation vers la caméra
 */
void Texture360Material::enable(mat4 mat4Projection, mat4 mat4ModelView)
{
    // appeler la méthode de la superclasse
    Material::enable(mat4Projection, mat4ModelView);

    // spécifier le coefficient de teinte et le coefficient de transparence
    glUniform1f(m_ColorCoefficientLoc, m_ColorCoefficient);
    glUniform1f(m_AlphaCoefficientLoc,   m_AlphaCoefficient);

    // activer la texture sur l'unité 0
    m_Texture->setTextureUnit(GL_TEXTURE0, m_TextureLoc);
}


/**
 * Cette méthode désactive le matériau
 */
void Texture360Material::disable()
{
    // désactiver les textures
    m_Texture->setTextureUnit(GL_TEXTURE0);

    // appeler la méthode de la superclasse
    Material::disable();
}


/**
 * Cette méthode supprime les ressources allouées
 */
Texture360Material::~Texture360Material()
{
}
