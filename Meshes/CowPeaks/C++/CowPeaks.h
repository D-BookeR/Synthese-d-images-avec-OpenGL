#ifndef COWPICS_H
#define COWPICS_H

// Définition de la classe CowPeaks

#include <Mesh.h>


class CowPeaks
{
public:

    /**
     * Constructeur
     */
    CowPeaks();
    ~CowPeaks();

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
