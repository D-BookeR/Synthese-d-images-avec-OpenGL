package com.dbooker.livreopengl;

import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.mesh.Mesh;
import com.dbooker.livreopengl.mesh.MeshModuleTopology;
import com.dbooker.livreopengl.mesh.MeshVertex;


// Définition de la classe Disk
public class Disk
{
    private Mesh m_Mesh;

    /**
     * constructeur
     * @param triangle_count : nombre de secteurs
     */
    public Disk(int triangle_count) throws Exception
    {
        // créer le maillage
        m_Mesh = new Mesh("Disk");

        // ajouter une nappe circulaire
        MeshModuleTopology topology = new MeshModuleTopology(m_Mesh);
        int disk0 = topology.addRevolutionSurface(triangle_count, 1, "disque %d-%d");

        // définir le vertex central
        MeshVertex center = m_Mesh.getVertex(disk0);
        center.setCoord(getCoord(0.0f, 0.0f));
        center.setNormal(vec3.fromValues(0.0,+1.0,0.0));

        // définir les vertices du bord
        for (int is=0; is<triangle_count; is++) {
            int num = is + disk0+1;
            MeshVertex s = m_Mesh.getVertex(num);
            // calculer les coordonnées du point
            s.setCoord(getCoord((float)is / triangle_count, +1.0f));
            s.setNormal(vec3.fromValues(0.0,+1.0,0.0));
        }
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
     * Cette méthode retourne les coordonnées 3D d'un point identifié par (a,b)
     * @param a : paramètre variant entre 0 et 1 en tournant autour du centre
     * @param b : rayon du point, distance au centre
     * @return vec3 contenant les coordonnées
     */
    private vec3 getCoord(float a, float b)
    {
        // angle de ce point
        float angle = (float) (a * 2.0*Math.PI);

        float x = (float) (b * Math.sin(angle));
        float y = 0.0f;
        float z = (float) (b * Math.cos(angle));
        return vec3.fromValues(x,y,z);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    public void destroy()
    {
        m_Mesh.destroy();
    }
}
