#ifndef COW_H
#define COW_H

// Définition de la classe Cow

#include <gl-matrix.h>
#include <Mesh.h>
#include <VBOset.h>
#include <ColorMaterial.h>

#include "ToonMaterial.h"


class Cow
{
public:

    /**
     * Constructeur
     */
    Cow(ToonMaterial* material);

    /** destructeur, libère le VBOset, le matériau et le maillage */
    ~Cow();

    /**
     * dessine l'objet sur l'écran
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    void onDraw(mat4& mat4Projection, mat4& mat4ModelView);

    /**
     * Fonction de dessin des arêtes de l'objet
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    void onDrawEdges(mat4& mat4Projection, mat4& mat4ModelView);


private:

    Mesh* m_Mesh;

    ToonMaterial* m_Material;
    ColorMaterial* m_EdgesMaterial;

    VBOset* m_VBOset;
    VBOset* m_VBOsetEdges;
};

#endif
