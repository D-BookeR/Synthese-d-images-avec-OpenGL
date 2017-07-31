#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <utils.h>
#include <IslandMaterial.h>

/**
 * Cette fonction définit la classe IslandMaterial pour dessiner le terrain.
 * @param heightmap : nom d'un fichier image contenant le relief
 * @param hmax : float qui donne la hauteur relative du terrain par rapport à ses dimensions, ex: 0.4
 * @param delta : float qui indique la distance pour calculer la normale, dépend de la résolution de l'image
 */
IslandMaterial::IslandMaterial(std::string heightmap, float hmax, float delta) : Material("IslandMaterial")
{
    // charger la texture donnant le relief
    m_TxHeightmap = new Texture2D(heightmap);
    m_TxHeightmapLoc = -1;

    // charger les textures diffuses
    m_TxDiffuse1 = new Texture2D("data/models/TerrainHM/terrain_tx.jpg", GL_LINEAR, GL_REPEAT);
    m_TxDiffuse1Loc = -1;
    m_TxDiffuse2 = new Texture2D("data/textures/sols/79798.jpg", GL_LINEAR, GL_REPEAT);
    m_TxDiffuse2Loc = -1;

    m_HMax = hmax;
    m_Delta = delta;

    // compiler le shader
    compileShader();
}


/**
 * retourne le source du Vertex Shader
 */
std::string IslandMaterial::getVertexShader()
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
    srcVertexShader << "out vec2 frgTexCoord;\n";
    srcVertexShader << "out float frgElevation;\n";
    srcVertexShader << "out vec4 frgPosition;\n";
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
    srcVertexShader << "    frgElevation = position.y;\n";
    srcVertexShader << "    // position du fragment par rapport à la caméra et projection écran\n";
    srcVertexShader << "    frgPosition = mat4ModelView * vec4(position, 1.0);\n";
    srcVertexShader << "    gl_Position = mat4Projection * frgPosition;\n";
    srcVertexShader << "    // détermination de la normale\n";
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
std::string IslandMaterial::getFragmentShader()
{
    std::ostringstream srcFragmentShader;
    srcFragmentShader.setf(std::ios::fixed, std::ios::floatfield);
    srcFragmentShader << "#version 300 es\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "// textures diffuses\n";
    srcFragmentShader << "uniform sampler2D txDiffuse1;\n";
    srcFragmentShader << "uniform sampler2D txDiffuse2;\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "// données interpolées venant du vertex shader\n";
    srcFragmentShader << "precision mediump float;\n";
    srcFragmentShader << "in vec2 frgTexCoord;\n";
    srcFragmentShader << "in float frgElevation;\n";
    srcFragmentShader << "in vec4 frgPosition;\n";
    srcFragmentShader << "in vec3 frgNormal;\n";
    srcFragmentShader << "out vec4 glFragData[4];\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "// plan de coupe\n";
    srcFragmentShader << "uniform vec4 ClipPlane;\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "void main()\n";
    srcFragmentShader << "{\n";
    srcFragmentShader << "    // plan de coupe\n";
    srcFragmentShader << "    if (dot(frgPosition, ClipPlane) < 0.0) discard;\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "    // couleur diffuse\n";
    srcFragmentShader << "    vec4 Kd1 = texture(txDiffuse1, frgTexCoord);\n";
    srcFragmentShader << "    vec4 Kd2 = texture(txDiffuse2, frgTexCoord * 4.0);\n";
    srcFragmentShader << "    vec4 Kd = mix(Kd2, Kd1, smoothstep(-0.05, 0.05, frgElevation));\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "    // calcul du vecteur normal\n";
    srcFragmentShader << "    vec3 N = normalize(frgNormal);\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "    // remplir les buffers MRT avec les informations nécessaires pour Phong plus tard\n";
    srcFragmentShader << "    glFragData[0] = Kd;\n";
    srcFragmentShader << "    glFragData[1] = vec4(0.0);\n";
    srcFragmentShader << "    glFragData[2] = vec4(frgPosition.xyz, 1.0);\n";
    srcFragmentShader << "    glFragData[3] = vec4(N, 0.0);\n";
    srcFragmentShader << "}";
    return srcFragmentShader.str();
}


/**
 * recompile le shader du matériau
 */
void IslandMaterial::compileShader()
{
    // appeler la méthode de la superclasse
    Material::compileShader();

    // déterminer où sont les variables uniform
    m_TxDiffuse1Loc  = glGetUniformLocation(m_ShaderId, "txDiffuse1");
    m_TxDiffuse2Loc  = glGetUniformLocation(m_ShaderId, "txDiffuse2");
    m_TxHeightmapLoc = glGetUniformLocation(m_ShaderId, "txHeightmap");
}


/**
 * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
 * @return le VBOset du matériau
 */
VBOset* IslandMaterial::createVBOset()
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
void IslandMaterial::enable(mat4 mat4Projection, mat4 mat4ModelView)
{
    // appeler la méthode de la superclasse
    Material::enable(mat4Projection, mat4ModelView);

    // activer la texture d'altitude sur l'unité 0
    m_TxHeightmap->setTextureUnit(GL_TEXTURE0, m_TxHeightmapLoc);

    // activer la texture diffuse1 sur l'unité 1
    m_TxDiffuse1->setTextureUnit(GL_TEXTURE1, m_TxDiffuse1Loc);

    // activer la texture diffuse2 sur l'unité 2
    m_TxDiffuse2->setTextureUnit(GL_TEXTURE2, m_TxDiffuse2Loc);
}


/**
 * Cette méthode désactive le matériau
 * NB: le shader doit être activé
 */
void IslandMaterial::disable()
{
    // désactiver les textures
    m_TxHeightmap->setTextureUnit(GL_TEXTURE0);
    m_TxDiffuse1->setTextureUnit(GL_TEXTURE1);
    m_TxDiffuse2->setTextureUnit(GL_TEXTURE2);

    // appeler la méthode de la superclasse
    Material::disable();
}


/**
 * Cette méthode supprime les ressources allouées
 */
IslandMaterial::~IslandMaterial()
{
    delete m_TxDiffuse1;
    delete m_TxDiffuse2;
    delete m_TxHeightmap;
}
