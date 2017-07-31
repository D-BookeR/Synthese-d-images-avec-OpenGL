#ifndef CUBE3MATS_H3MATS_H
#define CUBE3MATS_H3MATS_H

// Définition de la classe Cube3Mats

#include <vector>
#include <list>

#include <gl-matrix.h>
#include <Material.h>
#include <Mesh.h>
#include <VBOset.h>


/**
 * Classe Cube3Mats.
 */
class Cube3Mats
{
public:

    /**
     * constructeur
     */
    Cube3Mats();

    /** destructeur, libère le VBOset et le maillage */
    ~Cube3Mats();

    /**
     * dessine l'objet sur l'écran
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    void onDraw(mat4& mat4Projection, mat4& mat4ModelView);

protected:

    /**
     * Cette fonction ajoute une face perpendiculaire à l'axe au maillage fourni
     * @param mesh : maillage auquel on rajoute la face
     * @param axis : n° de la coordonnée + 1 qui donne la normale, ex: 2 pour Y, son signe donne le sens
     */
    void addFace(Mesh* mesh, int axis);

public:

    // matériaux
    Material* m_RedMaterial;
    Material* m_GreenMaterial;
    Material* m_ProceduralMaterial;

    // VBOset
    VBOset* m_VBOsetRed;
    VBOset* m_VBOsetVert;
    VBOset* m_VBOsetProcedural;

};

#endif
