package com.dbooker.livreopengl;


import java.util.ArrayList;

import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.mesh.Mesh;
import com.dbooker.livreopengl.mesh.MeshVertex;

public class Test
{
    private Mesh m_Mesh;

    public Test() throws Exception
    {
        final vec3 color = vec3.fromValues(1.0,0.25,0.15);

        // créer un maillage
        m_Mesh = new Mesh();

        // rajouter des sommets
        MeshVertex A = m_Mesh.addVertex("A"); A.setColor(vec3.fromValues(1.0f,0.25f,0.15f));
        MeshVertex B = m_Mesh.addVertex("B"); B.setColor(vec3.fromValues(0.25f,0.25f,1.0f));
        MeshVertex C = m_Mesh.addVertex("C"); C.setColor(vec3.fromValues(0.15f,1.0f,0.25f));
        MeshVertex D = m_Mesh.addVertex("D"); D.setColor(vec3.fromValues(1.0f,0.75f,0.25f));

        //this.mkCarre(A, B, C, D);
        this.mkTetra(A, B, C, D);

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


    private void mkSquare(MeshVertex A, MeshVertex B, MeshVertex C, MeshVertex D) throws Exception
    {
        /// Topologie : éléments et connexions

        // rajouter des triangles pour former un carré
        m_Mesh.addTriangle(A, B, C);
        m_Mesh.addTriangle(A, C, D);

        /// Géométrie : position des sommets et autres infos

        // affectation des couleurs et des coordonnées des sommets
        A.setCoord(vec3.fromValues(-2.0f, 0.0f, 2.0f));
        B.setCoord(vec3.fromValues( 2.0f, 0.0f, 2.0f));
        C.setCoord(vec3.fromValues( 2.0f, 0.0f,-2.0f));
        D.setCoord(vec3.fromValues(-2.0f, 0.0f,-2.0f));
    }


    private void mkTetra(MeshVertex A, MeshVertex B, MeshVertex C, MeshVertex D) throws Exception
    {
        /// Topologie : éléments et connexions

        // rajouter des triangles pour former un tétraèdre
        m_Mesh.addTriangle(A,B,C);
        m_Mesh.addTriangle(B,A,D);
        m_Mesh.addTriangle(B,D,C);
        m_Mesh.addTriangle(A,C,D);

        /// Géométrie : position des sommets et autres infos

        // affectation des couleurs et des coordonnées des sommets
        A.setCoord(vec3.fromValues( 1f, 1f, 1f));
        B.setCoord(vec3.fromValues(-1f,-1f, 1f));
        C.setCoord(vec3.fromValues( 1f,-1f,-1f));
        D.setCoord(vec3.fromValues(-1f, 1f,-1f));
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    public void destroy()
    {
        m_Mesh.destroy();
    }
}
