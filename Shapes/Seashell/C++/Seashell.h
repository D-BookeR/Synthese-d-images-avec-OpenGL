#ifndef COQUILLAGE_H
#define COQUILLAGE_H

// Définition de la classe Seashell

#include <vector>
#include <gl-matrix.h>

#include <Mesh.h>

/**
 * Classe Seashell
 */
class Seashell
{
public:

    /**
     * constructeur
    * @param nbtours : nombre de tours
    * @param nbdiv : nombre de divisions en longitude et en latitude
     */
    Seashell(int nbtours, int nbdiv);

    /** destructeur, libère le VBO et le shader */
    ~Seashell();

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
};

#endif
