package com.dbooker.livreopengl;

import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.mesh.Mesh;
import com.dbooker.livreopengl.mesh.MeshModuleTopology;
import com.dbooker.livreopengl.mesh.MeshVertex;


// Définition de la classe Tore
public class Tore
{
    private Mesh m_Mesh;

    /**
     * Cette fonction définit la classe Tore.
     * @param nblon : nombre de divisions en longitude
     * @param nblat : nombre de divisions en latitude
     * @param r : rayon du tube, le grand rayon est 1.0
     */
    public Tore(int nblon, int nblat, float r) throws Exception
    {
        // créer le maillage : une grille rectangulaire refermée sur elle-même
        m_Mesh = new Mesh("Tore");
        MeshModuleTopology topology = new MeshModuleTopology(m_Mesh);
        int num0 = topology.addRectangularSurface(nblon, nblat, "tore %d-%d", true, true);

        // parcourir les sommets pour définir les coordonnées
        for (int ilat=0; ilat<nblat; ilat++) {
            for (int ilon=0; ilon<nblon; ilon++) {
                // récupérer le sommet concerné
                int num = ilon + ilat*nblon + num0;
                MeshVertex vertex = m_Mesh.getVertex(num);
                // définir les coordonnées 3D du point
                float a = (float)ilon / nblon;
                float b = (float)ilat / nblat;
                vertex.setCoord(getCoord(r, a, b));
                vertex.setNormal(getNormal(r, a, b));
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
     * @param r : rayon du tube, le grand rayon est 1.0
     * @param a : longitude variant entre 0 et 1 en tournant autour du centre
     * @param b : latitude variant entre 0 (pôle sud) et 1 (pôle nord)
     * @return vec3 contenant les coordonnées
     */
    private vec3 getCoord(float r, float a, float b)
    {
        // longitude  attention, on tourne dans le sens ouest.est
        float lon = (float) (-a * 2.0 * Math.PI);
        float coslon = (float)Math.cos(lon);
        float sinlon = (float)Math.sin(lon);

        // latitude
        float lat = (float) (b * 2.0 * Math.PI);
        float coslat = (float)Math.cos(lat);
        float sinlat = (float)Math.sin(lat);

        // conversion toriques . cartésiennes
        float R = (float) (1.0 + r * coslat);
        float x = R * sinlon;
        float y = r * sinlat;
        float z = R * coslon;
        return vec3.fromValues(x,y,z);
    }


    /**
     * Cette méthode retourne la normale 3D au point identifié par (a,b)
     * @param r : rayon du tube, le grand rayon est 1.0
     * @param a : longitude variant entre 0 et 1 en tournant autour du centre
     * @param b : latitude variant entre 0 (pôle sud) et 1 (pôle nord)
     * @return vec3 contenant les coordonnées du vecteur normal
     */
    private vec3 getNormal(float r, float a, float b)
    {
        // longitude  attention, on tourne dans le sens ouest.est
        float lon = (float) (-a * 2.0 * Math.PI);
        float coslon = (float)Math.cos(lon);
        float sinlon = (float)Math.sin(lon);

        // latitude
        float lat = (float) (b * 2.0 * Math.PI);
        float coslat = (float)Math.cos(lat);
        float sinlat = (float)Math.sin(lat);

        float nx = sinlon * coslat;
        float ny = sinlat;
        float nz = coslon * coslat;
        return vec3.fromValues(nx,ny,nz);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    public void destroy()
    {
        m_Mesh.destroy();
    }
}
