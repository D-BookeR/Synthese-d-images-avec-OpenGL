#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <utils.h>
#include <MultipleTextureMaterial.h>


/**
 * Constructeur
 * @param texture sur laquelle est basé le matériau
 * @param columns nombre de sous-images horizontalement = nombre de colonnes
 * @param lines nombre de sous-images verticalement = nombre de lignes
 * @param delay temps entre deux sous-images pour l'animation
 */
MultipleTextureMaterial::MultipleTextureMaterial(Texture2D* texture, int columns, int lines, float delay) :
    Material("MultipleTextureMaterial")
{
    // initialisations
    m_Texture = texture;
    m_TextureLoc = -1;

    // caractéristiques de l'image
    m_LineCount = lines;
    m_ColumnCount = columns;
    m_TotalNumber = columns * lines;
    m_Size = vec2::fromValues(columns, lines);
    m_Offset = vec2::create();
    m_Delay = delay;

    // compiler le shader
    compileShader();
}


/**
 * retourne le source du Vertex Shader
 */
std::string MultipleTextureMaterial::getVertexShader()
{
    const char* srcVertexShader =
        "#version 300 es\n"
        "in vec3 glVertex;\n"
        "in vec2 glTexCoord;\n"
        "uniform mat4 mat4ModelView;\n"
        "uniform mat4 mat4Projection;\n"
        "uniform vec2 offset;\n"
        "uniform vec2 size;\n"
        "\n"
        "// interpolation vers les fragments\n"
        "out vec4 frgPosition;\n"
        "out vec2 frgTexCoord;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    frgPosition = mat4ModelView * vec4(glVertex, 1.0);\n"
        "    gl_Position = mat4Projection * frgPosition;\n"
        "    // coordonnées de texture altérées pour viser une sous-image\n"
        "    frgTexCoord = (glTexCoord + offset) / size;\n"
        "}";
    return srcVertexShader;
}


/**
 * retourne le source du Fragment Shader
 */
std::string MultipleTextureMaterial::getFragmentShader()
{
    const char* srcFragmentShader =
        "#version 300 es\n"
        "precision mediump float;\n"
        "in vec4 frgPosition;\n"
        "in vec2 frgTexCoord;\n"
        "out vec4 glFragData[4];\n"
        "\n"
        "uniform sampler2D txColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    // accès à la texture\n"
        "    glFragData[0] = texture(txColor, frgTexCoord);\n"
        "    glFragData[1] = vec4(0.0);\n"
        "    glFragData[2] = vec4(frgPosition.xyz, 1.0);\n"
        "    glFragData[3] = vec4(1.0, 1.0, 1.0, 1.0);\n"
        "}";
    return srcFragmentShader;
}


/**
 * compile ou recompile le shader
 */
void MultipleTextureMaterial::compileShader()
{
    // appeler la méthode de la superclasse
    Material::compileShader();

    // déterminer où sont les variables uniform
    m_TextureLoc = glGetUniformLocation(m_ShaderId, "txColor");
    m_OffsetLoc  = glGetUniformLocation(m_ShaderId, "offset");
    m_SizeLoc    = glGetUniformLocation(m_ShaderId, "size");
}


/**
 * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
 * @return le VBOset du matériau
 */
VBOset* MultipleTextureMaterial::createVBOset()
{
    // créer le VBOset et spécifier les noms des attribute nécessaires à ce matériau
    VBOset* vboset = Material::createVBOset();
    vboset->addAttribute(MeshVertex::ID_ATTR_TEXCOORD, Utils::VEC2, "glTexCoord");
    return vboset;
}


/**
 * sélectionne la partie de la Texture2D correspondant au temps
 */
void MultipleTextureMaterial::update()
{
    // déterminer le numéro de l'image d'après le temps
    int number = (int)floor(Utils::Time / m_Delay) % m_TotalNumber;

    // déterminer la ligne et la colonne d'après le numéro
    int column = number % m_ColumnCount;
    int line = m_LineCount-1-(int)floor(number / m_ColumnCount);

    // changer le décalage d'image pour le shader
    vec2::set(m_Offset, column, line);
}


/**
 * Cette méthode active le matériau : met en place son shader,
 * fournit les variables uniform qu'il demande
 * @param mat4Projection : mat4 contenant la projection
 * @param mat4ModelView : mat4 contenant la transformation vers la caméra
 */
void MultipleTextureMaterial::enable(mat4 mat4Projection, mat4 mat4ModelView)
{
    // appeler la méthode de la superclasse
    Material::enable(mat4Projection, mat4ModelView);

    // mettre à jour l'image
    update();

    // spécifier le coefficient de teinte et le coefficient de transparence
    vec2::glUniform(m_OffsetLoc, m_Offset);
    vec2::glUniform(m_SizeLoc,  m_Size);

    // activer la texture sur l'unité 0
    m_Texture->setTextureUnit(GL_TEXTURE0, m_TextureLoc);
}


/**
 * Cette méthode désactive le matériau
 */
void MultipleTextureMaterial::disable()
{
    // désactiver les textures
    m_Texture->setTextureUnit(GL_TEXTURE0);

    // appeler la méthode de la superclasse
    Material::disable();
}


/**
 * Cette méthode supprime les ressources allouées
 */
MultipleTextureMaterial::~MultipleTextureMaterial()
{
}
