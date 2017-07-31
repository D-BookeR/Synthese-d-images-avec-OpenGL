#ifndef VAGUE_H
#define VAGUE_H

// Définition de la classe Wave

#include <vector>
#include <gl-matrix.h>

#include <Mesh.h>

/**
 * Classe Wave.
 */
class Wave
{
public:

    /**
     * constructeur
     * @param contour : contour à dessiner
     * @param nbSecteurs : nombre de secteurs
     * @param nbSegments : nombre de cercles concentriques pour décrire la fonction
     */
    Wave(std::vector<vec2> border, int spokes_count, int segments_count);

    /** destructeur, libère le maillage */
    ~Wave();

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
