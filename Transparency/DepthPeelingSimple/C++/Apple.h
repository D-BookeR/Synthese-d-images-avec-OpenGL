#ifndef POMME_H
#define POMME_H

// Définition de la classe Apple

#include <gl-matrix.h>
#include <Mesh.h>
#include <Material.h>
#include <VBOset.h>


class Apple
{
public:

    /**
     * Constructeur
     */
    Apple(Material* material);

    /** destructeur, libère le VBOset, le matériau et le maillage */
    ~Apple();

    /**
     * dessin de l'objet
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    void onDraw(mat4& mat4Projection, mat4& mat4ModelView);

private:

    Mesh* m_Mesh;
    VBOset* m_VBOset;
};

#endif
