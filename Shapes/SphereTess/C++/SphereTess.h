#ifndef SPHERETESS_H
#define SPHERETESS_H

// Définition de la classe SphereTess

#include <vector>
#include <gl-matrix.h>

#include <Mesh.h>

/**
 * Classe SphereTess.
 * NB: on ne peut pas définir de bonnes coordonnées de texture, il y a un repliement à la longitude 180°
 */
class SphereTess
{
public:

    /**
     * constructeur, crée le maillage
     * @param type : forme de base 0=tétraèdre, 1=octaèdre, 2=icosaèdre
     * @param iterations : niveau de subdivision, 0=forme de base
     */
    SphereTess(int type, int iterations);

    /** destructeur, libère le maillage */
    ~SphereTess();

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
};

#endif
