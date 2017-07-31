#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <utils.h>
#include <GroundMaterial.h>

/**
 * Cette fonction définit la classe GroundMaterial pour dessiner le terrain.
 * @param heightmap : nom d'un fichier image contenant le relief
 * @param hmax : float qui donne la hauteur relative du terrain, ex: 0.4
 * @param delta : float qui indique la distance pour calculer la normale
 * @param diffuse : nom d'un fichier image contenant la texture diffuse
 * @param Ks : vec3
 * @param Ns : poli du matériau
 */
GroundMaterial::GroundMaterial(std::string heightmap, float hmax, float delta, std::string diffuse, vec3 Ks, float Ns) : Material("GroundMaterial")
{
    // charger le relief et la texture diffuse
    m_TxHeightmap = new Texture2D(heightmap);
    m_TxDiffuse = new Texture2D(diffuse);
    m_TxDiffuseLoc = -1;
    m_TxHeightmapLoc = -1;

    m_HMax = hmax;
    m_Delta = delta;
    m_Ks = Ks;
    m_Ns = Ns;

    // compiler le shader
    compileShader();
}


/**
 * retourne le source du Vertex Shader
 */
std::string GroundMaterial::getVertexShader()
{
    std::ostringstream srcVertexShader;
    srcVertexShader << "#version 300 es\n";
    srcVertexShader << "\n";
    srcVertexShader << "// attributs de sommets\n";
    srcVertexShader << "in vec3 glVertex;\n";
    srcVertexShader << "in vec2 glTexCoord;\n";
    srcVertexShader << "\n";
    srcVertexShader << "// paramètres du matériau\n";
    srcVertexShader << "const float delta = "<<m_Delta<<";\n";
    srcVertexShader << "const float hmax = "<<m_HMax<<";\n";
    srcVertexShader << "uniform sampler2D txHeightmap;\n";
    srcVertexShader << "\n";
    srcVertexShader << "// interpolation pour le fragment shader\n";
    srcVertexShader << "out vec4 frgPosition;\n";
    srcVertexShader << "out vec2 frgTexCoord;\n";
    srcVertexShader << "out vec3 frgNormal;\n";
    srcVertexShader << "\n";
    srcVertexShader << "// matrices de transformation\n";
    srcVertexShader << "uniform mat4 mat4ModelView;\n";
    srcVertexShader << "uniform mat4 mat4Projection;\n";
    srcVertexShader << "uniform mat3 mat3Normal;\n";
    srcVertexShader << "\n";
    srcVertexShader << "void main()\n";
    srcVertexShader << "{\n";
    srcVertexShader << "    // transformation du point par la heightmap\n";
    srcVertexShader << "    vec3 position = glVertex;\n";
    srcVertexShader << "    float height = texture(txHeightmap, glTexCoord).g * hmax;\n";
    srcVertexShader << "    position.y += height;\n";
    srcVertexShader << "    // position du fragment par rapport à la caméra et projection écran\n";
    srcVertexShader << "    frgPosition = mat4ModelView * vec4(position, 1.0);\n";
    srcVertexShader << "    gl_Position = mat4Projection * frgPosition;\n";
    srcVertexShader << "    // calcul de la normale locale\n";
    srcVertexShader << "    float heightN = texture(txHeightmap, glTexCoord+vec2(0.0,+delta)).g;\n";
    srcVertexShader << "    float heightS = texture(txHeightmap, glTexCoord+vec2(0.0,-delta)).g;\n";
    srcVertexShader << "    float heightE = texture(txHeightmap, glTexCoord+vec2(+delta,0.0)).g;\n";
    srcVertexShader << "    float heightW = texture(txHeightmap, glTexCoord+vec2(-delta,0.0)).g;\n";
    srcVertexShader << "    float dX = (heightE - heightW) * hmax;\n";
    srcVertexShader << "    float dZ = (heightS - heightN) * hmax;\n";
    srcVertexShader << "    vec3 N = vec3(-dX, 2.0*delta, -dZ);\n";
    srcVertexShader << "    frgNormal = mat3Normal * N;\n";
    srcVertexShader << "    // coordonnées de texture\n";
    srcVertexShader << "    frgTexCoord = glTexCoord;\n";
    srcVertexShader << "}";
    return srcVertexShader.str();
}


/**
 * retourne le source du Fragment Shader
 */
std::string GroundMaterial::getFragmentShader()
{
    std::ostringstream srcFragmentShader;
    srcFragmentShader.setf(std::ios::fixed, std::ios::floatfield);
    srcFragmentShader << "#version 300 es\n";
    srcFragmentShader << "precision mediump float;\n";
    srcFragmentShader << "uniform sampler2D txDiffuse;\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "// données interpolées venant du vertex shader\n";
    srcFragmentShader << "in vec4 frgPosition;\n";
    srcFragmentShader << "in vec2 frgTexCoord;\n";
    srcFragmentShader << "in vec3 frgNormal;\n";
    srcFragmentShader << "out vec4 glFragData[4];\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "void main()\n";
    srcFragmentShader << "{\n";
    srcFragmentShader << "    // couleur diffuse\n";
    srcFragmentShader << "    vec4 Kd = texture(txDiffuse, frgTexCoord);\n";
    srcFragmentShader << "    // couleur spéculaire\n";
    srcFragmentShader << "    const vec3 Ks = "<<vec3::str(m_Ks)<<";\n";
    srcFragmentShader << "    const float Ns = "<<m_Ns<<";\n";
    srcFragmentShader << "    // calcul du vecteur normal\n";
    srcFragmentShader << "    vec3 N = normalize(frgNormal);\n";
    srcFragmentShader << "    // remplir les buffers MRT avec les informations nécessaires pour Phong plus tard\n";
    srcFragmentShader << "    glFragData[0] = Kd;\n";
    if (m_Ns >= 0.0) {
        srcFragmentShader << "    glFragData[1] = vec4(Ks, Ns);\n";
    } else {
        srcFragmentShader << "    glFragData[1] = vec4(0.0);\n";
    }
    srcFragmentShader << "    glFragData[2] = vec4(frgPosition.xyz, 1.0);\n";
    srcFragmentShader << "    glFragData[3] = vec4(N, 0.0);\n";
    srcFragmentShader << "}";
    return srcFragmentShader.str();
}


/**
 * recompile le shader du matériau
 */
void GroundMaterial::compileShader()
{
    // appeler la méthode de la superclasse
    Material::compileShader();

    // déterminer où sont les variables uniform
    m_TxDiffuseLoc   = glGetUniformLocation(m_ShaderId, "txDiffuse");
    m_TxHeightmapLoc = glGetUniformLocation(m_ShaderId, "txHeightmap");
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

    // activer la texture d'altitude sur l'unité 0
    m_TxHeightmap->setTextureUnit(GL_TEXTURE0, m_TxHeightmapLoc);

    // activer la texture diffuse sur l'unité 1
    m_TxDiffuse->setTextureUnit(GL_TEXTURE1, m_TxDiffuseLoc);
}


/**
 * Cette méthode désactive le matériau
 * NB: le shader doit être activé
 */
void GroundMaterial::disable()
{
    // désactiver les textures
    m_TxHeightmap->setTextureUnit(GL_TEXTURE0);
    m_TxDiffuse->setTextureUnit(GL_TEXTURE1);

    // appeler la méthode de la superclasse
    Material::disable();
}


/**
 * Cette méthode supprime les ressources allouées
 */
GroundMaterial::~GroundMaterial()
{
    delete m_TxDiffuse;
    delete m_TxHeightmap;
}
