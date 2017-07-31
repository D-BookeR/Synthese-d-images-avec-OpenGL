package com.dbooker.livreopengl;

import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.mesh.Mesh;
import com.dbooker.livreopengl.mesh.MeshModuleTopology;
import com.dbooker.livreopengl.mesh.MeshVertex;


// Définition de la classe SphereLonLat
public class SphereLonLat
{
    private Mesh m_Mesh;

    /**
     * Cette fonction définit la classe SphereLonLat.
     * @param nbLon : nombre de divisions en longitude
     * @param nbLat : nombre de divisions en latitude
     */
    public SphereLonLat(int nbLon, int nbLat) throws Exception
    {
        // créer le maillage : une nappe rectangulaire
        m_Mesh = new Mesh("SphereLonLat");
        MeshModuleTopology topology = new MeshModuleTopology(m_Mesh);
        int num0 = topology.addRectangularSurface(nbLon, nbLat, "sphere %d-%d", true, false);

        // aller du pôle sud au pôle nord, pour définir les sommets
        for (int ilat=0; ilat<nbLat; ilat++) {
            // faire le tour du globe
            for (int ilon=0; ilon<nbLon; ilon++) {
                // récupérer le sommet
                MeshVertex vertex = m_Mesh.getVertex(ilon + ilat*nbLon + num0);
                // calculer ses coordonnées
                float a = (float)ilon / nbLon;
                float b = (float)ilat / (nbLat-1);
                vertex.setCoord(getCoord(a, b));
                vertex.setNormal(getCoord(a, b));
            }
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
     * @param a : longitude variant entre 0 et 1 en tournant autour du centre
     * @param b : latitude variant entre 0 (pôle sud) et 1 (pôle nord)
     * @return vec3 contenant les coordonnées
     */
    private vec3 getCoord(float a, float b)
    {
        // longitude : 0.5 - a pour décaler 0° au niveau du méridien de greenwich et tourner dans le bon sens
        float lon = (float) ((0.5 - a) * 2.0 * Math.PI);
        float coslon = (float) Math.cos(lon);
        float sinlon = (float) Math.sin(lon);

        // latitude
        float lat = (float) ((b - 0.5) * Math.PI);
        float coslat = (float) Math.cos(lat);
        float sinlat = (float) Math.sin(lat);

        // conversion sphériques -> cartésiennes
        float x = sinlon * coslat;
        float y = sinlat;
        float z = coslon * coslat;
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
