package com.dbooker.livreopengl.mesh;

import android.util.Log;


/**
 * Ce module rajoute des fonctions et des classes pour calculer des déformations sur un maillage
 *
 */
@SuppressWarnings("unused")
public class MeshModuleAnimation extends MeshModule
{
    /**
     * initialise le module sans maillage
     */
    public MeshModuleAnimation()
    {
        super();
    }


    /**
     * initialise le module sur le maillage fourni
     * @param mesh : maillage à gérer
     */
    public MeshModuleAnimation(Mesh mesh)
    {
        super(mesh);
    }


    /** destructeur */
    public void destroy()
    {
        super.destroy();
    }


    /**
     * associe le maillage autremaillage en tant que déformation de this.m_Mesh
     * @param othermesh : morph target
     */
    public void buildMorph(Mesh othermesh)
    {
        // les deux maillages doivent avoir le même nombre de sommets
        int count = m_Mesh.getVertexCount();
        if (count == othermesh.getVertexCount()) {
            // utiliser les coordonnées et normales de othermesh en tant que morph target
            for (int i=0; i<count; i++) {
                MeshVertex vertex = m_Mesh.getVertex(i);
                MeshVertex morph = othermesh.getVertex(i);
                vertex.setAttribute(MeshVertex.ID_ATTR_VERTEX1, morph.getCoord());
                vertex.setAttribute(MeshVertex.ID_ATTR_NORMAL1, morph.getNormal());
            }
        } else {
            Log.w(TAG, "nombre de sommets différent entre "+m_Mesh.getName()+" et "+othermesh.getName());
            // recopier les coordonnées et normales du maillage en tant que morph target
            for (int i=0; i<count; i++) {
                MeshVertex vertex = m_Mesh.getVertex(i);
                vertex.setAttribute(MeshVertex.ID_ATTR_VERTEX1, vertex.getCoord());
                vertex.setAttribute(MeshVertex.ID_ATTR_NORMAL1, vertex.getNormal());
            }
        }
    }
}
