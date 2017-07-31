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
 */
GroundMaterial::GroundMaterial(std::string heightmap, float hmax, float delta) : Material("GroundMaterial")
{
    // charger le relief
    m_TxHeightmap = new Texture2D(heightmap);
    m_TxHeightmapLoc = -1;

    m_HMax = hmax;
    m_Delta = delta;

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
    srcVertexShader << "out vec3 frgPosition;\n";
    srcVertexShader << "out vec3 frgNormal;\n";
    srcVertexShader << "\n";
    srcVertexShader << "// matrices de transformation\n";
    srcVertexShader << "uniform mat4 mat4ModelView;\n";
    srcVertexShader << "uniform mat4 mat4Projection;\n";
    srcVertexShader << "\n";
    srcVertexShader << "void main()\n";
    srcVertexShader << "{\n";
    srcVertexShader << "    // transformation du point par la heightmap\n";
    srcVertexShader << "    frgPosition = glVertex;\n";
    srcVertexShader << "    float height = texture(txHeightmap, glTexCoord).g * hmax;\n";
    srcVertexShader << "    frgPosition.y += height;\n";
    srcVertexShader << "    // position du fragment par rapport à la caméra et projection écran\n";
    srcVertexShader << "    gl_Position = mat4Projection * mat4ModelView * vec4(frgPosition, 1.0);\n";
    srcVertexShader << "    // calcul de la normale locale\n";
    srcVertexShader << "    float heightN = texture(txHeightmap, glTexCoord+vec2(0.0,+delta)).g;\n";
    srcVertexShader << "    float heightS = texture(txHeightmap, glTexCoord+vec2(0.0,-delta)).g;\n";
    srcVertexShader << "    float heightE = texture(txHeightmap, glTexCoord+vec2(+delta,0.0)).g;\n";
    srcVertexShader << "    float heightW = texture(txHeightmap, glTexCoord+vec2(-delta,0.0)).g;\n";
    srcVertexShader << "    float dX = (heightE - heightW) * hmax;\n";
    srcVertexShader << "    float dZ = (heightS - heightN) * hmax;\n";
    srcVertexShader << "    frgNormal = vec3(-dX, 2.0*delta, -dZ);\n";
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
    srcFragmentShader << "const float hmax = "<<m_HMax<<"; // hauteur relative maximale\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "// données interpolées venant du vertex shader\n";
    srcFragmentShader << "in vec3 frgPosition;\n";
    srcFragmentShader << "in vec3 frgNormal;\n";
    srcFragmentShader << "out vec4 glFragColor;\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "void main()\n";
    srcFragmentShader << "{\n";
    srcFragmentShader << "    // normaliser le vecteur normal local\n";
    srcFragmentShader << "    vec3 Nlocal = normalize(frgNormal);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "    // altitude et pente\n";
    srcFragmentShader << "    float elevation = frgPosition.y/hmax;\n";
    srcFragmentShader << "    float slope = 2.0 * (1.0 - Nlocal.y);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "    // couleur diffuse : dépend de l'altitude et de la pente\n";
    srcFragmentShader << "    glFragColor = vec4(elevation, slope, 0.7-elevation, 1.0);\n";
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
}


/**
 * Cette méthode désactive le matériau
 * @note le shader doit être activé
 */
void GroundMaterial::disable()
{
    // désactiver les textures
    m_TxHeightmap->setTextureUnit(GL_TEXTURE0);

    // appeler la méthode de la superclasse
    Material::disable();
}


/**
 * Cette méthode supprime les ressources allouées
 */
GroundMaterial::~GroundMaterial()
{
    delete m_TxHeightmap;
}
