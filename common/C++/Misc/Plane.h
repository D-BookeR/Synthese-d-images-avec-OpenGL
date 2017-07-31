#ifndef MISC_PLANE_H
#define MISC_PLANE_H

// Définition de la classe Plane

#include <gl-matrix.h>

#include <Mesh.h>
#include <VBOset.h>


/**
 * Classe Plane.
 */
class Plane
{
public:

    /**
     * constructeur
     * @param material : matériau à appliquer sur le plan
     */
    Plane(Material* material);

    /** destructeur, libère le VBO et le shader */
    ~Plane();

    /**
     * dessine l'objet
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    void onDraw(mat4& mat4Projection, mat4& mat4ModelView);

private:

    VBOset* m_VBOset;

};

#endif
