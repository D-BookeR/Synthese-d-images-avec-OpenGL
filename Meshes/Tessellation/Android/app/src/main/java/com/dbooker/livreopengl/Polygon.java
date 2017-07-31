package com.dbooker.livreopengl;


import java.util.ArrayList;
import java.util.Arrays;

import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.mesh.Mesh;
import com.dbooker.livreopengl.mesh.MeshVertex;

public class Polygon
{
    private Mesh m_Mesh;

    public Polygon() throws Exception
    {
        // créer l'objet maillage
        this.m_Mesh = new Mesh("Polygon");

        // rajouter des sommets
        MeshVertex A = this.m_Mesh.addVertex("A");
        A.setColor(vec3.fromValues(0.5, 0.8, 0.2));
        A.setCoord(vec3.fromValues(0.5, -2.0, 0.0));

        MeshVertex B = this.m_Mesh.addVertex("B");
        B.setColor(vec3.fromValues(0.5, 0.8, 0.2));
        B.setCoord(vec3.fromValues(2.0, -1.0, 0.0));

        MeshVertex C = this.m_Mesh.addVertex("C");
        C.setColor(vec3.fromValues(0.5, 0.8, 0.2));
        C.setCoord(vec3.fromValues(1.0, 0.0, 0.0));

        MeshVertex D = this.m_Mesh.addVertex("D");
        D.setColor(vec3.fromValues(0.5, 0.8, 0.2));
        D.setCoord(vec3.fromValues(2.0, 1.5, 0.0));

        MeshVertex E = this.m_Mesh.addVertex("E");
        E.setColor(vec3.fromValues(0.5, 0.8, 0.2));
        E.setCoord(vec3.fromValues(0.0, 0.5, 0.0));

        MeshVertex F = this.m_Mesh.addVertex("F");
        F.setColor(vec3.fromValues(0.5, 0.8, 0.2));
        F.setCoord(vec3.fromValues(0.5, 2.0, 0.0));

        MeshVertex G = this.m_Mesh.addVertex("G");
        G.setColor(vec3.fromValues(0.5, 0.8, 0.2));
        G.setCoord(vec3.fromValues(-2.0, 1.0, 0.0));

        MeshVertex H = this.m_Mesh.addVertex("H");
        H.setColor(vec3.fromValues(0.5, 0.8, 0.2));
        H.setCoord(vec3.fromValues(-0.5, 0.0, 0.0));

        MeshVertex I = this.m_Mesh.addVertex("I");
        I.setColor(vec3.fromValues(0.5, 0.8, 0.2));
        I.setCoord(vec3.fromValues(-2.0, -1.0, 0.0));

        // ajouter un polygone
        //this.m_Mesh.addPolygonConvex(new ArrayList<>(Arrays.asList(A,B,C,D,E,F,G,H,I)));
        this.m_Mesh.addPolygon(new ArrayList<>(Arrays.asList(A,B,C,D,E,F,G,H,I)), vec3.fromValues(0.0, 0.0, 1.0));
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
