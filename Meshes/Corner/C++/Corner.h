#ifndef COIN_H
#define COIN_H

// Définition de la classe Corner

#include <Mesh.h>


class Corner
{
public:

    Corner();
    ~Corner();

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
