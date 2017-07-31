#ifndef EAU_H
#define EAU_H

// Définition de la classe Water

#include <gl-matrix.h>
#include <Mesh.h>
#include <Material.h>
#include <VBOset.h>


/**
 * Classe Water
 */
class Water
{
public:

    /**
     * Constructeur
     * @param materiau pour l'eau
     */
    Water(Material* material);
    ~Water();

    /**
     * dessine la surface de l'eau
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    void onDraw(mat4& mat4Projection, mat4& mat4ModelView);


private:

    Mesh* m_Mesh;
    VBOset* m_VBOset;

};

#endif
