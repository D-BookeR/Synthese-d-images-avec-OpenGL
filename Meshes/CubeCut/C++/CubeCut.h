#ifndef CUBETRONQUE_H
#define CUBETRONQUE_H

// Définition de la classe CubeCut

#include <Mesh.h>


class CubeCut
{
public:

    CubeCut();
    ~CubeCut();

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
