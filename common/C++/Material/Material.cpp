/**
 * Définition de la classe Material, une base pour construire des matériaux
 */

#include <GL/glew.h>
#include <GL/gl.h>

#include <sstream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#include <utils.h>
#include <VBOset.h>
#include <Material.h>
#include <DeferredShadingMaterial.h>


/**
 * constructeur
 * NB: chaque sous-classe doit appeler elle-même compileShader() pour créer le shader
 * @param nom : nom du matériau
 */
Material::Material(std::string nom)
{
    // nom du matériau
    m_Name = nom;

    // identifiant OpenGL du shader
    m_ShaderId = 0;

    // emplacement des matrices de changement de repère
    m_MatPloc  = -1;
    m_MatMVloc = -1;
    m_MatNloc  = -1;

    // matrice normale
    m_Mat3Normal = mat3::create();

    // plan de coupe par défaut à l'infini
    m_ClipPlaneOn = false;
    m_ClipPlane = vec4::fromValues(0,0,1,1e38);
    m_ClipPlaneLoc = -1;

    // note : ce constructeur ne compile pas le shader, c'est aux sous-classes de le faire
}


/** destructeur */
Material::~Material()
{
    // supprimer le shader
    if (m_ShaderId > 0) Utils::deleteShaderProgram(m_ShaderId);
}


/**
 * retourne le nom du matériau
 * @return nom du matériau
 */
std::string Material::getName()
{
    return m_Name;
}


/**
 * retourne l'identifiant du shader de ce matériau
 * @return identifiant du shader
 */
GLint Material::getShaderId()
{
    return m_ShaderId;
}


/**
 * retourne le source du Vertex Shader
 */
std::string Material::getVertexShader()
{
    const char* srcVertexShader =
        "#version 300 es\n"
        "in vec3 glVertex;\n"
        "uniform mat4 mat4Projection;\n"
        "uniform mat4 mat4ModelView;\n"
        "out vec4 frgPosition;\n"
        "void main()\n"
        "{\n"
        "    frgPosition = mat4ModelView * vec4(glVertex, 1.0);\n"
        "    gl_Position = mat4Projection * frgPosition;\n"
        "}";
    return srcVertexShader;
}


/**
 * retourne le source du Fragment Shader
 */
std::string Material::getFragmentShader()
{
    std::ostringstream srcFragmentShader;
    srcFragmentShader << "#version 300 es\n";
    srcFragmentShader << "precision mediump float;\n";
    if (m_ClipPlaneOn) {
        srcFragmentShader << "\n";
        srcFragmentShader << "// plan de coupe\n";
        srcFragmentShader << "uniform vec4 ClipPlane;\n";
    }
    srcFragmentShader << "\n";
    srcFragmentShader << "in vec4 frgPosition;\n";
    srcFragmentShader << "out vec4 glFragData[4];\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "void main()\n";
    srcFragmentShader << "{\n";
    if (m_ClipPlaneOn) {
        srcFragmentShader << "    // plan de coupe\n";
        srcFragmentShader << "    if (dot(frgPosition, ClipPlane) < 0.0) discard;\n";
        srcFragmentShader << "\n";
    }
    srcFragmentShader << "    glFragData[0] = vec4(1.0, 0.0, 1.0, 1.0);\n";
    srcFragmentShader << "    glFragData[1] = vec4(0.0);\n";
    srcFragmentShader << "    glFragData[2] = vec4(frgPosition.xyz, 1.0);\n";
    srcFragmentShader << "    glFragData[3] = vec4(1.0, 1.0, 1.0, 1.0);\n";
    srcFragmentShader << "}";
    return srcFragmentShader.str();
}


/**
 * recompile le shader du matériau
 */
void Material::compileShader()
{
    // supprimer l'ancien shader s'il y en avait un
    if (m_ShaderId > 0) Utils::deleteShaderProgram(m_ShaderId);

    // construire le vertex shader
    std::string srcVertexShader = getVertexShader();

    // construire le fragment shader
    std::string srcFragmentShader = getFragmentShader();

    // compiler et lier les shaders
    m_ShaderId = Utils::makeShaderProgram(srcVertexShader, srcFragmentShader, m_Name);

    // déterminer où sont les variables uniform
    m_MatPloc      = glGetUniformLocation(m_ShaderId, "mat4Projection");
    m_MatMVloc     = glGetUniformLocation(m_ShaderId, "mat4ModelView");
    m_MatNloc      = glGetUniformLocation(m_ShaderId, "mat3Normal");
    m_ClipPlaneLoc = glGetUniformLocation(m_ShaderId, "ClipPlane");
}


/**
 * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
 * @return le VBOset du matériau
 */
VBOset* Material::createVBOset()
{
    // créer le VBOset et spécifier les noms des attribute nécessaires à ce matériau
    VBOset* vboset = new VBOset(this);
    vboset->addAttribute(MeshVertex::ID_ATTR_VERTEX,   Utils::VEC3, "glVertex");
    return vboset;
}


/**
 * Cette méthode active le matériau : met en place son shader,
 * fournit les variables uniform qu'il demande
 * @param mat4Projection : mat4 contenant la projection
 * @param mat4ModelView : mat4 contenant la transformation vers la caméra
 */
void Material::enable(mat4 mat4Projection, mat4 mat4ModelView)
{
    // activer le shader
    glUseProgram(m_ShaderId);

    // fournir les matrices MV et P
    mat4::glUniformMatrix(m_MatPloc, mat4Projection);
    mat4::glUniformMatrix(m_MatMVloc, mat4ModelView);

    // calculer et fournir la matrice normale au shader
    if (m_MatNloc >= 0) {
        mat3::fromMat4(m_Mat3Normal, mat4ModelView);
        mat3::transpose(m_Mat3Normal, m_Mat3Normal);
        mat3::invert(m_Mat3Normal, m_Mat3Normal);
        mat3::glUniformMatrix(m_MatNloc, m_Mat3Normal);
    }

    // si le plan de coupe est actif, alors le fournir
    if (m_ClipPlaneOn && m_ClipPlaneLoc >= 0) {
        vec4::glUniform(m_ClipPlaneLoc, m_ClipPlane);
    }
}


/**
 * Cette méthode désactive le matériau
 * NB : le shader doit encore être activé
 */
void Material::disable()
{
    // désactiver le shader
    glUseProgram(0);
}


/**
 * définit un plan de coupe pour les fragments. Ce plan est en coordonnées caméra
 * @param active : true s'il faut compiler un shader gérant le plan de coupe
 * @param plane vec4 en coordonnées caméra ou null pour supprimer le plan de coupe
 */
void Material::setClipPlane(bool active, vec4 plane)
{
    // il faut recompiler s'il y a un changement d'état
    bool recompile = (active != m_ClipPlaneOn);

    m_ClipPlaneOn = active;
    m_ClipPlane = plane;

    // recompiler le shader
    if (recompile) compileShader();
}


/**
 * enlève le plan de coupe mais sans recompiler le shader (le plan est mis à l'infini)
 * @param active : true s'il faut compiler un shader gérant le plan de coupe
 */
void Material::setClipPlane(bool active)
{
    // il faut recompiler s'il y a un changement d'état
    bool recompile = (active != m_ClipPlaneOn);

    // nouvel état et plan à l'infini
    m_ClipPlaneOn = active;
    m_ClipPlane = vec4::fromValues(0,0,1,1e38);

    // recompiler le shader
    if (recompile) compileShader();
}


/**
 * désactive le plan de coupe
 */
void Material::resetClipPlane()
{
    bool recompile = m_ClipPlaneOn;
    m_ClipPlaneOn = false;
    m_ClipPlane = vec4::fromValues(0,0,1,1e38);
    if (recompile) compileShader();
}


/**
 * Cette méthode crée une collection de DeferredShadingMaterial lue d'un fichier MTL pour un fichier OBJ.
 * NB : le chargement du fichier est fait en asynchrone, par une requête HTTP sur le serveur
 * @param folder : contient le fichier mtl et les textures
 * @param mtlfilename : nom seul du fichier au format MTL (ne pas mettre de chemin)
 * @param callback : fonction(that, materiaux) à appeler après avoir chargé les matériaux
 * @param that : objet pour lequel on charge des matériaux, il est passé en premier paramètre de la callback
 */
std::map<std::string, Material*> Material::loadMTL(std::string folder, std::string mtlfilename)
{
    // description d'un matériau, avant sa création
    class MatDescription {
    public:
        std::string map_Kd;
        vec4 Kd;
        vec3 Ks;
        float Ns;
        float d;
        MatDescription() {
            map_Kd = "";
            Kd = vec4::create();
            Ks = vec3::create();
            Ns = 0.0;
        }
    };

    // dictionnaire des matériaux du fichier => dans le modèle
    std::map<std::string, MatDescription> mat_descriptions;

    // matériau courant
    MatDescription* mat_description = nullptr;

    // ouverture du fichier
    std::ifstream inputStream;
    std::string fullpathname = folder + "/" + mtlfilename;
    inputStream.open(fullpathname.c_str(), std::ifstream::in);
    if (! inputStream.is_open()) {
        std::cerr << "Erreur : le fichier \"" << fullpathname << "\" ne peut pas etre lu." << std::endl;
        exit(EXIT_FAILURE);
    }

    // parcourir le fichier mtl ligne par ligne
    char* word = NULL;
    char* saveptr_word = NULL;
    int line_number = 0;
    char line[180];
    while (inputStream.getline(line, sizeof(line))) {
        line_number++;
        // extraire le premier mot de la ligne
        word = strtok_r(line," \t", &saveptr_word);
        if (word == NULL) continue;
        // mettre le mot en minuscules
        for (char* c=word; *c!='\0'; ++c) *c = tolower(*c);

        if (strcmp(word,"newmtl") == 0) {
            // créer une spécif de matériau et l'ajouter aux spécifs
            word = strtok_r(NULL, " \t", &saveptr_word);
            mat_description = &mat_descriptions[word];
        } else
        if (mat_description != nullptr) {
            if (strcmp(word, "map_kd") == 0) {
                // construire le nom de la texture
                word = strtok_r(NULL, " \t", &saveptr_word);
                mat_description->map_Kd = folder + "/" + word;
            } else
            if (strcmp(word, "kd") == 0) {
                float r = atof(strtok_r(NULL, " \t", &saveptr_word));
                float g = atof(strtok_r(NULL, " \t", &saveptr_word));
                float b = atof(strtok_r(NULL, " \t", &saveptr_word));
                mat_description->Kd = vec4::fromValues(r,g,b, 1.0);
            } else
            if (strcmp(word, "ks") == 0) {
                float r = atof(strtok_r(NULL, " \t", &saveptr_word));
                float g = atof(strtok_r(NULL, " \t", &saveptr_word));
                float b = atof(strtok_r(NULL, " \t", &saveptr_word));
                mat_description->Ks = vec3::fromValues(r,g,b);
            } else
            if (strcmp(word, "ns") == 0) {
                mat_description->Ns = atof(strtok_r(NULL, " \t", &saveptr_word));
            } else
            if (strcmp(word, "d") == 0) {
                mat_description->Kd[3] = atof(strtok_r(NULL, " \t", &saveptr_word));
            }
        }
    }

    // repasser sur tous les matériaux et les créer effectivement
    std::map<std::string, Material*> materials;
    for (auto const& it: mat_descriptions) {
        std::string mat_name = it.first;
        MatDescription mat_description = it.second;
        if (mat_description.map_Kd.empty()) {
            materials[mat_name] = new DeferredShadingMaterial(mat_description.Kd, mat_description.Ks, mat_description.Ns);
        } else {
            materials[mat_name] = new DeferredShadingMaterial(mat_description.map_Kd, mat_description.Ks, mat_description.Ns);
        }
    }

    // retourner la map des matériaux
    return materials;
}
