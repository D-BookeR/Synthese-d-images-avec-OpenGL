#ifndef CUBESUBDIV_H
#define CUBESUBDIV_H

// Définition de la classe CubeSubdiv

#include <Mesh.h>


class CubeSubdiv
{
public:

    CubeSubdiv();
    ~CubeSubdiv();

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
