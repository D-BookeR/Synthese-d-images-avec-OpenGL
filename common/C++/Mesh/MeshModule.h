#ifndef MESH_MESHMODULE_H
#define MESH_MESHMODULE_H

#include <Mesh.h>


/**
 * Cette classe représente un module permettant de faire des opérations sur les maillages
 *
 */
class MeshModule
{
protected:

    /** maillage géré par ce module */
    Mesh* m_Mesh;


public:

    /**
     * initialise le module sur le maillage fourni
     * @param mesh maillage concerné
     */
    MeshModule(Mesh* mesh = nullptr);

    /**
     * définit le maillage à gérer
     * @param mesh maillage concerné
     */
    void setMesh(Mesh* mesh);

};

#endif
