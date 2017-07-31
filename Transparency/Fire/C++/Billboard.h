#ifndef BILLBOARD_H
#define BILLBOARD_H

// Définition de la classe Billboard

#include <string>
#include <vector>
#include <gl-matrix.h>
#include <utils.h>
#include <VBOset.h>
#include <Mesh.h>

#include <Material.h>


class Billboard
{
public:

    /**
     * crée un billboard : un rectangle portant une texture transparente
     * @param material : matériau à appliquer
     * @param width : largeur du panneau
     * @param height : hauteur du panneau
     */
    Billboard(Material* material, float largeur=1.0, float height=1.0);

    /** destructeur */
    ~Billboard();

    /**
     * dessin du billboard sur l'écran, variante simple
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    void onDraw(mat4& mat4Projection, mat4& mat4ModelView);


protected:

    // matrices ModelView associées
    mat4 m_ModelViewFixed;

    // maillage
    Mesh* m_Mesh;

    // VBOset
    VBOset* m_VBOset;
};

#endif
