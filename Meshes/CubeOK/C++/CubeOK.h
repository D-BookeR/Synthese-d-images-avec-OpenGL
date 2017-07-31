#ifndef CUBEOK_H
#define CUBEOK_H

// Définition de la classe CubeOK

#include <Mesh.h>


class CubeOK
{
public:

    CubeOK();
    ~CubeOK();

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
