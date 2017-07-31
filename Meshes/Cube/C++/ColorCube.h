#ifndef CUBESIMPLE_H
#define CUBESIMPLE_H

// Définition de la classe ColorCube

#include <Mesh.h>


class ColorCube
{
public:

    ColorCube();
    ~ColorCube();

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
