#ifndef MISC_CUBE_H
#define MISC_CUBE_H

// Définition de la classe Cube

#include <Mesh.h>
#include <VBOset.h>
#include <Material.h>


class Cube
{
public:

    Cube(Material* material);
    ~Cube();

    /**
     * dessine l'objet
     * @param mat4Projection matrice de projection
     * @param mat4ModelView matrice qui positionne l'objet devant la caméra
     */
    void onDraw(mat4& mat4Projection, mat4& mat4ModelView);


private:

    VBOset* m_VBOset;

};

#endif
