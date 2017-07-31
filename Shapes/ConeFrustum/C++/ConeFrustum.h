#ifndef TRONCCONE_H
#define TRONCCONE_H

// Définition de la classe ConeFrustum

#include <vector>
#include <gl-matrix.h>

#include <Mesh.h>

/**
 * Classe ConeFrustum.
 */
class ConeFrustum
{
public:

    /**
     * constructeur, crée le maillage
     * @param spokes_count : nombre de secteurs
     * @param RB : rayon en bas (y=-1)
     * @param RH : rayon en haut (y=+1)
     */
    ConeFrustum(int spokes_count, float RB, float RH);

    /** destructeur, libère le maillage */
    ~ConeFrustum();

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
     * @param RB : rayon en bas, quand b vaut -1
     * @param RH : rayon en haut, quand b vaut +1
     * @param a : paramètre variant entre 0 et 1 en tournant autour du centre
     * @param b : hauteur considérée, entre -1 et +1
     * @return vec3 contenant les coordonnées
     */
    vec3 getCoord(float RB, float RH, float a, float b);
};

#endif
