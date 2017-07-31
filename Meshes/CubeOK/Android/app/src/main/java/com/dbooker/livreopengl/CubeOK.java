package com.dbooker.livreopengl;


import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.mesh.Mesh;
import com.dbooker.livreopengl.mesh.MeshVertex;

public class CubeOK
{
    private Mesh m_Mesh;

    public CubeOK() throws Exception
    {
        m_Mesh = new Mesh();

        /// dessus

        MeshVertex s1a = m_Mesh.addVertex("s1a");
        MeshVertex s2a = m_Mesh.addVertex("s2a");
        MeshVertex s3a = m_Mesh.addVertex("s3a");
        MeshVertex s4a = m_Mesh.addVertex("s4a");
        m_Mesh.addQuad(s1a, s2a, s3a, s4a);
        s1a.setCoord(vec3.fromValues(-1,+1,+1));
        s2a.setCoord(vec3.fromValues(+1,+1,+1));
        s3a.setCoord(vec3.fromValues(+1,+1,-1));
        s4a.setCoord(vec3.fromValues(-1,+1,-1));

        /// dessous

        MeshVertex s1b = m_Mesh.addVertex("s1b");
        MeshVertex s2b = m_Mesh.addVertex("s2b");
        MeshVertex s3b = m_Mesh.addVertex("s3b");
        MeshVertex s4b = m_Mesh.addVertex("s4b");
        m_Mesh.addQuad(s4b, s3b, s2b, s1b);
        s1b.setCoord(vec3.fromValues(-1,-1,+1));
        s2b.setCoord(vec3.fromValues(+1,-1,+1));
        s3b.setCoord(vec3.fromValues(+1,-1,-1));
        s4b.setCoord(vec3.fromValues(-1,-1,-1));

        /// devant

        MeshVertex s1c = m_Mesh.addVertex("s1c");
        MeshVertex s2c = m_Mesh.addVertex("s2c");
        MeshVertex s3c = m_Mesh.addVertex("s3c");
        MeshVertex s4c = m_Mesh.addVertex("s4c");
        m_Mesh.addQuad(s1c, s2c, s3c, s4c);
        s1c.setCoord(vec3.fromValues(-1,-1,+1));
        s2c.setCoord(vec3.fromValues(+1,-1,+1));
        s3c.setCoord(vec3.fromValues(+1,+1,+1));
        s4c.setCoord(vec3.fromValues(-1,+1,+1));

        /// derrière

        MeshVertex s1d = m_Mesh.addVertex("s1d");
        MeshVertex s2d = m_Mesh.addVertex("s2d");
        MeshVertex s3d = m_Mesh.addVertex("s3d");
        MeshVertex s4d = m_Mesh.addVertex("s4d");
        m_Mesh.addQuad(s4d, s3d, s2d, s1d);
        s1d.setCoord(vec3.fromValues(-1,-1,-1));
        s2d.setCoord(vec3.fromValues(+1,-1,-1));
        s3d.setCoord(vec3.fromValues(+1,+1,-1));
        s4d.setCoord(vec3.fromValues(-1,+1,-1));

        /// droit

        MeshVertex s1e = m_Mesh.addVertex("s1e");
        MeshVertex s2e = m_Mesh.addVertex("s2e");
        MeshVertex s3e = m_Mesh.addVertex("s3e");
        MeshVertex s4e = m_Mesh.addVertex("s4e");
        m_Mesh.addQuad(s1e, s2e, s3e, s4e);
        s1e.setCoord(vec3.fromValues(+1,-1,+1));
        s2e.setCoord(vec3.fromValues(+1,-1,-1));
        s3e.setCoord(vec3.fromValues(+1,+1,-1));
        s4e.setCoord(vec3.fromValues(+1,+1,+1));

        /// gauche

        MeshVertex s1f = m_Mesh.addVertex("s1f");
        MeshVertex s2f = m_Mesh.addVertex("s2f");
        MeshVertex s3f = m_Mesh.addVertex("s3f");
        MeshVertex s4f = m_Mesh.addVertex("s4f");
        m_Mesh.addQuad(s4f, s3f, s2f, s1f);
        s1f.setCoord(vec3.fromValues(-1,-1,+1));
        s2f.setCoord(vec3.fromValues(-1,-1,-1));
        s3f.setCoord(vec3.fromValues(-1,+1,-1));
        s4f.setCoord(vec3.fromValues(-1,+1,+1));

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
