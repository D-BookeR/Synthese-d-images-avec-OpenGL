#ifndef TOUR_H
#define TOUR_H

// Définition de la classe Lathe

#include <vector>
#include <gl-matrix.h>

#include <Mesh.h>

/**
 * Classe Lathe.
 */
class Lathe
{
public:

    /**
     * constructeur, crée le maillage
     * @param contour : tableau de vec2(x,y)
     * @param nbSect : nombre de secteurs
     */
    Lathe(std::vector<vec2> border, int nbSect);

    /** destructeur, libère le maillage */
    ~Lathe();

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

    // liste des points du contour
    std::vector<vec2> border;

private:

    /**
     * Cette méthode retourne les coordonnées 3D d'un point identifié par (a,ip)
     * @param a : paramètre variant entre 0 et 1 en tournant autour de l'axe Y
     * @param ip : numéro du point sur le contour
     * @return vec3 contenant les coordonnées
     */
    vec3 getCoord(float a, int ip);
};

#endif
