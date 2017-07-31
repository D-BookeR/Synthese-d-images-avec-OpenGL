#ifndef PYRAMID_H
#define PYRAMID_H

// Définition de la classe Pyramid

#include <vector>
#include <list>

#include <gl-matrix.h>
#include <Material.h>
#include <Mesh.h>
#include <VBOset.h>


/**
 * Classe Pyramid.
 */
class Pyramid
{
public:

    /**
     * constructeur
     */
    Pyramid();

    /** destructeur, libère le VBOset et le maillage */
    ~Pyramid();

    /**
     * dessine l'objet sur l'écran
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    void onDraw(mat4& mat4Projection, mat4& mat4ModelView);


public:

    // matériaux
    Material* m_ColorMaterial;
    Material* m_RedMaterial;
    Material* m_GreenMaterial;
    Material* m_ProceduralMaterial;

    // VBOset
    std::vector<VBOset*> m_VBOsets;

};

#endif
