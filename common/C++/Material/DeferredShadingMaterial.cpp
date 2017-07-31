/**
 * Définition de la classe DeferredShadingMaterial, une spécialisation de Material
 * Ce matériau est destiné à dessiner en mode Multiple Render Targets
 */

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

#include <utils.h>
#include <DeferredShadingMaterial.h>
#include <VBOset.h>

/**
 * initialisations communes à tous les constructeurs
 */
void DeferredShadingMaterial::init()
{
    // couleurs
    m_Kd = vec4::fromValues(1.0, 0.0, 1.0, 1.0);
    m_Ks = vec3::fromValues(0.5, 0.5, 0.5);
    m_Ns = 60.0;

    // textures
    m_TxDiffuse = nullptr;
    m_TxDiffuseLoc = -1;
    m_TxSpecular = nullptr;
    m_TxSpecularLoc = -1;
}


/**
 * Constructeur de la classe DeferredShadingMaterial. Ce matériau mémorise dans un FBO MRT les informations
 * nécessaires pour appliquer le modèle de Phong ultérieurement : position, normale, couleur.
 * @param Kd : vec3
 */
DeferredShadingMaterial::DeferredShadingMaterial(vec4 Kd) :
    Material("DeferredShadingMaterial")
{
    init();

    m_Kd = Kd;
    m_KdIsInterpolated = false;
    m_Ks = vec3::fromValues(0,0,0);
    m_Ns = -1.0;

    // compiler le shader
    compileShader();
}


/**
 * Constructeur de la classe DeferredShadingMaterial. Ce matériau mémorise dans un FBO MRT les informations
 * nécessaires pour appliquer le modèle de Phong ultérieurement : position, normale, couleur.
 * @param Ks : vec3
 * @param Ns : poli du matériau
 */
DeferredShadingMaterial::DeferredShadingMaterial(vec3 Ks, float Ns) :
    Material("DeferredShadingMaterial")
{
    init();

    m_KdIsInterpolated = true;
    m_Ks = Ks;
    m_Ns = Ns;

    // compiler le shader
    compileShader();
}


/**
 * Constructeur de la classe DeferredShadingMaterial. Ce matériau mémorise dans un FBO MRT les informations
 * nécessaires pour appliquer le modèle de Phong ultérieurement : position, normale, couleur.
 * @param Kd : vec3
 * @param Ks : vec3
 * @param Ns : poli du matériau
 */
DeferredShadingMaterial::DeferredShadingMaterial(vec4 Kd, vec3 Ks, float Ns) :
    Material("DeferredShadingMaterial")
{
    init();

    m_Kd = Kd;
    m_KdIsInterpolated = false;
    m_Ks = Ks;
    m_Ns = Ns;

    // compiler le shader
    compileShader();
}


/**
 * Constructeur de la classe DeferredShadingMaterial. Ce matériau mémorise dans un FBO MRT les informations
 * nécessaires pour appliquer le modèle de Phong ultérieurement : position, normale, couleur.
 * @param diffuse : nom d'une texture
 * @param Ks : vec3
 * @param Ns : poli du matériau
 */
DeferredShadingMaterial::DeferredShadingMaterial(std::string diffuse, vec3 Ks, float Ns) :
    Material("DeferredShadingMaterial")
{
    init();

    m_TxDiffuse = new Texture2D(diffuse, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT);
    m_KdIsInterpolated = false;
    m_Ks = Ks;
    m_Ns = Ns;

    // compiler le shader
    compileShader();
}


/** destructeur */
DeferredShadingMaterial::~DeferredShadingMaterial()
{
    // libérer les textures qui ont été chargées
    if (m_TxDiffuse != nullptr) delete m_TxDiffuse;
    if (m_TxSpecular != nullptr) delete m_TxSpecular;
}


/**
 * retourne le source du Vertex Shader
 */
std::string DeferredShadingMaterial::getVertexShader()
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
    srcVertexShader << "\n";
    srcVertexShader << "// interpolation vers les fragments\n";
    srcVertexShader << "out vec4 frgPosition;\n";
    srcVertexShader << "out vec3 frgNormal;\n";

    // interpoler l'attribut couleur diffuse ?
    if (m_KdIsInterpolated) {
        // la couleur diffuse est un attribut de vertex
        srcVertexShader << "in vec4 glColor;\n";
        srcVertexShader << "out vec4 frgColor;\n";
    }

    // interpoler les coordonnées de texture ?
    if (m_TxDiffuse != nullptr || m_TxSpecular != nullptr) {
        // passer les coordonnées de texture
        srcVertexShader << "in vec2 glTexCoord;\n";
        srcVertexShader << "out vec2 frgTexCoord;\n";
    }

    srcVertexShader << "\n";
    srcVertexShader << "void main()\n";
    srcVertexShader << "{\n";
    srcVertexShader << "    frgPosition = mat4ModelView * vec4(glVertex, 1.0);\n";
    srcVertexShader << "    gl_Position = mat4Projection * frgPosition;\n";
    srcVertexShader << "    frgNormal = mat3Normal * glNormal;\n";
    if (m_KdIsInterpolated) {
        // interpoler la couleur diffuse
        srcVertexShader << "    frgColor = glColor;\n";
    }
    if (m_TxDiffuse != nullptr || m_TxSpecular != nullptr) {
        // interpoler les coordonnées de texture
        srcVertexShader << "    frgTexCoord = glTexCoord;\n";
    }
    srcVertexShader << "}";
    return srcVertexShader.str();
}


/**
 * retourne le source du Fragment Shader
 */
std::string DeferredShadingMaterial::getFragmentShader()
{
    std::ostringstream srcFragmentShader;
    srcFragmentShader.setf(std::ios::fixed, std::ios::floatfield);
    srcFragmentShader << "#version 300 es\n";
    srcFragmentShader << "precision mediump float;\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "// coordonnées et normale du fragment\n";
    srcFragmentShader << "in vec4 frgPosition;\n";
    srcFragmentShader << "in vec3 frgNormal;\n";

    // coordonnées de texture s'il y a une texture
    if (m_TxDiffuse != nullptr || m_TxSpecular != nullptr) {
        // coordonnées de texture interpolées
        srcFragmentShader << "in vec2 frgTexCoord;\n";
    }
    srcFragmentShader << "out vec4 glFragData[4];\n";

    srcFragmentShader << "\n";
    srcFragmentShader << "// caractéristiques du matériau\n";

    // couleur ou texture diffuse
    if (m_TxDiffuse != nullptr) {
        srcFragmentShader << "uniform sampler2D txDiffuse;\n";
    } else if (m_KdIsInterpolated) {
        srcFragmentShader << "in vec4 frgColor;\n";
    } else {
        srcFragmentShader << "const vec4 Kd = "<<vec4::str(m_Kd)<<";\n";
    }

    // couleur ou texture spéculaire
    if (m_Ns >= 0.0) {
        if (m_TxSpecular != nullptr) {
            srcFragmentShader << "uniform sampler2D txSpecular;\n";
        } else {
            srcFragmentShader << "const vec3 Ks = "<<vec3::str(m_Ks)<<";\n";
        }
        srcFragmentShader << "const float Ns = "<<m_Ns<<";\n";
    }

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

    // couleur diffuse du fragment
    if (m_TxDiffuse != nullptr) {
        srcFragmentShader << "    vec4 Kd = texture(txDiffuse, frgTexCoord);\n";
    } else if (m_KdIsInterpolated) {
        srcFragmentShader << "    vec4 Kd = frgColor;\n";
    }

    // couleur spéculaire du fragment
    if (m_Ns >= 0.0 && m_TxSpecular != nullptr) {
        srcFragmentShader << "    vec3 Ks = texture(txSpecular, frgTexCoord).rgb;\n";
    }

    // remplir les buffers MRT avec les informations nécessaires pour Phong plus tard
    srcFragmentShader << "    // remplir les buffers avec les informations\n";
    srcFragmentShader << "    glFragData[0] = Kd;\n";
    if (m_Ns >= 0.0) {
        srcFragmentShader << "    glFragData[1] = vec4(Ks, Ns);\n";
    } else {
        srcFragmentShader << "    glFragData[1] = vec4(0.0);\n";
    }
    srcFragmentShader << "    glFragData[2] = vec4(frgPosition.xyz, 1.0);\n";
    srcFragmentShader << "    glFragData[3] = vec4(normalize(frgNormal), 0.0);\n";
    srcFragmentShader << "}";
    return srcFragmentShader.str();
}


/**
 * recompile le shader du matériau
 */
void DeferredShadingMaterial::compileShader()
{
    // appeler la méthode de la superclasse
    Material::compileShader();

    // déterminer où sont les variables uniform spécifiques
    m_TxDiffuseLoc  = glGetUniformLocation(m_ShaderId, "txDiffuse");
    m_TxSpecularLoc = glGetUniformLocation(m_ShaderId, "txSpecular");
}


/**
 * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
 * @return le VBOset du matériau
 */
VBOset* DeferredShadingMaterial::createVBOset()
{
    // créer le VBOset et spécifier les noms des attribute nécessaires à ce matériau
    VBOset* vboset = Material::createVBOset();
    vboset->addAttribute(MeshVertex::ID_ATTR_COLOR,    Utils::VEC4, "glColor");
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
void DeferredShadingMaterial::enable(mat4 mat4Projection, mat4 mat4ModelView)
{
    // appeler la méthode de la superclasse
    Material::enable(mat4Projection, mat4ModelView);

    // prochaine unité de texture à utiliser
    GLenum unit = GL_TEXTURE0;

    // activer la texture diffuse si elle est utilisée
    if (m_TxDiffuseLoc >= 0) {
        m_TxDiffuse->setTextureUnit(unit, m_TxDiffuseLoc);
        unit += 1;
    }

    // activer la texture spéculaire si elle est utilisée
    if (m_TxSpecularLoc >= 0) {
        m_TxSpecular->setTextureUnit(unit, m_TxSpecularLoc);
        unit += 1;
    }
}


/**
 * Cette méthode désactive le matériau
 * NB : le shader doit être activé
 */
void DeferredShadingMaterial::disable()
{
    // prochaine unité de texture à utiliser
    GLenum unit = GL_TEXTURE0;

    // désactiver la texture diffuse si elle est utilisée
    if (m_TxDiffuseLoc >= 0 && m_TxDiffuse != nullptr) {
        m_TxDiffuse->setTextureUnit(unit);
        unit += 1;
    }

    // désactiver la texture spéculaire si elle est utilisée
    if (m_TxSpecularLoc >= 0 && m_TxSpecular != nullptr) {
        m_TxSpecular->setTextureUnit(unit);
        unit += 1;
    }

    // appeler la méthode de la superclasse
    Material::disable();
}
