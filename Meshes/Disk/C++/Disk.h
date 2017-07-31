#ifndef DISQUE_H
#define DISQUE_H

// Définition de la classe Disk

#include <Mesh.h>


class Disk
{
public:

    Disk();
    ~Disk();

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
