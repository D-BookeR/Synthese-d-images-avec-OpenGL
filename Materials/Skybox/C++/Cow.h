#ifndef COW_H
#define COW_H

// Définition de la classe Cow

#include <gl-matrix.h>
#include <Mesh.h>
#include <DeferredShadingMaterial.h>
#include <Light.h>
#include <VBOset.h>


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
     * dessin de l'objet
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    void onDraw(mat4& mat4Projection, mat4& mat4ModelView);


private:

    Mesh* m_Mesh;

    DeferredShadingMaterial* m_Material;

    VBOset* m_VBOset;
};

#endif
