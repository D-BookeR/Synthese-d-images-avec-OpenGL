#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <utils.h>
#include <NormalMaterial.h>

/**
 * Cette fonction définit un matériau pour dessiner le maillage
 */
NormalMaterial::NormalMaterial() : Material("NormalMaterial")
{
    // charger les textures
    m_DiffuseTexture = new Texture2D("data/textures/BenCloward/metalSheet/diffuse.jpg", GL_LINEAR, GL_REPEAT);
    m_DiffuseTextureLoc = -1;
    m_NormalTexture = new Texture2D("data/textures/BenCloward/metalSheet/normal.jpg", GL_LINEAR, GL_REPEAT);
    m_NormalTextureLoc = -1;


    // compiler le shader
    compileShader();
}


/**
 * retourne le source du Vertex Shader
 */
std::string NormalMaterial::getVertexShader()
{
    const char* srcVertexShader =
        "#version 300 es\n"
        "// matrices de changement de repère\n"
        "uniform mat4 mat4ModelView;\n"
        "uniform mat4 mat4Projection;\n"
        "uniform mat3 mat3Normal;\n"
        "\n"
        "// VBOs contenant le repère de Frenet complet\n"
        "in vec3 glVertex;\n"
        "in vec3 glNormal;\n"
        "in vec3 glTangent;\n"
        "in vec2 glTexCoord;\n"
        "\n"
        "// variables à interpoler pour le fragment shader\n"
        "out vec4 frgPosition;\n"
        "out vec3 frgNormal;\n"
        "out vec3 frgTangent;\n"
        "out vec2 frgTexCoord;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    frgPosition = mat4ModelView * vec4(glVertex, 1.0);\n"
        "    gl_Position = mat4Projection * frgPosition;\n"
        "    frgNormal = mat3Normal * glNormal;\n"
        "    frgTangent = mat3Normal * glTangent;\n"
        "    frgTexCoord = glTexCoord * 5.0; // x5 pour réduire la taille de la texture\n"
        "}";
    return srcVertexShader;
}


/**
 * retourne le source du Fragment Shader
 */
std::string NormalMaterial::getFragmentShader()
{
    const char* srcFragmentShader =
        "#version 300 es\n"
        "precision mediump float;\n"
        "\n"
        "// variables interpolées venant du vertex shader\n"
        "in vec4 frgPosition;\n"
        "in vec3 frgNormal;\n"
        "in vec3 frgTangent;\n"
        "in vec2 frgTexCoord;\n"
        "out vec4 glFragColor;\n"
        "\n"
        "// textures diffuse et normale\n"
        "uniform sampler2D txDiffuse;\n"
        "uniform sampler2D txNormal;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    // matrice de passage du repère de Frenet à l'espace global\n"
        "    vec3 N = normalize(frgNormal);\n"
        "    vec3 T = normalize(frgTangent);\n"
        "    vec3 B = cross(N, T);\n"
        "    mat3 TBN = mat3(T, B, N);\n"
        "    \n"
        "    // vecteur normal dans le repère de Frenet\n"
        "    vec3 Nfrenet = texture(txNormal, frgTexCoord).xyz * 2.0 - 1.0;\n"
        "    \n"
        "    // changement de repère du vecteur normal extrait de la normal map\n"
        "    N = normalize(TBN * Nfrenet);\n"
        "    \n"
        "    // caractéristiques locales du matériau\n"
        "    vec3 Kd = texture(txDiffuse, frgTexCoord).rgb;\n"
        "    vec3 Ks = vec3(1.0, 1.0, 1.0);\n"
        "    float Ns = 120.0;\n"
        "    \n"
        "    // direction et couleur de la lumière\n"
        "    vec3 LightDirection = vec3(0.7, 0.3, 0.5);\n"
        "    vec3 LightColor = vec3(1.5, 1.5, 1.5);\n"
        "    \n"
        "    // contribution diffuse de Lambert\n"
        "    vec3 L = normalize(LightDirection);\n"
        "    float dotNL = clamp(dot(N, L), 0.0, 1.0);\n"
        "    \n"
        "    // contribution spéculaire de Phong\n"
        "    vec3 R = reflect(normalize(frgPosition.xyz), N);\n"
        "    float dotRL = clamp(dot(R, L), 0.0, 1.0);\n"
        "    \n"
        "    // couleur finale\n"
        "    glFragColor = vec4(LightColor*(Kd *dotNL + Ks*pow(dotRL, Ns)), 1.0);\n"
        "}";
    return srcFragmentShader;
}


/**
 * compile ou recompile le shader
 */
void NormalMaterial::compileShader()
{
    // appeler la méthode de la superclasse
    Material::compileShader();

    // déterminer où sont les variables uniform
    m_DiffuseTextureLoc = glGetUniformLocation(m_ShaderId, "txDiffuse");
    m_NormalTextureLoc = glGetUniformLocation(m_ShaderId, "txNormal");
}


/**
 * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
 * @return le VBOset du matériau
 */
VBOset* NormalMaterial::createVBOset()
{
    // créer le VBOset et spécifier les noms des attribute nécessaires à ce matériau
    VBOset* vboset = Material::createVBOset();
    vboset->addAttribute(MeshVertex::ID_ATTR_NORMAL,   Utils::VEC3, "glNormal");
    vboset->addAttribute(MeshVertex::ID_ATTR_TANGENT,  Utils::VEC3, "glTangent");
    vboset->addAttribute(MeshVertex::ID_ATTR_TEXCOORD, Utils::VEC2, "glTexCoord");

    return vboset;
}


/**
 * Cette méthode active le matériau : met en place son shader,
 * fournit les variables uniform qu'il demande
 * @param mat4Projection : mat4 contenant la projection
 * @param mat4ModelView : mat4 contenant la transformation vers la caméra
 */
void NormalMaterial::enable(mat4 mat4Projection, mat4 mat4ModelView)
{
    // appeler la méthode de la superclasse
    Material::enable(mat4Projection, mat4ModelView);

    // activer les textures chacune sur une unité différente
    m_DiffuseTexture->setTextureUnit(GL_TEXTURE0, m_DiffuseTextureLoc);
    m_NormalTexture->setTextureUnit(GL_TEXTURE1, m_NormalTextureLoc);
}


/**
 * Cette méthode désactive le matériau
 */
void NormalMaterial::disable()
{
    // désactiver les textures
    m_DiffuseTexture->setTextureUnit(GL_TEXTURE0);
    m_NormalTexture->setTextureUnit(GL_TEXTURE1);

    // appeler la méthode de la superclasse
    Material::disable();
}


/**
 * Cette méthode supprime les ressources allouées
 */
NormalMaterial::~NormalMaterial()
{
    Utils::deleteShaderProgram(m_ShaderId);
    delete m_DiffuseTexture;
    delete m_NormalTexture;
}
