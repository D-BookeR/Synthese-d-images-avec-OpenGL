#ifndef POLYGONE_H
#define POLYGONE_H

// Définition de la classe Polygon

#include <Mesh.h>


class Polygon
{
public:

    Polygon();
    ~Polygon();

    /**
     * fournit le maillage de cet objet
     * @return maillage
     */
    Mesh* getMesh()
    {
        return m_Mesh;
    }

private:

    Mesh* m_Mesh;

};

#endif
