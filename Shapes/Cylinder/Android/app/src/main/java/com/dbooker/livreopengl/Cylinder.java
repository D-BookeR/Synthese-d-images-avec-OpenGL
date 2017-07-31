package com.dbooker.livreopengl;

import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.mesh.Mesh;
import com.dbooker.livreopengl.mesh.MeshModuleTopology;
import com.dbooker.livreopengl.mesh.MeshVertex;


// Définition de la classe Cylinder
public class Cylinder
{
    private Mesh m_Mesh;

    /**
     * constructeur
     * @param spokes_count : nombre de secteurs
     */
    public Cylinder(int spokes_count) throws Exception
    {
        // créer le maillage : une grille rectangulaire et deux disques
        m_Mesh = new Mesh("Cylinder");
        MeshModuleTopology topology = new MeshModuleTopology(m_Mesh);
        int diskS0 = topology.addRevolutionSurface(spokes_count, 1, "disqueS %d");
        int cylinder0    = topology.addRectangularSurface(spokes_count, 2, "cylindre %d-%d", true, false);
        int diskN0 = topology.addRevolutionSurface(spokes_count, 1, "disqueN %d");

        // définir les vertices du tube
        for (int is=0; is<spokes_count; is++) {
            // vertex sud
            MeshVertex vS = m_Mesh.getVertex(is + 1*spokes_count + cylinder0);
            vS.setCoord(getCoord((float)is / spokes_count, -1.0f));
            vS.setNormal(getNormal((float)is / spokes_count));
            // vertex nord
            MeshVertex vN = m_Mesh.getVertex(is + 0*spokes_count + cylinder0);
            vN.setCoord(getCoord((float)is / spokes_count, +1.0f));
            vN.setNormal(getNormal((float)is / spokes_count));
        }

        // définir les vertices des disques
        for (int is=0; is<spokes_count; is++) {
            // vertex sud (attention, inverser la direction)
            MeshVertex vS = m_Mesh.getVertex(is + diskS0+1);
            vS.setCoord(getCoord((float)-is / spokes_count, -1.0f));
            vS.setNormal(vec3.fromValues( 0.0,-1.0,0.0));
            // vertex nord
            MeshVertex vN = m_Mesh.getVertex(is + diskN0+1);
            vN.setCoord(getCoord((float)is / spokes_count, +1.0f));
            vN.setNormal(vec3.fromValues( 0.0,+1.0,0.0));
        }

        // définir le vertex du pole sud
        MeshVertex vS = m_Mesh.getVertex(diskS0);
        vS.setCoord(vec3.fromValues(0.0,-1.0,0.0));
        vS.setNormal(vec3.fromValues(0.0,-1.0,0.0));

        // définir le vertex du pole nord
        MeshVertex vN = m_Mesh.getVertex(diskN0);
        vN.setCoord(vec3.fromValues(0.0,+1.0,0.0));
        vN.setNormal(vec3.fromValues(0.0,+1.0,0.0));
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
     * @param b : hauteur sur l'axe y
     * @return vec3 contenant les coordonnées
     */
    private vec3 getCoord(float a, float b)
    {
        // angle de ce point
        float angle = (float) (a * 2.0*Math.PI);

        // conversion cylindriques -> cartésiennes
        float x = (float) Math.sin(angle);
        float y = b;
        float z = (float) Math.cos(angle);
        return vec3.fromValues(x,y,z);
    }


    /**
     * Cette méthode retourne la normale 3D au point identifié par (a,b)
     * @param a : paramètre variant entre 0 et 1 en tournant autour du centre
     * @return vec3 contenant les coordonnées du vecteur normal
     */
    private vec3 getNormal(float a)
    {
        // angle de ce point
        float angle = (float) (a * 2.0*Math.PI);

        float nx = (float) Math.sin(angle);
        float ny = 0.0f;
        float nz = (float) Math.cos(angle);
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
