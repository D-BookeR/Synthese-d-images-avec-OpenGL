#ifndef TORE_H
#define TORE_H

// Définition de la classe Tore

#include <vector>
#include <gl-matrix.h>

#include <Mesh.h>

/**
 * Classe Tore
 */
class Tore
{
public:

    /**
     * constructeur
     * @param nblon : nombre de divisions en longitude
     * @param nblat : nombre de divisions en latitude
     * @param r : rayon du tube, le grand rayon est 1.0
     */
    Tore(int nblon, int nblat, float r);

    /** destructeur, libère le VBO et le shader */
    ~Tore();

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
     * @param r : rayon du tube, le grand rayon est 1.0
     * @param a : longitude variant entre 0 et 1 en tournant autour du centre
     * @param b : latitude variant entre 0 (pôle sud) et 1 (pôle nord)
     * @return vec3 contenant les coordonnées
     */
    vec3 getCoord(float r, float a, float b);

    /**
     * Cette méthode retourne la normale 3D au point identifié par (a,b)
     * @param r : rayon du tube, le grand rayon est 1.0
     * @param a : longitude variant entre 0 et 1 en tournant autour du centre
     * @param b : latitude variant entre 0 (pôle sud) et 1 (pôle nord)
     * @return vec3 contenant les coordonnées du vecteur normal
     */
    vec3 getNormal(float r, float a, float b);
};

#endif
