#ifndef CYLINDRE_H
#define CYLINDRE_H

// Définition de la classe Cylinder

#include <vector>
#include <gl-matrix.h>

#include <Mesh.h>

/**
 * Classe Cylinder.
 */
class Cylinder
{
public:

    /**
     * constructeur, crée le maillage
     * @param spokes_count : nombre de secteurs
     */
    Cylinder(int spokes_count);

    /** destructeur, libère le maillage */
    ~Cylinder();

    /**
     * fournit le maillage de cet objet
     * @return maillage
     */
    Mesh* getMesh()
    {
        return m_Mesh;
    }


private:

    // maillage
    Mesh* m_Mesh;

private:

    /**
     * Cette méthode retourne les coordonnées 3D d'un point identifié par (a,b)
     * @param a : paramètre variant entre 0 et 1 en tournant autour du centre
     * @param b : rayon du point, distance au centre
     * @return vec3 contenant les coordonnées
     */
    vec3 getCoord(float a, float b);

    /**
     * Cette méthode retourne la normale 3D au point identifié par (a,_)
     * @param a : paramètre variant entre 0 et 1 en tournant autour du centre
     * @return vec3 contenant les coordonnées du vecteur normal
     */
    vec3 getNormal(float a);
};

#endif
