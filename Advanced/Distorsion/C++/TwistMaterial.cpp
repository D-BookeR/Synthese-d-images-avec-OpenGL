#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <utils.h>
#include <TwistMaterial.h>


/**
 * Constructeur
 */
TwistMaterial::TwistMaterial() : Material("TwistMaterial")
{
    // charger la texture
    m_Texture = new Texture2D("data/textures/earth/earth_map.jpg");
    m_TextureLoc = -1;

    // compiler le shader
    compileShader();
}


/**
 * retourne le source du Vertex Shader
 */
std::string TwistMaterial::getVertexShader()
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
std::string TwistMaterial::getFragmentShader()
{
    const char* srcFragmentShader =
        "#version 300 es\n"
        "precision mediump float;\n"
        "in vec2 frgTexCoord;\n"
        "out vec4 glFragColor;\n"
        "\n"
        "uniform sampler2D txColor;\n"
        "uniform float MaxAngle;\n"
        "\n"
        "const float PI = 3.1415927;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    // modification des coordonnées de texture\n"
        "    vec2 texcoord = frgTexCoord;\n"
        "\n"
        "    // la rotation est d'autant plus forte qu'on est proche du centre\n"
        "    float strength = pow(sin(texcoord.s * PI) * sin(texcoord.t * PI), 3.0);\n"
        "    float angle = radians(MaxAngle) * strength;\n"
        "\n"
        "    // produire une matrice de rotation 2D\n"
        "    float cosa = cos(angle);\n"
        "    float sina = sin(angle);\n"
        "    mat2 rotation = mat2(cosa,sina,-sina,cosa);\n"
        "\n"
        "    // appliquer cette rotation aux coordonnées de texture\n"
        "    const vec2 center = vec2(0.5, 0.5);\n"
        "    texcoord = rotation * (texcoord-center) + center;\n"
        "\n"
        "    glFragColor = texture(txColor, texcoord);\n"
        "}";
    return srcFragmentShader;
}


/**
 * compile ou recompile le shader
 */
void TwistMaterial::compileShader()
{
    // appeler la méthode de la superclasse
    Material::compileShader();

    // déterminer où sont les variables uniform
    m_TextureLoc   = glGetUniformLocation(m_ShaderId, "txColor");
    m_MaxAngleLoc  = glGetUniformLocation(m_ShaderId, "MaxAngle");
}


/**
 * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
 * @return le VBOset du matériau
 */
VBOset* TwistMaterial::createVBOset()
{
    // créer le VBOset et spécifier les noms des attribute nécessaires à ce matériau
    VBOset* vboset = Material::createVBOset();
    vboset->addAttribute(MeshVertex::ID_ATTR_TEXCOORD, Utils::VEC2, "glTexCoord");
    return vboset;
}


/**
 * modifie l'angle maximal de la torsion
 * @param anglemax en degrés
 */
void TwistMaterial::setMaxAngle(float anglemax)
{
    // activer le shader
    glUseProgram(m_ShaderId);

    // fournir le nouvel angle
    glUniform1f(m_MaxAngleLoc, anglemax);

    // désactiver le shader
    glUseProgram(0);
}


/**
 * Cette méthode active le matériau : met en place son shader,
 * fournit les variables uniform qu'il demande
 * @param mat4Projection : mat4 contenant la projection
 * @param mat4ModelView : mat4 contenant la transformation vers la caméra
 */
void TwistMaterial::enable(mat4 mat4Projection, mat4 mat4ModelView)
{
    // appeler la méthode de la superclasse
    Material::enable(mat4Projection, mat4ModelView);

    // activer la texture sur l'unité 0
    m_Texture->setTextureUnit(GL_TEXTURE0, m_TextureLoc);
}


/**
 * Cette méthode désactive le matériau
 */
void TwistMaterial::disable()
{
    // désactiver les textures
    m_Texture->setTextureUnit(GL_TEXTURE0);

    // appeler la méthode de la superclasse
    Material::disable();
}


/**
 * Cette méthode supprime les ressources allouées
 */
TwistMaterial::~TwistMaterial()
{
    delete m_Texture;
}
