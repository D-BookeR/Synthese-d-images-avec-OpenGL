#ifndef COW_H
#define COW_H

// Définition de la classe Cow

#include <gl-matrix.h>
#include <Mesh.h>
#include <VBOset.h>

#include "SkeletonMaterial.h"
#include "JointDebug.h"


class Cow
{
public:

    /**
     * Constructeur
     */
    Cow();

    /** destructeur, libère le VBOset, le matériau et le maillage */
    ~Cow();

    /**
     * Dessin de la vache sur l'écran
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    void onDraw(mat4& mat4Projection, mat4& mat4ModelView);


private:

    Mesh* m_Mesh;

    SkeletonMaterial* m_Material;

    VBOset* m_VBOset;

    JointDebug* m_BodyJoint;
    JointDebug* m_NeckJoint;
    JointDebug* m_HeadJoint;
    JointDebug* m_TailJoint1;
    JointDebug* m_TailJoint2;

};

#endif
