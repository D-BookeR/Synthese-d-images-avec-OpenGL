package com.dbooker.livreopengl;

import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.mesh.Mesh;
import com.dbooker.livreopengl.mesh.MeshModuleTopology;
import com.dbooker.livreopengl.mesh.MeshVertex;


// Définition de la classe Cone
public class Cone
{
    private Mesh m_Mesh;

    /**
     * constructeur
     * @param triangle_count : nombre de secteurs (4 pour une pyramide à base carrée, bcp plus pour un cône)
     */
    public Cone(int triangle_count) throws Exception
    {
        // créer le maillage : une grille rectangulaire
        m_Mesh = new Mesh("Cone");
        MeshModuleTopology topology = new MeshModuleTopology(m_Mesh);
        int num0 = topology.addRectangularSurface(triangle_count, 2, "cone %d-%d", true, false);

        // définir les vertices du cône
        for (int is=0; is<triangle_count; is++) {
            // vertex sur la pointe
            MeshVertex s0 = m_Mesh.getVertex(is + 0*triangle_count + num0);
            s0.setCoord(getCoord((is + 0.5f) / triangle_count, 0.0f));

            // vertex sur le bord
            MeshVertex s1 = m_Mesh.getVertex(is + 1*triangle_count + num0);
            s1.setCoord(getCoord((is + 0.0f) / triangle_count, 1.0f));
        }

        // calculer les normales
        m_Mesh.computeNormals();
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
     * @param b : rayon du point, distance à l'axe Y
     * @return vec3 contenant les coordonnées
     */
    private vec3 getCoord(float a, float b)
    {
        // angle de ce point
        float angle = (float) (a * 2.0*Math.PI);

        float x = (float) (b * Math.sin(angle));
        float y = 1.0f - b;
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
