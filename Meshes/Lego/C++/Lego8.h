#ifndef LEGO8_H8_H
#define LEGO8_H8_H

// Définition de la classe Lego8

#include <Mesh.h>


class Lego8
{
public:

    Lego8();
    ~Lego8();

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
