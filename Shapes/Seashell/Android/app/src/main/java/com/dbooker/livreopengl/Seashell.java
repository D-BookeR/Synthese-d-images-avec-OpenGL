package com.dbooker.livreopengl;

import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.mesh.Mesh;
import com.dbooker.livreopengl.mesh.MeshModuleTopology;
import com.dbooker.livreopengl.mesh.MeshVertex;
import com.dbooker.livreopengl.utils.Utils;


// Définition de la classe Seashell
public class Seashell
{
    private Mesh m_Mesh;

    /**
     * constructeur
     * @param turns_count : nombre de tours
     * @param steps : nombre de divisions en longitude et en latitude
     */
    public Seashell(int turns_count, int steps) throws Exception
    {
        // paramètres
        float r = 0.35f;

        // créer le maillage : une nappe hexagonale partiellement refermée sur elle-même
        m_Mesh = new Mesh("Seashell");
        MeshModuleTopology topology = new MeshModuleTopology(m_Mesh);
        int num0 = topology.addHexagonalSurface(steps*turns_count, steps, "seashell %d-%d", false, true);

        // parcourir les sommets pour définir les coordonnées
        for (int ilat=0; ilat<steps; ilat++) {
            for (int ilon=0; ilon<steps*turns_count; ilon++) {
                // récupérer le sommet concerné
                int num = ilon + ilat*steps*turns_count + num0;
                MeshVertex vertex = m_Mesh.getVertex(num);
                // définir les coordonnées 3D du point
                float a = (ilon - 0.5f*(ilat%2)) / steps;
                float b = (float)ilat / steps;
                vertex.setCoord(getCoord(r, a, b));
                // définir la couleur du point
                float c = (ilon - 0.5f*(ilat%2)) / (steps*turns_count);
                vertex.setColor(Utils.hsv2rgb(vec3.fromValues(0.9f-c*0.2f, 0.8f, 0.7f)));
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
     * Cette méthode retourne les coordonnées 3D d'un point identifié par (r,a,b)
     * @param r : rayon du tube, le grand rayon est 1.0
     * @param a : longitude variant entre 0 et 1 en tournant autour du centre
     * @param b : latitude variant entre 0 (pôle sud) et 1 (pôle nord)
     * @return vec3 contenant les coordonnées
     */
    private vec3 getCoord(float r, float a, float b)
    {
        // longitude  attention, on tourne dans le sens ouest.est
        float lon = (float) (-a * 2.0 * Math.PI);
        float coslon = (float) Math.cos(lon);
        float sinlon = (float) Math.sin(lon);

        // latitude
        float lat = (float) (b * 2.0 * Math.PI);
        float coslat = (float) Math.cos(lat);
        float sinlat = (float) Math.sin(lat);

        // évolution du petit rayon selon a
        r = r * a * 0.5f;

        // conversion toriques -> cartésiennes
        float R = r * (1.0f + coslat);
        float x = R * sinlon;
        float y = r * sinlat + a*a*0.2f - 2.0f;
        float z = R * coslon;
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
