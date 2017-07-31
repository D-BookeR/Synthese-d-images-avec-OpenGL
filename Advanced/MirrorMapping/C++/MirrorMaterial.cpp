#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <utils.h>
#include <MirrorMaterial.h>

/**
 * Cette fonction définit un matériau pour dessiner le maillage
 * @param texture : instance de TextureCube
 */
MirrorMaterial::MirrorMaterial(TextureCube* texture) : Material("MirrorMaterial")
{
    // mémoriser la texture
    m_Texture = texture;
    m_TextureLoc = -1;

    // compiler le shader
    compileShader();
}


/**
 * retourne le source du Vertex Shader
 */
std::string MirrorMaterial::getVertexShader()
{
    const char* srcVertexShader =
        "#version 300 es\n"
        "in vec3 glVertex;\n"
        "in vec3 glNormal;\n"
        "out vec3 frgDirection;\n"
        "uniform mat4 mat4ModelViewTeapot;\n"
        "uniform mat3 mat3NormalTeapot;\n"
        "uniform mat3 mat3NormalScene;\n"
        "uniform mat4 mat4Projection;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = mat4Projection * mat4ModelViewTeapot * vec4(glVertex, 1.0);\n"
        "    vec3 Vue = vec3(0,0,-1);\n"
        "    frgDirection = mat3NormalScene * reflect(Vue, mat3NormalTeapot * glNormal);\n"
        "}";
    return srcVertexShader;
}


/**
 * retourne le source du Fragment Shader
 */
std::string MirrorMaterial::getFragmentShader()
{
    const char* srcFragmentShader =
        "#version 300 es\n"
        "precision mediump float;\n"
        "in vec3 frgDirection;\n"
        "uniform samplerCube txCube;\n"
        "out vec4 glFragColor;\n"
        "void main()\n"
        "{\n"
        "    //DEBUG glFragColor = vec4(frgDirection, 1);return;\n"
        "    // couleur extraite de la texture cube\n"
        "    glFragColor = texture(txCube, frgDirection);\n"
        "}";
    return srcFragmentShader;
}


/**
 * recompile le shader du matériau
 */
void MirrorMaterial::compileShader()
{
    // appeler la méthode de la superclasse
    Material::compileShader();

    // déterminer où sont les variables uniform
    m_MatProjectionLoc       = glGetUniformLocation(m_ShaderId, "mat4Projection");
    m_MatModelViewTeapotLoc = glGetUniformLocation(m_ShaderId, "mat4ModelViewTeapot");
    m_MatNormalSceneLoc      = glGetUniformLocation(m_ShaderId, "mat3NormalScene");
    m_MatNormalTeapotLoc    = glGetUniformLocation(m_ShaderId, "mat3NormalTeapot");
    m_TextureLoc             = glGetUniformLocation(m_ShaderId, "txCube");
}


/**
 * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
 * @return le VBOset du matériau
 */
VBOset* MirrorMaterial::createVBOset()
{
    // créer le VBOset et spécifier les noms des attribute nécessaires à ce matériau
    VBOset* vboset = Material::createVBOset();
    vboset->addAttribute(MeshVertex::ID_ATTR_NORMAL, Utils::VEC3, "glNormal");
    return vboset;
}


/**
 * Cette méthode active le matériau : met en place son shader,
 * fournit les variables uniform qu'il demande
 * @param mat4Projection : mat4 contenant la projection
 * @param mat4ModelViewTeapot : mat4 contenant la transformation vers la caméra
 */
void MirrorMaterial::enable(mat4 mat4Projection, mat4 mat4ModelViewTeapot)
{
    // activer le shader
    glUseProgram(m_ShaderId);

    // fournir les matrices MV et P
    mat4::glUniformMatrix(m_MatModelViewTeapotLoc, mat4ModelViewTeapot);
    mat4::glUniformMatrix(m_MatProjectionLoc, mat4Projection);

    // calculer et fournir la matrice mat3NormalTheiere au shader
    mat3 mat3NormalTeapot = mat3::create();
    mat3::fromMat4(mat3NormalTeapot, mat4ModelViewTeapot);
    mat3::transpose(mat3NormalTeapot, mat3NormalTeapot);
    mat3::invert(mat3NormalTeapot, mat3NormalTeapot);
    mat3::glUniformMatrix(m_MatNormalTeapotLoc, mat3NormalTeapot);

    // activer la texture sur l'unité 0
    m_Texture->setTextureUnit(GL_TEXTURE0, m_TextureLoc);
}


/**
 * indique au matériau quelle est la transformation de la scène afin
 * de viser le point correct dans la skybox qui se réfléchit sur le matériau
 * @param mat4ModelViewScene : matrice ModelView utilisée pour dessiner la skybox
 */
void MirrorMaterial::setModelViewScene(mat4 mat4ModelViewScene)
{
    // calculer la matrice mat3NormalScene
    mat3 mat3NormalScene = mat3::create();
    mat3::fromMat4(mat3NormalScene, mat4ModelViewScene);
    mat3::transpose(mat3NormalScene, mat3NormalScene);
    // pas d'inversion car c'est une matrice pour aller dans le repère de la scène

    // fournir la matrice mat3NormalScene au shader
    glUseProgram(m_ShaderId);
    mat3::glUniformMatrix(m_MatNormalSceneLoc, mat3NormalScene);
}



/**
 * Cette méthode désactive le matériau
 */
void MirrorMaterial::disable()
{
    // désactiver les textures
    m_Texture->setTextureUnit(GL_TEXTURE0, -1);

    // appeler la méthode de la superclasse
    Material::disable();
}


/**
 * Cette méthode supprime les ressources allouées
 */
MirrorMaterial::~MirrorMaterial()
{
}
