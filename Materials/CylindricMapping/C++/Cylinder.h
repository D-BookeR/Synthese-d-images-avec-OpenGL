#ifndef CYLINDRE_H
#define CYLINDRE_H

// Définition de la classe Cylinder

#include <vector>

#include <gl-matrix.h>
#include <Mesh.h>
#include <VBOset.h>
#include <Texture2D.h>

#include "TransparentMaterial.h"


/**
 * Classe Cylinder.
 */
class Cylinder
{
public:

    /**
     * constructeur, crée le maillage
     * @param spokes_count : nombre de secteurs
     * @param texture à appliquer sur l'objet
     */
    Cylinder(int spokes_count, Texture2D* texture);

    /** destructeur, libère le maillage */
    ~Cylinder();

    /**
     * affecte la texture DepthMap du shader
     * @param far : depth buffer qui limite les dessins arrière de ce matériau
     * @param near : depth buffer qui limite les dessins avant de ce matériau
     */
    void setDepthMaps(GLint far, GLint near);

    /**
     * spécifie la taille de la fenêtre
     * @param width : largeur en nombre de pixels de la fenêtre
     * @param height : hauteur en nombre de pixels de la fenêtre
     */
    void setWindowDimensions(int width, int height);

    /**
     * Modifie l'extension verticale du cylindre
     * @param minY hauteur de la base
     * @param maxY hauteur du sommet
     */
    void updateMinMaxY(float minY, float maxY);

    /**
     * dessine l'objet sur l'écran
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    void onDraw(mat4& mat4Projection, mat4& mat4ModelView);


private:

    Mesh* m_Mesh;
    TransparentMaterial* m_Material;
    VBOset* m_VBOset;
    int m_Num0;
    int m_SpokesCount;

private:

    /**
     * Cette méthode retourne les coordonnées 3D d'un point identifié par (a,b)
     * @param a : paramètre variant entre 0 et 1 en tournant autour du centre
     * @param b : rayon du point, distance au centre
     * @return vec3 contenant les coordonnées
     */
    vec3 getVertex(float a, float b);

    /**
     * Cette méthode retourne la normale 3D au point identifié par (a,_)
     * @param a : paramètre variant entre 0 et 1 en tournant autour du centre
     * @return vec3 contenant les coordonnées du vecteur normal
     */
    vec3 getNormal(float a);
};

#endif
