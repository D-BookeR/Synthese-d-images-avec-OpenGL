package com.dbooker.livreopengl;


import static android.opengl.GLES20.GL_POLYGON_OFFSET_FILL;
import static android.opengl.GLES20.glColorMask;
import static android.opengl.GLES20.glDisable;
import static android.opengl.GLES20.glEnable;
import static android.opengl.GLES20.glPolygonOffset;

import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.glmatrix.vec4;
import com.dbooker.livreopengl.mesh.Mesh;
import com.dbooker.livreopengl.mesh.MeshModuleDrawing;
import com.dbooker.livreopengl.mesh.MeshModuleLoading;
import com.dbooker.livreopengl.mesh.MeshModuleProcessing;
import com.dbooker.livreopengl.material.VBOset;
import com.dbooker.livreopengl.material.Material;
import com.dbooker.livreopengl.material.ColorMaterial;
import com.dbooker.livreopengl.material.DeferredShadingMaterial;

public class CowEdges
{
    private Mesh m_Mesh;
    private Material m_TrianglesMaterial;
    private Material m_EdgesMaterial;
    private VBOset m_VBOsetTriangles;
    private VBOset m_VBOsetEdges;

    public CowEdges() throws Exception
    {
        // créer le matériau pour les triangles
        vec4 Kd = vec4.fromValues(0.9, 0.9, 0.9, 1.0);
        vec3 Ks = vec3.fromValues(0.1, 0.1, 0.1);
        float Ns = 64;
        m_TrianglesMaterial = new DeferredShadingMaterial(Kd, Ks, Ns);
        // remarque : il n'y a pas de lampe dans la scène, alors le matériau semblera plat

        // créer le matériau pour les arêtes
        vec4 black = vec4.fromValues(0.0, 0.0, 0.0, 1.0);
        m_EdgesMaterial = new ColorMaterial(black);

        // créer le maillage de la vache
        m_Mesh = new Mesh();
        MeshModuleLoading loader = new MeshModuleLoading(m_Mesh);
        loader.loadObjFile("data/models/Cow/Cow.obj", null, 0.15f);

        // en option : subdiviser le maillage en lissant ou non
        MeshModuleProcessing transformation = new MeshModuleProcessing(m_Mesh);
        m_Mesh.computeNormals();
        transformation.subdivideAll(m_Mesh.getTriangleList(), 1, 1.25f);

        // module de dessin
        MeshModuleDrawing renderer = new MeshModuleDrawing(m_Mesh);

        // finaliser l'objet : construire ses VBOs
        m_Mesh.computeNormals();
        m_VBOsetTriangles = renderer.createStripVBOset(m_TrianglesMaterial, true);

        // créer la liste d'arêtes et les VBOs pour les afficher
        m_VBOsetEdges = renderer.createEdgesVBOset(m_EdgesMaterial);
    }


    public void onDraw(mat4 mat4Projection, mat4 mat4ModelView)
    {
        boolean triangles = true;            // remplit les triangles => opaques
        boolean background_color = false;    // si true : les triangles sont de la couleur du fond

        // mettre en place un décalage de polygone
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.0f, 1.0f);

        // dessiner les triangles uniquement dans le depth buffer
        if (triangles) {

            // en option : bloquer les dessins visibles, mais garder l'écriture du depth buffer
            // ça permet d'avoir une vache toujours de la même couleur que le fond
            if (background_color) glColorMask(false, false, false, false);

            // dessiner les triangles mais ils seront invisibles
            m_VBOsetTriangles.onDraw(mat4Projection, mat4ModelView);

            // ré-auroriser les dessins sur l'écran
            glColorMask(true, true, true, true);
        }

        // supprimer le décalage
        glPolygonOffset(0.0f, 0.0f);
        glDisable(GL_POLYGON_OFFSET_FILL);

        // dessiner les arêtes
        m_VBOsetEdges.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    public void destroy()
    {
        m_VBOsetTriangles.destroy();
        m_VBOsetEdges.destroy();
        m_Mesh.destroy();
        m_TrianglesMaterial.destroy();
        m_EdgesMaterial.destroy();
    }
}
