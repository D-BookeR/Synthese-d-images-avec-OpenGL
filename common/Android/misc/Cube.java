package com.dbooker.livreopengl.misc;

import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.vec2;
import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.mesh.Mesh;
import com.dbooker.livreopengl.mesh.MeshModuleDrawing;
import com.dbooker.livreopengl.material.VBOset;
import com.dbooker.livreopengl.mesh.MeshVertex;
import com.dbooker.livreopengl.material.Material;


/**
 * Cette classe permet de dessiner un maillage en forme de cube
 */
@SuppressWarnings("unused")
public class Cube
{
    private VBOset m_VBOset;

    /**
     * Constructeur
     * @param material matériau à appliquer sur le cube
     * @throws Exception
     */
    Cube(Material material) throws Exception
    {
        Mesh mesh = new Mesh("Cube");

        /// dessus

        MeshVertex s1a = mesh.addVertex("s1a");
        MeshVertex s2a = mesh.addVertex("s2a");
        MeshVertex s3a = mesh.addVertex("s3a");
        MeshVertex s4a = mesh.addVertex("s4a");
        mesh.addQuad(s1a, s2a, s3a, s4a);

        s1a.setCoord(vec3.fromValues(-1,+1,+1)); s1a.setTexCoord(vec2.fromValues(0.0, 0.0)); s1a.setNormal(vec3.fromValues(0,+1,0));
        s2a.setCoord(vec3.fromValues(+1,+1,+1)); s2a.setTexCoord(vec2.fromValues(0.0, 0.0));
        s3a.setCoord(vec3.fromValues(+1,+1,-1)); s3a.setTexCoord(vec2.fromValues(0.0, 0.0));
        s4a.setCoord(vec3.fromValues(-1,+1,-1)); s4a.setTexCoord(vec2.fromValues(0.0, 0.0));

        /// dessous

        MeshVertex s1b = mesh.addVertex("s1b");
        MeshVertex s2b = mesh.addVertex("s2b");
        MeshVertex s3b = mesh.addVertex("s3b");
        MeshVertex s4b = mesh.addVertex("s4b");
        mesh.addQuad(s4b, s3b, s2b, s1b);
        s1b.setCoord(vec3.fromValues(-1,-1,+1)); s1b.setTexCoord(vec2.fromValues(0.0, 0.0));
        s2b.setCoord(vec3.fromValues(+1,-1,+1)); s2b.setTexCoord(vec2.fromValues(0.0, 0.0));
        s3b.setCoord(vec3.fromValues(+1,-1,-1)); s3b.setTexCoord(vec2.fromValues(0.0, 0.0));
        s4b.setCoord(vec3.fromValues(-1,-1,-1)); s4b.setTexCoord(vec2.fromValues(0.0, 0.0));

        /// devant

        MeshVertex s1c = mesh.addVertex("s1c");
        MeshVertex s2c = mesh.addVertex("s2c");
        MeshVertex s3c = mesh.addVertex("s3c");
        MeshVertex s4c = mesh.addVertex("s4c");
        mesh.addQuad(s1c, s2c, s3c, s4c);
        s1c.setCoord(vec3.fromValues(-1,-1,+1)); s1c.setTexCoord(vec2.fromValues(0.0, 0.0));
        s2c.setCoord(vec3.fromValues(+1,-1,+1)); s2c.setTexCoord(vec2.fromValues(0.0, 0.0));
        s3c.setCoord(vec3.fromValues(+1,+1,+1)); s3c.setTexCoord(vec2.fromValues(0.0, 0.0));
        s4c.setCoord(vec3.fromValues(-1,+1,+1)); s4c.setTexCoord(vec2.fromValues(0.0, 0.0));

        /// derrière

        MeshVertex s1d = mesh.addVertex("s1d");
        MeshVertex s2d = mesh.addVertex("s2d");
        MeshVertex s3d = mesh.addVertex("s3d");
        MeshVertex s4d = mesh.addVertex("s4d");
        mesh.addQuad(s4d, s3d, s2d, s1d);
        s1d.setCoord(vec3.fromValues(-1,-1,-1)); s1d.setTexCoord(vec2.fromValues(0.0, 0.0));
        s2d.setCoord(vec3.fromValues(+1,-1,-1)); s2d.setTexCoord(vec2.fromValues(0.0, 0.0));
        s3d.setCoord(vec3.fromValues(+1,+1,-1)); s3d.setTexCoord(vec2.fromValues(0.0, 0.0));
        s4d.setCoord(vec3.fromValues(-1,+1,-1)); s4d.setTexCoord(vec2.fromValues(0.0, 0.0));

        /// droit

        MeshVertex s1e = mesh.addVertex("s1e");
        MeshVertex s2e = mesh.addVertex("s2e");
        MeshVertex s3e = mesh.addVertex("s3e");
        MeshVertex s4e = mesh.addVertex("s4e");
        mesh.addQuad(s1e, s2e, s3e, s4e);
        s1e.setCoord(vec3.fromValues(+1,-1,+1)); s1e.setTexCoord(vec2.fromValues(0.0, 0.0));
        s2e.setCoord(vec3.fromValues(+1,-1,-1)); s2e.setTexCoord(vec2.fromValues(0.0, 0.0));
        s3e.setCoord(vec3.fromValues(+1,+1,-1)); s3e.setTexCoord(vec2.fromValues(0.0, 0.0));
        s4e.setCoord(vec3.fromValues(+1,+1,+1)); s4e.setTexCoord(vec2.fromValues(0.0, 0.0));

        /// gauche

        MeshVertex s1f = mesh.addVertex("s1f");
        MeshVertex s2f = mesh.addVertex("s2f");
        MeshVertex s3f = mesh.addVertex("s3f");
        MeshVertex s4f = mesh.addVertex("s4f");
        mesh.addQuad(s4f, s3f, s2f, s1f);
        s1f.setCoord(vec3.fromValues(-1,-1,+1)); s1f.setTexCoord(vec2.fromValues(0.0, 0.0));
        s2f.setCoord(vec3.fromValues(-1,-1,-1)); s2f.setTexCoord(vec2.fromValues(0.0, 0.0));
        s3f.setCoord(vec3.fromValues(-1,+1,-1)); s3f.setTexCoord(vec2.fromValues(0.0, 0.0));
        s4f.setCoord(vec3.fromValues(-1,+1,+1)); s4f.setTexCoord(vec2.fromValues(0.0, 0.0));

        // calculer les normales
        mesh.computeNormals();

        // créer les VBO pour dessiner ce maillage
        MeshModuleDrawing renderer = new MeshModuleDrawing(mesh);
        m_VBOset = renderer.createStripVBOset(material, true);
    }


    /**
     * dessine l'objet
     * @param mat4Projection matrice de projection
     * @param mat4ModelView matrice qui positionne l'objet devant la caméra
     */
    public void onDraw(mat4 mat4Projection, mat4 mat4ModelView)
    {
        m_VBOset.onDraw(mat4Projection, mat4ModelView);
    }


    /** destructeur */
    public void destroy()
    {
        m_VBOset.destroy();
    }
}
