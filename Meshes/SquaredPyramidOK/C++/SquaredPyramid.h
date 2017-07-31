#ifndef PYRAMIDECARREE_H
#define PYRAMIDECARREE_H

// Définition de la classe SquaredPyramid

#include <Mesh.h>


class SquaredPyramid
{
public:

    SquaredPyramid();
    ~SquaredPyramid();

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
