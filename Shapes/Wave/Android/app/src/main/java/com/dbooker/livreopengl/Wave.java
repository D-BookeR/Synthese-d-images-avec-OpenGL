package com.dbooker.livreopengl;

import java.util.List;

import com.dbooker.livreopengl.glmatrix.vec2;
import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.mesh.Mesh;
import com.dbooker.livreopengl.mesh.MeshModuleTopology;
import com.dbooker.livreopengl.mesh.MeshVertex;


// Définition de la classe Wave
public class Wave
{
    private Mesh m_Mesh;
    private List<vec2> border;

    /**
     * Cette fonction définit la classe Wave.
     * @param border : contour à dessiner
     * @param spokes_count : nombre de secteurs
     * @param segments_count : nombre de cercles concentriques pour décrire la fonction
     */
    public Wave(List<vec2> border, int spokes_count, int segments_count) throws Exception
    {
        this.border = border;

        // créer le maillage : une grille plane polaire
        m_Mesh = new Mesh("Wave");
        MeshModuleTopology topology = new MeshModuleTopology(m_Mesh);
        int num0 = topology.addRectangularSurface(spokes_count, segments_count, "grille %d-%d", true, false);

        // parcourir les sommets pour définir leurs coordonnées
        for (int is=0; is<spokes_count; is++) {
            float a = 1.0f - (float)is / spokes_count;
            for (int ip=0; ip<segments_count; ip++) {
                // numéro du vertex
                int num = is + ip*spokes_count + num0;
                // récupérer le sommet concerné
                MeshVertex vertex = m_Mesh.getVertex(num);
                // définir les coordonnées 3D du point
                vertex.setCoord(getCoord(a, ip));
            }
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
     * Cette méthode retourne les coordonnées 3D d'un point identifié par (a,ip)
     * @param a : paramètre variant entre 0 et 1 en tournant autour de l'axe Y
     * @param ip : numéro du point sur le contour
     * @return vec3 contenant les coordonnées
     */
    private vec3 getCoord(float a, int ip)
    {
        // point du contour : (r, y)
        vec2 point = border.get(ip);
        float r = point.get(0);
        float y = point.get(1);

        // angle de ce point
        float angle = (float) (a * 2.0 * Math.PI);

        // conversion polaires . cartésiennes
        float x = (float) (r * Math.sin(angle));
        float z = (float) (r * Math.cos(angle));
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
