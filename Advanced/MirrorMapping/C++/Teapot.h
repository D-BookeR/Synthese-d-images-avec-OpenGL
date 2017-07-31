#ifndef TEAPOT_H
#define TEAPOT_H

// Définition de la classe Teapot

#include <gl-matrix.h>
#include <Mesh.h>
#include <VBOset.h>
#include <Light.h>
#include <TextureCube.h>

#include "MirrorMaterial.h"


class Teapot
{
public:

    /**
     * Constructeur
     */
    Teapot(TextureCube* texture);

    /** destructeur, libère le VBOset, le matériau et le maillage */
    ~Teapot();

    /**
     * dessine l'objet sur l'écran
     * @param mat4Projection : matrice de projection
     * @param mat4ModelViewScene : matrice de vue
     */
    void onDraw(mat4& mat4Projection, mat4& mat4ModelViewScene);


private:

    Mesh* m_Mesh;

    MirrorMaterial* m_Material;

    VBOset* m_VBOset;
};

#endif
