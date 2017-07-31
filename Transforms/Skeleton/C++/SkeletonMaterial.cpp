// Définition de la classe SkeletonMaterial, une spécialisation de Material
// Ce matériau gère une hiérarche de Joint représentant un squelette animable

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

#include <utils.h>
#include <SkeletonMaterial.h>
#include <VBOset.h>

#define max(a,b) ((a>b)?a:b)

/**
 * Constructeur de la classe SkeletonMaterial. Ce matériau gère une hiérarchie
 * de Joint représentant un squelette animable
 * @param Ks : vec3
 * @param Ns : poli du matériau
 */
SkeletonMaterial::SkeletonMaterial(vec3 Ks, float Ns) :
    Material("SkeletonMaterial")
{
    // caractéristiques du matériau
    m_Ks = Ks;
    m_Ns = Ns;

    // compiler le shader
    compileShader();
}


/**
 * crée une nouvelle jointure pour ce matériau
 * @param joint : jointure à ajouter
 * @return jointure à ajouter
 */
Joint* SkeletonMaterial::addJoint(Joint* joint)
{
    // l'ajouter à la liste
    m_Joints.push_back(joint);

    // recompiler le shader car le nombre de jointures a changé
    compileShader();

    return joint;
}


/**
 * calcule les poids des sommets du maillage
 * @param mesh : maillage concerné
 */
void SkeletonMaterial::computeWeights(Mesh* mesh)
{
    if (m_Joints.size() == 0) throw std::runtime_error("Il n'y a aucune joint");
    // traiter tous les sommets
    for (MeshVertex* vertex: mesh->getVertexList()) {
        // initialiser les poids à 0
        for (Joint* joint: m_Joints) {
            joint->setWeight(0.0);
        }
        // calculer le poids du sommet compte tenu de toutes les jointures (les parents sont calculés avant les enfants)
        vec3 coords = vertex->getCoord();
        for (Joint* joint: m_Joints) {
            joint->computeWeights(coords);
        }
        // préparer les attributs poids et identifiants du sommet
        computeWeightsVertex(vertex);
    }
}


/**
 * affecte les attributs ID_ATTR_IDBONES et ID_ATTR_WEIGHTS d'un sommet
 * @param vertex : sommet dont il faut affecter les poids et numéros de jointures
 */
void SkeletonMaterial::computeWeightsVertex(MeshVertex* vertex)
{
    // tableau des numéros de jointures et des poids du sommet
    std::vector<float> JointsIndex;
    std::vector<float> JointsWeight;
    // passer les jointures en revue, garder celles dont les poids ne sont pas nuls
    float sum = 0.0;
    int num = 0;
    for (Joint* joint: m_Joints) {
        float weight = joint->getWeight();
        if (weight > 0.0) {
            // ajouter le poids et le numéro à la fin
            JointsIndex.push_back(num);
            JointsWeight.push_back(weight);
            sum += weight;
        }
        num++;
    }
    // modifier les poids si leur somme n'est pas 1.0
    if (sum == 0.0) {
        // forcer la dépendance totale à la première jointure
        JointsIndex.push_back(0.0);
        JointsWeight.push_back(1.0);
    } else if (sum != 1.0) {
        for (int i=0; i<JointsWeight.size(); i++) {
            JointsWeight[i] /= sum;
        }
    }
    // compléter les listes à 4 éléments
    for (int i=JointsIndex.size(); i<4; i++) {
        JointsIndex.push_back(0.0);
        JointsWeight.push_back(0.0);
    }
    // enregistrer ces informations dans le sommet
    vertex->setAttribute(MeshVertex::ID_ATTR_IDBONES, vec4::fromValues(JointsIndex[0], JointsIndex[1], JointsIndex[2], JointsIndex[3]));
    vertex->setAttribute(MeshVertex::ID_ATTR_WEIGHTS, vec4::fromValues(JointsWeight[0], JointsWeight[1], JointsWeight[2], JointsWeight[3]));
}



/**
 * recopie le poids d'une des jointures dans l'attribut ID_ATTR_COLOR
 * @param mesh : maillage contenant les sommets à colorer
 * @param joint : jointure concernée
 */
void SkeletonMaterial::debugWeights(Mesh* mesh, Joint* joint)
{
    for (MeshVertex* vertex: mesh->getVertexList()) {
        // recalculer le poids de ce sommet pour la jointure
        vec3 coords = vertex->getCoord();
        for (Joint* j: m_Joints) {
            j->computeWeights(coords);
        }
        // transformer le poids en couleur
        float weight = joint->getWeight();
        vec4 color = vec4::create();
        vec4::lerp(color, vec4::fromValues(0,0,1,1), vec4::fromValues(1,0,0,1), weight);
        vertex->setColor(color);
    }
}



/**
 * calcule les matrices de transformation des jointures
 */
void SkeletonMaterial::computeGlobalMatrices()
{
    // calculer les matrices absolues des jointures
    for (Joint* joint: m_Joints) {
        joint->computeGlobalMatrix();
    }
}


/** destructeur */
SkeletonMaterial::~SkeletonMaterial()
{
}


/**
 * retourne le source du Vertex Shader
 */
std::string SkeletonMaterial::getVertexShader()
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
    srcVertexShader << "in vec4 glColor;\n";
    srcVertexShader << "in vec4 JointsIndex;    // numéro des jointures de ce sommet\n";
    srcVertexShader << "in vec4 JointsWeight;   // poids des jointures pour ce sommet\n";
    srcVertexShader << "\n";
    srcVertexShader << "// interpolation vers les fragments\n";
    srcVertexShader << "out vec4 frgPosition;\n";
    srcVertexShader << "out vec3 frgNormal;\n";
    srcVertexShader << "out vec4 frgColor;\n";
    srcVertexShader << "\n";
    srcVertexShader << "// définition des os du squelette\n";
    srcVertexShader << "const int N = "<<max(1, m_Joints.size())<<";\n";
    srcVertexShader << "uniform mat4 JointsGlobalMatrix[N];\n";
    srcVertexShader << "\n";
    srcVertexShader << "void main()\n";
    srcVertexShader << "{\n";
    srcVertexShader << "    // coordonnées et normale initiale\n";
    srcVertexShader << "    vec4 vertex_orig = vec4(glVertex, 1.0);\n";
    srcVertexShader << "    vec4 normal_orig = vec4(glNormal, 0.0);\n";
    srcVertexShader << "\n";
    srcVertexShader << "    // calculer la transformation par sa première jointure\n";
    srcVertexShader << "    int num = int(JointsIndex.x);\n";
    srcVertexShader << "    float weight = JointsWeight.x;\n";
    srcVertexShader << "    mat4 matrix = JointsGlobalMatrix[num];\n";
    srcVertexShader << "    vec4 vertex = matrix * vertex_orig * weight;\n";
    srcVertexShader << "    vec4 normal = matrix * normal_orig * weight;\n";
    srcVertexShader << "\n";
    srcVertexShader << "    // ajouter la transformation par sa deuxième jointure\n";
    srcVertexShader << "    num = int(JointsIndex.y);\n";
    srcVertexShader << "    weight = JointsWeight.y;\n";
    srcVertexShader << "    matrix = JointsGlobalMatrix[num];\n";
    srcVertexShader << "    vertex += matrix * vertex_orig * weight;\n";
    srcVertexShader << "    normal += matrix * normal_orig * weight;\n";
    srcVertexShader << "\n";
    srcVertexShader << "    // ajouter la transformation par sa troisième jointure\n";
    srcVertexShader << "    num = int(JointsIndex.z);\n";
    srcVertexShader << "    weight = JointsWeight.z;\n";
    srcVertexShader << "    matrix = JointsGlobalMatrix[num];\n";
    srcVertexShader << "    vertex += matrix * vertex_orig * weight;\n";
    srcVertexShader << "    normal += matrix * normal_orig * weight;\n";
    srcVertexShader << "\n";
    srcVertexShader << "    // ajouter la transformation par sa quatrième jointure\n";
    srcVertexShader << "    num = int(JointsIndex.w);\n";
    srcVertexShader << "    weight = JointsWeight.w;\n";
    srcVertexShader << "    matrix = JointsGlobalMatrix[num];\n";
    srcVertexShader << "    vertex += matrix * vertex_orig * weight;\n";
    srcVertexShader << "    normal += matrix * normal_orig * weight;\n";
    srcVertexShader << "\n";
    srcVertexShader << "    // normaliser les coordonnées (perte de précision)\n";
    srcVertexShader << "    vertex /= vertex.w;\n";
    srcVertexShader << "\n";
    srcVertexShader << "    // transformation en repère caméra et écran\n";
    srcVertexShader << "    frgPosition = mat4ModelView * vertex;\n";
    srcVertexShader << "    gl_Position = mat4Projection * frgPosition;\n";
    srcVertexShader << "    frgNormal = mat3Normal * normal.xyz;\n";
    srcVertexShader << "\n";
    srcVertexShader << "    // couleur\n";
    srcVertexShader << "    frgColor = glColor;\n";
    srcVertexShader << "}";
    return srcVertexShader.str();
}


/**
 * retourne le source du Fragment Shader
 */
std::string SkeletonMaterial::getFragmentShader()
{
    std::ostringstream srcFragmentShader;
    srcFragmentShader.setf(std::ios::fixed, std::ios::floatfield);
    srcFragmentShader << "#version 300 es\n";
    srcFragmentShader << "precision mediump float;\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "// coordonnées et normale du fragment\n";
    srcFragmentShader << "in vec4 frgPosition;\n";
    srcFragmentShader << "in vec3 frgNormal;\n";
    srcFragmentShader << "in vec4 frgColor;\n";
    srcFragmentShader << "out vec4 glFragData[4];\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "// caractéristiques du matériau\n";
    srcFragmentShader << "const vec3 Ks = "<<vec3::str(m_Ks)<<";\n";
    srcFragmentShader << "const float Ns = "<<m_Ns<<";\n";
    srcFragmentShader << "\n";
    srcFragmentShader << "void main()\n";
    srcFragmentShader << "{\n";
    srcFragmentShader << "    glFragData[0] = frgColor;\n";
    srcFragmentShader << "    glFragData[1] = vec4(Ks, Ns);\n";
    srcFragmentShader << "    glFragData[2] = vec4(frgPosition.xyz, 1.0);\n";
    srcFragmentShader << "    glFragData[3] = vec4(normalize(frgNormal), 0.0);\n";
    srcFragmentShader << "}";
    return srcFragmentShader.str();
}


/**
 * recompile le shader du matériau
 */
void SkeletonMaterial::compileShader()
{
    // appeler la méthode de la superclasse
    Material::compileShader();

    // déterminer où sont les variables uniform spécifiques
    m_MatsGlobalJointsLoc = glGetUniformLocation(m_ShaderId, "JointsGlobalMatrix");
}


/**
 * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
 * @return le VBOset du matériau, voir Mesh.cpp
 */
VBOset* SkeletonMaterial::createVBOset()
{
    // créer le VBOset et spécifier les noms des attribute nécessaires à ce matériau
    VBOset* vboset = Material::createVBOset();
    vboset->addAttribute(MeshVertex::ID_ATTR_NORMAL,  Utils::VEC3, "glNormal");
    vboset->addAttribute(MeshVertex::ID_ATTR_COLOR,   Utils::VEC4, "glColor");
    vboset->addAttribute(MeshVertex::ID_ATTR_IDBONES, Utils::VEC4, "JointsIndex");
    vboset->addAttribute(MeshVertex::ID_ATTR_WEIGHTS, Utils::VEC4, "JointsWeight");
    return vboset;
}


/**
 * Cette méthode active le matériau : met en place son shader,
 * fournit les variables uniform qu'il demande
 * @param mat4Projection : mat4 contenant la projection
 * @param mat4ModelView : mat4 contenant la transformation vers la caméra
 */
void SkeletonMaterial::enable(mat4 mat4Projection, mat4 mat4ModelView)
{
    // appeler la méthode de la superclasse
    Material::enable(mat4Projection, mat4ModelView);

    // fournir les matrices des os
    for (int i=0; i<m_Joints.size(); i++) {
        mat4::glUniformMatrix(m_MatsGlobalJointsLoc+i, m_Joints[i]->getGlobalMatrix());
    }
}
