#ifndef PYRAMIDECARREE_H
#define PYRAMIDECARREE_H

// Définition de la classe SquaredPyramid

#include <gl-matrix.h>

#include <Mesh.h>
#include <Material.h>
#include <VBOset.h>


class SquaredPyramid
{
public:

    SquaredPyramid(Material* material);
    ~SquaredPyramid();

    /**
     * dessine l'objet
     * @param mat4Projection matrice de projection
     * @param mat4ModelView matrice qui positionne l'objet devant la caméra
     */
    void onDraw(mat4 mat4Projection, mat4 mat4ModelView);

private:

    Mesh* m_Mesh;
    VBOset* m_VBOset;

};

#endif
