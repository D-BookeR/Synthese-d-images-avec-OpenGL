// Définition de la classe TextureMaterial, une spécialisation de Material
// Ce matériau permet de dessiner en mode Multiple Render Targets

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <utils.h>
#include <TextureMaterial.h>
#include <VBOset.h>

/**
 * constructeur
 * @param texture du matériau
 */
TextureMaterial::TextureMaterial(Texture2D* texture) :
    Material("TextureMaterial")
{
    // texture
    m_Texture = texture;

    // compiler le shader
    compileShader();
}


/**
 * retourne le source du Vertex Shader
 */
std::string TextureMaterial::getVertexShader()
{
    return
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
        "    frgNormal   = mat3Normal * glNormal;\n"
        "    frgTexCoord = glTexCoord;\n"
        "}";
}


/**
 * retourne le source du Fragment Shader
 */
std::string TextureMaterial::getFragmentShader()
{
    std::ostringstream srcFragmentShader;
    srcFragmentShader << "#version 300 es\n";
    if (m_ClipPlaneOn) {
        srcFragmentShader << "\n";
        srcFragmentShader << "// plan de coupe\n";
        srcFragmentShader << "uniform vec4 ClipPlane;\n";
    }
    srcFragmentShader << "\n";
    srcFragmentShader << "// interpolation vers les fragments\n";
    srcFragmentShader << "precision mediump float;\n";
    srcFragmentShader << "in vec4 frgPosition;\n";
    srcFragmentShader << "in vec3 frgNormal;\n";
    srcFragmentShader << "in vec2 frgTexCoord;\n";
    srcFragmentShader << "out vec4 glFragData[4];\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "uniform sampler2D txColor;\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "void main()\n";
    srcFragmentShader << "{\n";
    if (m_ClipPlaneOn) {
        srcFragmentShader << "    // plan de coupe\n";
        srcFragmentShader << "    if (dot(frgPosition, ClipPlane) < 0.0) discard;\n";
        srcFragmentShader << "\n";
    }
    srcFragmentShader << "    glFragData[0] = texture(txColor, frgTexCoord);\n";
    srcFragmentShader << "    glFragData[1] = vec4(0.0);\n";
    srcFragmentShader << "    glFragData[2] = vec4(frgPosition.xyz, 1.0);\n";
    srcFragmentShader << "    glFragData[3] = vec4(frgNormal, 0.0);\n";
    srcFragmentShader << "}\n";
    return srcFragmentShader.str();
}


/**
 * recompile le shader du matériau
 */
void TextureMaterial::compileShader()
{
    // appeler la méthode de la superclasse
    Material::compileShader();

    // déterminer où sont les variables uniform spécifiques
    m_TextureLoc = glGetUniformLocation(m_ShaderId, "txColor");
}


/**
 * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
 * @return le VBOset du matériau
 */
VBOset* TextureMaterial::createVBOset()
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
void TextureMaterial::enable(mat4 mat4Projection, mat4 mat4ModelView)
{
    // appeler la méthode de la superclasse
    Material::enable(mat4Projection, mat4ModelView);

    // activer la texture sur l'unité 0
    m_Texture->setTextureUnit(GL_TEXTURE0, m_TextureLoc);
}


/**
 * Cette méthode désactive le matériau
 */
void TextureMaterial::disable()
{
    // désactiver la texture
    m_Texture->setTextureUnit(GL_TEXTURE0);

    // appeler la méthode de la superclasse
    Material::disable();
}


/**
 * Cette méthode supprime les ressources allouées
 */
TextureMaterial::~TextureMaterial()
{
}
