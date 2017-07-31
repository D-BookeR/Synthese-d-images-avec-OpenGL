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
 * Cette classe permet de dessiner un mesh en forme d'étoile
 */
@SuppressWarnings("unused")
public class Plane
{
    private VBOset m_VBOset;


    /**
     * constructeur
     * @param material : matériau à appliquer sur le plan
     * @throws Exception
     */
    Plane(Material material) throws Exception
    {
        Mesh mesh = new Mesh("Plane");

        MeshVertex s1 = mesh.addVertex("s1a");
        MeshVertex s2 = mesh.addVertex("s2a");
        MeshVertex s3 = mesh.addVertex("s3a");
        MeshVertex s4 = mesh.addVertex("s4a");
        mesh.addQuad(s1, s2, s3, s4);
        s1.setCoord(vec3.fromValues(-1,0,+1)); s1.setTexCoord(vec2.fromValues(0.0, 1.0)); s1.setNormal(vec3.fromValues(0,+1,0));
        s2.setCoord(vec3.fromValues(+1,0,+1)); s2.setTexCoord(vec2.fromValues(1.0, 1.0)); s2.setNormal(vec3.fromValues(0,+1,0));
        s3.setCoord(vec3.fromValues(+1,0,-1)); s3.setTexCoord(vec2.fromValues(1.0, 0.0)); s3.setNormal(vec3.fromValues(0,+1,0));
        s4.setCoord(vec3.fromValues(-1,0,-1)); s4.setTexCoord(vec2.fromValues(0.0, 0.0)); s4.setNormal(vec3.fromValues(0,+1,0));

        // calculer les normales
        mesh.computeNormals();

        // préparer le VBOset pour le dessiner
        MeshModuleDrawing renderer = new MeshModuleDrawing(mesh);
        m_VBOset = renderer.createStripVBOset(material, true);
    }


    /**
     * dessine l'objet
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice qui positionne l'objet devant la caméra
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
