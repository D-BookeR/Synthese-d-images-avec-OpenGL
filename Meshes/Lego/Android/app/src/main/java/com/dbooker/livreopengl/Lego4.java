package com.dbooker.livreopengl;


import java.util.ArrayList;

import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.mesh.Mesh;
import com.dbooker.livreopengl.mesh.MeshModuleProcessing;
import com.dbooker.livreopengl.mesh.MeshVertex;

public class Lego4
{
    private Mesh m_Mesh;

    public Lego4() throws Exception
    {
        vec3 color = vec3.fromValues(1.0,0.25,0.15);

        // créer quelques polygones
        m_Mesh = new Mesh();

        // rajouter des sommets
        MeshVertex A = m_Mesh.addVertex("A");
        MeshVertex B = m_Mesh.addVertex("B");
        MeshVertex C = m_Mesh.addVertex("C");
        MeshVertex D = m_Mesh.addVertex("D");
        MeshVertex E = m_Mesh.addVertex("E");
        MeshVertex F = m_Mesh.addVertex("F");
        MeshVertex G = m_Mesh.addVertex("G");
        MeshVertex H = m_Mesh.addVertex("H");

        // rajouter des triangles et des quads
        m_Mesh.addTriangle(A, E, B);
        m_Mesh.addTriangle(B, E, G);
        m_Mesh.addTriangle(B, G, D);
        m_Mesh.addTriangle(D, G, H);
        m_Mesh.addTriangle(D, H, C);
        m_Mesh.addTriangle(C, H, F);
        m_Mesh.addTriangle(C, F, A);
        m_Mesh.addTriangle(A, F, E);
        m_Mesh.addQuad(E, F, H, G);


        /// Géométrie : position des sommets et autres info

        // affectation des couleurs et des coordonnées des sommets
        A.setCoord(vec3.fromValues(-2.0, 0.0,-2.0)); A.setColor(color);
        B.setCoord(vec3.fromValues( 2.0, 0.0,-2.0)); B.setColor(color);
        C.setCoord(vec3.fromValues(-2.0, 0.0, 2.0)); C.setColor(color);
        D.setCoord(vec3.fromValues( 2.0, 0.0, 2.0)); D.setColor(color);

        E.setCoord(vec3.fromValues( 0.0, 0.0,-1.0)); E.setColor(color);
        F.setCoord(vec3.fromValues(-1.0, 0.0, 0.0)); F.setColor(color);
        G.setCoord(vec3.fromValues( 1.0, 0.0, 0.0)); G.setColor(color);
        H.setCoord(vec3.fromValues( 0.0, 0.0, 1.0)); H.setColor(color);

        // calculer les normales
        m_Mesh.computeNormals();

        // module de transformation pour ce maillage
        MeshModuleProcessing transformation = new MeshModuleProcessing(m_Mesh);

        // faire une extrusion de la bosse
        ArrayList<MeshVertex> edge = new ArrayList<MeshVertex>();
        edge.add(E); edge.add(F); edge.add(H); edge.add(G);
        transformation.extrudePolygon(edge, 3.0f);

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
     * Cette méthode supprime les ressources allouées
     */
    public void destroy()
    {
        m_Mesh.destroy();
    }
}
