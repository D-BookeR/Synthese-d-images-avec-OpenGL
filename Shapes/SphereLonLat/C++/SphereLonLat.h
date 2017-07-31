#ifndef SPHERELONLAT_H
#define SPHERELONLAT_H

// Définition de la classe SphereLonLat

#include <vector>
#include <gl-matrix.h>

#include <Mesh.h>

/**
 * Classe SphereLonLat.
 */
class SphereLonLat
{
public:

    /**
     * constructeur, crée le maillage
     * @param nbLon : nombre de divisions en longitude
     * @param nbLat : nombre de divisions en latitude
     */
    SphereLonLat(int nbLon, int nbLat);

    /** destructeur, libère le maillage */
    ~SphereLonLat();

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
     * @param a : longitude variant entre 0 et 1 en tournant autour du centre
     * @param b : latitude variant entre 0 (pôle sud) et 1 (pôle nord)
     * @return vec3 contenant les coordonnées
     */
    vec3 getCoord(float a, float b);
};

#endif
