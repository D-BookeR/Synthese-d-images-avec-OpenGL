#ifndef COW_H
#define COW_H

// Définition de la classe Cow

#include <Mesh.h>


class Cow
{
public:

    /**
     * Constructeur
     * @param nombre de sommets à simplifier
     */
    Cow(int nombre=0);
    ~Cow();

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
