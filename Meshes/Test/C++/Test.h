#ifndef TEST_H
#define TEST_H

// Définition de la classe Test

#include <Mesh.h>
#include <MeshVertex.h>


class Test
{
public:

    Test();
    ~Test();

    /**
     * fournit le maillage de cet objet
     * @return maillage
     */
    Mesh* getMesh()
    {
        return m_Mesh;
    }

private:

    void mkSquare(MeshVertex* A, MeshVertex* B, MeshVertex* C, MeshVertex* D);

    void mkTetra(MeshVertex* A, MeshVertex* B, MeshVertex* C, MeshVertex* D);

private:

    Mesh* m_Mesh;

};

#endif
