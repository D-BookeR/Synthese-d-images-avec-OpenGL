#ifndef CONE_H
#define CONE_H

// Définition de la classe Cone

#include <vector>
#include <gl-matrix.h>

#include <Mesh.h>

/**
 * Classe Cone.
 */
class Cone
{
public:

    /**
     * constructeur, crée le maillage
     * @param spokes_count : nombre de secteurs
     */
    Cone(int spokes_count);

    /** destructeur, libère le maillage */
    ~Cone();

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
};

#endif
