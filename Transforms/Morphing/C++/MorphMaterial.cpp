// Définition de la classe MorphMaterial, une spécialisation de Material
// Ce matériau permet d'appliquer une déformation par cible
// C'est une simplification de DeferredShadingMaterial

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

#include <utils.h>
#include <MorphMaterial.h>
#include <VBOset.h>



/**
 * constructeur
 * @param diffuse : nom d'une texture
 * @param Ks : vec3
 * @param Ns : poli du matériau
 */
MorphMaterial::MorphMaterial(std::string diffuse, vec3 Ks, float Ns) :
    Material("MorphMaterial")
{
    // texture diffuse
    m_TxDiffuse = new Texture2D(diffuse, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT);

    // couleur spéculaire
    m_Ks = Ks;
    m_Ns = Ns;

    // coefficient de morphing
    m_MorphCoef = 0.0;

    // compiler le shader
    compileShader();
}


/** destructeur */
MorphMaterial::~MorphMaterial()
{
    // libérer les textures qui ont été chargées
    if (m_TxDiffuse != nullptr) delete m_TxDiffuse;
}


/**
 * retourne le source du Vertex Shader
 */
std::string MorphMaterial::getVertexShader()
{
    // début du vertex shader
    std::ostringstream srcVertexShader;
    srcVertexShader << "#version 300 es\n";
    srcVertexShader << "\n";
    srcVertexShader << "// paramètres uniform\n";
    srcVertexShader << "uniform mat4 mat4ModelView;\n";
    srcVertexShader << "uniform mat4 mat4Projection;\n";
    srcVertexShader << "uniform mat3 mat3Normal;\n";
    srcVertexShader << "\n";
    srcVertexShader << "// attributs de sommets\n";
    srcVertexShader << "in vec3 glVertex;\n";
    srcVertexShader << "in vec3 glNormal;\n";
    srcVertexShader << "in vec2 glTexCoord;\n";
    srcVertexShader << "\n";
    srcVertexShader << "// morph target\n";
    srcVertexShader << "uniform float morphcoef;\n";
    srcVertexShader << "in vec3 glVertex1;\n";
    srcVertexShader << "in vec3 glNormal1;\n";
    srcVertexShader << "\n";
    srcVertexShader << "// interpolation vers les fragments\n";
    srcVertexShader << "out vec4 frgPosition;\n";
    srcVertexShader << "out vec3 frgNormal;\n";
    srcVertexShader << "out vec2 frgTexCoord;\n";
    srcVertexShader << "\n";
    srcVertexShader << "void main()\n";
    srcVertexShader << "{\n";
    srcVertexShader << "    // interpolation entre référence et morph target\n";
    srcVertexShader << "    vec3 vertex = mix(glVertex, glVertex1, morphcoef);\n";
    srcVertexShader << "    vec3 normal = mix(glNormal, glNormal1, morphcoef);\n";
    srcVertexShader << "\n";
    srcVertexShader << "    // transformations pour l'affichage\n";
    srcVertexShader << "    frgPosition = mat4ModelView * vec4(vertex, 1.0);\n";
    srcVertexShader << "    gl_Position = mat4Projection * frgPosition;\n";
    srcVertexShader << "    frgNormal = mat3Normal * normal;\n";
    srcVertexShader << "    frgTexCoord = glTexCoord;\n";
    srcVertexShader << "}";
    return srcVertexShader.str();
}


/**
 * retourne le source du Fragment Shader
 */
std::string MorphMaterial::getFragmentShader()
{
    std::ostringstream srcFragmentShader;
    srcFragmentShader.setf(std::ios::fixed, std::ios::floatfield);
    srcFragmentShader << "#version 300 es\n";
    srcFragmentShader << "precision mediump float;\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "// coordonnées et normale du fragment\n";
    srcFragmentShader << "in vec4 frgPosition;\n";
    srcFragmentShader << "in vec3 frgNormal;\n";
    srcFragmentShader << "in vec2 frgTexCoord;\n";
    srcFragmentShader << "out vec4 glFragData[4];\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "// caractéristiques du matériau\n";
    srcFragmentShader << "uniform sampler2D txDiffuse;\n";
    srcFragmentShader << "const vec3 Ks = "<<vec3::str(m_Ks)<<";\n";
    srcFragmentShader << "const float Ns = "<<m_Ns<<";\n";
    // plan de coupe
    if (m_ClipPlaneOn) {
        srcFragmentShader << "\n";
        srcFragmentShader << "// plan de coupe\n";
        srcFragmentShader << "uniform vec4 ClipPlane;\n";
    }
    srcFragmentShader << "\n";
    srcFragmentShader << "void main()\n";
    srcFragmentShader << "{\n";
    // plan de coupe
    if (m_ClipPlaneOn) {
        srcFragmentShader << "    if (dot(frgPosition, ClipPlane) < 0.0) discard;\n";
    }
    srcFragmentShader << "    vec4 Kd = texture(txDiffuse, frgTexCoord);\n";
    srcFragmentShader << "    // remplir les buffers avec les informations\n";
    srcFragmentShader << "    glFragData[0] = Kd;\n";
    srcFragmentShader << "    glFragData[1] = vec4(Ks, Ns);\n";
    srcFragmentShader << "    glFragData[2] = vec4(frgPosition.xyz, 1.0);\n";
    srcFragmentShader << "    glFragData[3] = vec4(normalize(frgNormal), 0.0);\n";
    srcFragmentShader << "}";
    return srcFragmentShader.str();
}


/**
 * recompile le shader du matériau
 */
void MorphMaterial::compileShader()
{
    // appeler la méthode de la superclasse
    Material::compileShader();

    // déterminer où sont les variables uniform spécifiques
    m_TxDiffuseLoc = glGetUniformLocation(m_ShaderId, "txDiffuse");
    m_MorphCoefLoc = glGetUniformLocation(m_ShaderId, "morphcoef");
}


/**
 * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
 * @return le VBOset du matériau, voir Mesh.cpp
 */
VBOset* MorphMaterial::createVBOset()
{
    // créer le VBOset et spécifier les noms des attribute nécessaires à ce matériau
    VBOset* vboset = Material::createVBOset();
    vboset->addAttribute(MeshVertex::ID_ATTR_COLOR,    Utils::VEC4, "glColor");
    vboset->addAttribute(MeshVertex::ID_ATTR_TEXCOORD, Utils::VEC2, "glTexCoord");
    vboset->addAttribute(MeshVertex::ID_ATTR_VERTEX1,  Utils::VEC3, "glVertex1");
    vboset->addAttribute(MeshVertex::ID_ATTR_NORMAL,   Utils::VEC3, "glNormal");
    vboset->addAttribute(MeshVertex::ID_ATTR_NORMAL1,  Utils::VEC3, "glNormal1");
    return vboset;
}


/**
 * Cette méthode active le matériau : met en place son shader,
 * fournit les variables uniform qu'il demande
 * @param mat4Projection : mat4 contenant la projection
 * @param mat4ModelView : mat4 contenant la transformation vers la caméra
 */
void MorphMaterial::enable(mat4 mat4Projection, mat4 mat4ModelView)
{
    // appeler la méthode de la superclasse
    Material::enable(mat4Projection, mat4ModelView);

    // activer la texture diffuse
    m_TxDiffuse->setTextureUnit(GL_TEXTURE0, m_TxDiffuseLoc);

    // fournir le coefficient de morphing
    glUniform1f(m_MorphCoefLoc, m_MorphCoef);
}


/**
 * Cette méthode désactive le matériau
 * NB : le shader doit être activé
 */
void MorphMaterial::disable()
{
    // désactiver la texture diffuse
    m_TxDiffuse->setTextureUnit(GL_TEXTURE0);

    // appeler la méthode de la superclasse
    Material::disable();
}


/**
 * définit la valeur du morph pour changer la forme de l'objet
 * @param coef entre 0 et 1
 */
void MorphMaterial::setMorphCoef(float coef)
{
    m_MorphCoef = coef;
}
