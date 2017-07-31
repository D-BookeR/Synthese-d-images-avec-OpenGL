#ifndef SKELETONMATERIAL_H
#define SKELETONMATERIAL_H

// Définition de la classe SkeletonMaterial

#include <sstream>
#include <vector>

#include <gl-matrix.h>
#include <utils.h>
#include <Material.h>
#include <Mesh.h>
#include <MeshVertex.h>
#include <VBOset.h>

#include "Joint.h"


class SkeletonMaterial: public Material
{
public:

    /**
     * constructeur
     * @param Ks : vec3
     * @param Ns : poli du matériau
     */
    SkeletonMaterial(vec3 Ks, float Ns);

    /** destructeur */
    virtual ~SkeletonMaterial();

    /**
     * crée une nouvelle jointure pour ce matériau
     * @param joint : jointure à ajouter
     * @return jointure à ajouter
     */
    Joint* addJoint(Joint* joint);

    /**
     * calcule les poids des sommets du maillage
     * @param mesh : maillage concerné
     */
    void computeWeights(Mesh* mesh);

    /**
     * recopie le poids d'une des jointures dans l'attribut ID_ATTR_COLOR
     * @param mesh : maillage contenant les sommets à colorer
     * @param joint : jointure concernée
     */
    void debugWeights(Mesh* mesh, Joint* joint);

    /**
     * calcule les matrices de transformation des jointures
     */
    void computeGlobalMatrices();

    /**
     * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
     * @return le VBOset du matériau
     */
    virtual VBOset* createVBOset();

    /**
     * Cette méthode active le matériau
     * @param mat4Projection : fournir la matrice de projection
     * @param mat4ModelView : fournir la matrice de vue
     */
    virtual void enable(mat4 mat4Projection, mat4 mat4ModelView);


protected:

    /** recompile le shader du matériau */
    virtual void compileShader();

    std::string getVertexShader();
    virtual std::string getFragmentShader();

    /**
     * affecte les attributs ID_ATTR_IDBONES et ID_ATTR_WEIGHTS d'un sommet
     * @param vertex : sommet dont il faut affecter les poids et numéros de jointures
     */
    void computeWeightsVertex(MeshVertex* vertex);


protected:

    /// paramètres de construction des shaders
    vec4 m_Kd;
    vec3 m_Ks;
    float m_Ns;

    /// liste des jointures
    std::vector<Joint*> m_Joints;

    /// emplacement des matrices (attention, différent de WebGL)
    GLint m_MatsGlobalJointsLoc;
};

#endif
