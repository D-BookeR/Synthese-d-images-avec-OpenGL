// classes et modules nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshHalfEdge.js");
Requires("libs/Mesh/MeshEdge.js");
Requires("libs/Mesh/MeshVertex.js");
Requires("libs/Mesh/MeshTriangle.js");


/**
 * Cette classe représente un module permettant de faire des opérations sur les maillages
 *
 */
class MeshModule
{

    /**
     * initialise le module sur le maillage fourni
     * @param mesh : maillage concerné
     */
    constructor(mesh=null)
    {
        this.m_Mesh = mesh;
    }


    /** destructeur */
    destroy()
    {
    }


    /**
     * définit le maillage à gérer
     * @param mesh : maillage concerné
     */
    setMesh(mesh)
    {
        this.m_Mesh = mesh;
    }
}
