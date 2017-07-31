#ifndef LEGO4_H4_H
#define LEGO4_H4_H

// Définition de la classe Lego4

#include <Mesh.h>


class Lego4
{
public:

    Lego4();
    ~Lego4();

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
