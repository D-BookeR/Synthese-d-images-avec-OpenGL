#ifndef ROCHER_H
#define ROCHER_H

// Définition de la classe Rock

#include <gl-matrix.h>
#include <Mesh.h>
#include <VBOset.h>
#include <TextureMaterial.h>

#include "Cylinder.h"


class Rock
{
public:

    /**
     * Constructeur
     */
    Rock(Texture2D* texture, Cylinder* cylindre);

    /** destructeur, libère le VBOset, le matériau et le maillage */
    ~Rock();

    /**
     * dessin de l'objet
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    void onDraw(mat4& mat4Projection, mat4& mat4ModelView);


private:

    Mesh* m_Mesh;

    TextureMaterial* m_Material;

    VBOset* m_VBOset;
};

#endif
