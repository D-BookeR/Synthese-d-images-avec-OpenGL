package com.dbooker.livreopengl;

import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.mesh.Mesh;
import com.dbooker.livreopengl.mesh.MeshModuleLoading;
import com.dbooker.livreopengl.mesh.MeshModuleDrawing;
import com.dbooker.livreopengl.material.VBOset;
import com.dbooker.livreopengl.material.TextureMaterial;
import com.dbooker.livreopengl.material.Texture2D;


// Définition de la classe Ground
public class Ground
{
    private Texture2D m_Texture;
    private TextureMaterial m_Material;
    private Mesh m_Mesh;
    private VBOset m_VBOset;

    /**
     * Constructeur
     */
    public Ground() throws Exception
    {
        // créer le matériau du terrain
        m_Texture = new Texture2D("data/models/Terrain/TerrainAt.png");
        m_Material = new TextureMaterial(m_Texture);

        // charger le maillage
        m_Mesh = new Mesh("Ground");
        MeshModuleLoading loader = new MeshModuleLoading(m_Mesh);
        loader.loadObjFile("data/models/Terrain/Terrain.obj", null, 1.0f);

        // créer le VBOset
        MeshModuleDrawing renderer = new MeshModuleDrawing(m_Mesh);
        m_VBOset = renderer.createStripVBOset(m_Material, true);
    }


    /**
     * dessine l'objet sur l'écran
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    public void onDraw(mat4 mat4Projection, mat4 mat4ModelView)
    {
        m_VBOset.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    public void destroy()
    {
        m_Mesh.destroy();
        m_Material.destroy();
        m_Texture.destroy();
        m_VBOset.destroy();
    }
}
