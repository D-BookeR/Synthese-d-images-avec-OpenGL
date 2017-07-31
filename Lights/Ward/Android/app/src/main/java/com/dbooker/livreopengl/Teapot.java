package com.dbooker.livreopengl;


import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.mesh.Mesh;
import com.dbooker.livreopengl.mesh.MeshModuleLoading;
import com.dbooker.livreopengl.mesh.MeshModuleDrawing;
import com.dbooker.livreopengl.material.VBOset;

public class Teapot
{
    private static final String TAG = "LivreOpenGL";

    private Mesh m_Mesh;
    private VBOset m_VBOset;

    /**
     * Constructeur
     * @param material : matériau à appliquer
     */
    public Teapot(WardMaterial material) throws Exception
    {
        // charger le maillage
        m_Mesh = new Mesh("Teapot");
        MeshModuleLoading loader = new MeshModuleLoading(m_Mesh);
        loader.loadObjFile("data/models/Teapot/teapot.obj", null, 0.02f);

        // calculer les normales
        m_Mesh.computeNormals();

        // calculer les tangentes
        m_Mesh.computeTangents();

        // créer le VBOset
        MeshModuleDrawing renderer = new MeshModuleDrawing(m_Mesh);
        m_VBOset = renderer.createStripVBOset(material, true);
    }


    /**
     * dessine l'objet sur l'écran
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    public void onDraw(mat4 mat4Projection, mat4 mat4ModelView)
    {
        // construire la matrice MV finale
        mat4 mat4ModelViewTeapot = mat4.clone(mat4ModelView);

        // décaler la théière vers le bas
        mat4.translate(mat4ModelViewTeapot, mat4ModelViewTeapot, vec3.fromValues(0.0,-0.7,0.0));

        // dessiner l'objet
        m_VBOset.onDraw(mat4Projection, mat4ModelViewTeapot);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    public void destroy()
    {
        m_Mesh.destroy();
        m_VBOset.destroy();
    }
}
