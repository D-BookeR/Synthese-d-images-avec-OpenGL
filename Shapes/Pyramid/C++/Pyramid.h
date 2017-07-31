#ifndef PYRAMIDE_H
#define PYRAMIDE_H

// Définition de la classe Pyramid

#include <vector>
#include <gl-matrix.h>

#include <Mesh.h>

/**
 * Classe Pyramid.
 */
class Pyramid
{
public:

    /**
     * constructeur, crée le maillage
     */
    Pyramid();

    /** destructeur, libère le maillage */
    ~Pyramid();

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
