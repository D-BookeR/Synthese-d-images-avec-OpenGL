#ifndef GELLULE_H
#define GELLULE_H

// Définition de la classe Capsule

#include <vector>
#include <map>
#include <list>
#include <gl-matrix.h>

#include <Mesh.h>

/**
 * Classe Capsule.
 */
class Capsule
{
public:

    /**
     * Constructeur
     * @param spokes_count : nombre de rayons autour du centre
     * @param nbSegments : nombre de segments sur chaque rayon
     * @param longueur de la partie cylindrique
     */
    Capsule(int spokes_count, int segments_count, float longueur);

    /** destructeur, libère le maillage */
    ~Capsule();

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
     * @param longueur de la partie cylindrique
     * @return vec3 contenant les coordonnées
     */
    vec3 getCoord(float a, float b, float longueur);

    /**
     * Cette méthode retourne la normale 3D au point identifié par (a,b)
     * @param a : paramètre variant entre 0 et 1 en tournant autour du centre
     * @param b : rayon du point, distance au centre
     * @return vec3 contenant les coordonnées du vecteur normal
     */
    vec3 getNormal(float a, float b);
};

#endif
