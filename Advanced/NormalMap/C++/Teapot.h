#ifndef TEAPOT_H
#define TEAPOT_H

// Définition de la classe Teapot

#include <gl-matrix.h>
#include <Mesh.h>
#include <VBOset.h>

#include "NormalMaterial.h"


class Teapot
{
public:

    /**
     * Constructeur
     */
    Teapot();

    /** destructeur, libère le VBOset, le matériau et le maillage */
    ~Teapot();

    /**
     * dessin de l'objet
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    void onDraw(mat4& mat4Projection, mat4& mat4ModelView);


private:

    Mesh* m_Mesh;

    NormalMaterial* m_Material;

    VBOset* m_VBOset;
};

#endif
