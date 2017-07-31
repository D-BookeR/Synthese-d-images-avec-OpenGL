#ifndef BALAYAGE_H
#define BALAYAGE_H

// Définition de la classe PathExtrusion

#include <vector>
#include <gl-matrix.h>

#include <Mesh.h>

/**
 * Classe PathExtrusion.
 */
class PathExtrusion
{
public:

    /**
     * constructeur, crée le maillage
     * @param contourXY : ligne qu'il faut faire balayer le long du contour ZY
     * @param contourZY : ligne qui définit la trajectoire du contourXY
     */
    PathExtrusion(std::vector<vec2> borderXY, std::vector<vec2> borderZY);

    /** destructeur, libère le maillage */
    ~PathExtrusion();

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

    // contours
    std::vector<vec2> borderXY;
    std::vector<vec2> borderZY;


private:

    /**
     * Cette méthode retourne les coordonnées 3D d'un point identifié par (ix, iz)
     * @param ix : numéro du point sur le contourXY
     * @param iz : numéro du point sur le contourZY
     * @return vec3 contenant les coordonnées
     */
    vec3 getCoord(int ix, int iz);


    /**
     * Cette fonction calcule la normale au niveau du point (ix,iz)
     * @param ix : numéro du point dans le tableau contourXY
     * @param iz : numéro du point dans le tableau contourZY
     * @return vec3 : normale en ce point
     */
    vec3 getNormal(int ix, int iz);

};

#endif
