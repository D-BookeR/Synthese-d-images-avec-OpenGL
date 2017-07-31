package com.dbooker.livreopengl;


import android.util.Log;

import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.mesh.Mesh;
import com.dbooker.livreopengl.mesh.MeshModuleLoading;
import com.dbooker.livreopengl.mesh.MeshModulePhysics;
import com.dbooker.livreopengl.mesh.MeshModuleRedux;

public class Cow
{
    private static final String TAG = "LivreOpenGL";

    private Mesh m_Mesh;

    /**
     * Constructeur
     * @param nombre de sommets à supprimer par simplification de maillage
     */
    public Cow(int nombre) throws Exception
    {
        m_Mesh = new Mesh("Cow");
        MeshModuleLoading loader = new MeshModuleLoading(m_Mesh);
        loader.loadObjFile("data/models/Cow/Cow.obj", null, 0.15f);

        // définir la couleur de tous les sommets
        final int vertex_count = m_Mesh.getVertexCount();
        for (int i=0; i<vertex_count; i++) {
            m_Mesh.getVertex(i).setColor(vec3.fromValues(1.0, 0.7, 0.6));
        }
        Log.i(TAG, "La vache contient " + m_Mesh.getTriangleCount() + " triangles");

        // appliquer une réduction : supprimer un certain nombre de sommets
        MeshModuleRedux reduction = new MeshModuleRedux(m_Mesh);
        m_Mesh.computeNormals();
        reduction.reduxCount(nombre);
        Log.i(TAG, "Après réduction, la vache contient seulement " + m_Mesh.getTriangleCount() + " triangles");

        // calculer les informations physiques
        MeshModulePhysics physique = new MeshModulePhysics(m_Mesh);
        physique.setDensity(1.0f);
        physique.compVolumeIntegrals();
    }


    /**
     * fournit le maillage de cet objet
     * @return maillage
     */
    public Mesh getMesh()
    {
        return m_Mesh;
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    public void destroy()
    {
        m_Mesh.destroy();
    }
}
