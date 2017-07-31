package com.dbooker.livreopengl;

import java.util.List;

import com.dbooker.livreopengl.glmatrix.vec2;
import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.mesh.Mesh;
import com.dbooker.livreopengl.mesh.MeshModuleTopology;
import com.dbooker.livreopengl.mesh.MeshVertex;


// Définition de la classe Lathe
public class Lathe
{
    private Mesh m_Mesh;
    private List<vec2> border;

    /**
     * Cette fonction définit la classe Lathe.
     * @param border : tableau de vec2 représentant la ligne qu'il faut faire tourner
     * @param spokes_count : nombre de secteurs
     */
    public Lathe(List<vec2> border, int spokes_count) throws Exception
    {
        // initialiser les variables membres
        this.border = border;
        int points_count = border.size();

        // créer le maillage : une grille rectangulaire
        m_Mesh = new Mesh("Lathe");
        MeshModuleTopology topology = new MeshModuleTopology(m_Mesh);
        int num0 = topology.addHexagonalSurface(spokes_count, points_count, "tour %d-%d", true, false);

        // parcourir les sommets pour définir leurs coordonnées
        for (int is=0; is<spokes_count; is++) {
            for (int ip=0; ip<points_count; ip++) {
                // angle 0..1 autour de l'axe
                float a = (float) ((is - 0.5*(ip%2)) / spokes_count);
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

        // angle de ce point, attention, ouest.est
        float angle = (float) (-a * 2.0 * Math.PI);

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
