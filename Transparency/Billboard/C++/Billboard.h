#ifndef BILLBOARD_H
#define BILLBOARD_H

// Définition de la classe Billboard

#include <string>
#include <vector>

#include <gl-matrix.h>
#include <utils.h>
#include <Mesh.h>
#include <VBOset.h>

#include "Texture360.h"
#include "Texture360Material.h"


class Billboard
{
public:

    /**
     * crée un billboard : un rectangle portant une texture360
     * @param texture360
     */
    Billboard(Texture360* texture360);

    /** destructeur */
    ~Billboard();


    /**
     * dessin du billboard sur l'écran
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    void onDraw(mat4& mat4Projection, mat4& mat4ModelView);

    /**
     * dessin du billboard sur l'écran, variante plus simple
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    void onDraw2(mat4& mat4Projection, mat4& mat4ModelView);


protected:

    // matrices ModelView associées
    mat4 m_ModelViewTmp;
    mat4 m_ModelViewFixed;

    // maillage
    Mesh* m_Mesh;

    // matériau
    Texture360Material* m_Material;

    // VBOset
    VBOset* m_VBOset;
};

#endif
