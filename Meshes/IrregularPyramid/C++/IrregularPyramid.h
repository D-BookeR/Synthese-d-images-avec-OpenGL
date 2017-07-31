#ifndef PYRAMIDEIRREGULIERE_H
#define PYRAMIDEIRREGULIERE_H

// Définition de la classe IrregularPyramid

#include <Mesh.h>


class IrregularPyramid
{
public:

    IrregularPyramid();
    ~IrregularPyramid();

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
