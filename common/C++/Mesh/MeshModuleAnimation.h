#ifndef MESH_MESHMODULEANIMATION_H
#define MESH_MESHMODULEANIMATION_H

/**
 * Ce module rajoute des fonctions et des classes pour calculer des déformations sur un maillage
 *
 */

#include <MeshModule.h>

class MeshModuleAnimation: public MeshModule
{

public:

    /**
     * initialise le module sur le maillage fourni
     * @param mesh maillage à gérer
     */
    MeshModuleAnimation(Mesh* mesh=nullptr);

    /**
     * associe le maillage autre en tant que déformation de this->m_Mesh
     * @param other autre maillage
     */
    void buildMorph(Mesh* other);

};

#endif
