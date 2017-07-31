package com.dbooker.livreopengl.mesh;



/**
 * Cette classe représente un module permettant de faire des opérations sur les maillages
 *
 */
class MeshModule
{
    protected static final String TAG = "OpenGL";


    /** maillage géré par ce module */
    protected Mesh m_Mesh;


    /**
     * initialise le module mais sans maillage
     */
    MeshModule()
    {
        this.m_Mesh = null;
    }


    /**
     * initialise le module sur le maillage fourni
     * @param mesh : maillage concerné
     */
    MeshModule(Mesh mesh)
    {
        this.m_Mesh = mesh;
    }


    /** destructeur */
    public void destroy()
    {
    }


    /**
     * définit le maillage à gérer
     * @param mesh : maillage concerné
     */
    public void setMesh(Mesh mesh)
    {
        this.m_Mesh = mesh;
    }
}
