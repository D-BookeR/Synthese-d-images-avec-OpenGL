package com.dbooker.livreopengl;

import java.util.HashMap;
import java.util.Map;

import com.dbooker.livreopengl.misc.SceneElement;
import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.mesh.Mesh;
import com.dbooker.livreopengl.mesh.MeshModuleLoading;
import com.dbooker.livreopengl.mesh.MeshModuleDrawing;
import com.dbooker.livreopengl.mesh.MeshModuleAnimation;
import com.dbooker.livreopengl.material.VBOset;
import com.dbooker.livreopengl.material.Material;


// Définition de la classe Pumpkin

public class Pumpkin implements SceneElement.Drawable
{
    // VBOset et matériaux
    private Map<String, VBOset> m_VBOsets = new HashMap<>();
    private Map<String, MorphMaterial> m_Materials = new HashMap<>();


    public Pumpkin() throws Exception
    {
        // définir les matériaux
        vec3 Ks = vec3.fromValues(0.165, 0.165, 0.165);
        m_Materials.put("Pumpkin01", new MorphMaterial("data/models/Pumpkin/Pumpkin01.jpg", Ks, 64.0f));
        m_Materials.put("Pumpkin02", new MorphMaterial("data/models/Pumpkin/Pumpkin02.jpg", Ks, 64.0f));
        m_Materials.put("Stalk01", new MorphMaterial("data/models/Pumpkin/Stalk01.jpg", Ks, 64.0f));
        m_Materials.put("Stalk02", new MorphMaterial("data/models/Pumpkin/Stalk02.jpg", Ks, 64.0f));

        // modules de chargement, dessin et morphing
        MeshModuleLoading loader = new MeshModuleLoading();
        MeshModuleDrawing renderer  = new MeshModuleDrawing();
        MeshModuleAnimation animation = new MeshModuleAnimation();

        // charger les maillages, un par matériau
        for (String matname: m_Materials.keySet()) {
            // obtenir le matériau
            Material material = m_Materials.get(matname);

            // charger le maillage correspondant à ce matériau
            Mesh mesh = new Mesh("Pumpkin");
            loader.setMesh(mesh);
            loader.loadObjFile("data/models/Pumpkin/pumpkin.obj", matname, 2.0f);

            // charger le maillage du morph n°1
            Mesh morph = new Mesh("Pumpkin_morph1");
            loader.setMesh(morph);
            loader.loadObjFile("data/models/Pumpkin/pumpkin_morph1.obj", matname, 2.0f);

            // construire le morph
            animation.setMesh(mesh);
            animation.buildMorph(morph);

            // construire le VBOset pour dessiner ce maillage
            renderer.setMesh(mesh);
            m_VBOsets.put(matname, renderer.createStripVBOset(material, true));

            // supprimer les maillages
            mesh.destroy();
            morph.destroy();
        }
    }


    /**
     * définit la valeur du morph pour changer la forme de l'objet
     * @param coef entre 0 et 1
     */
    public void setMorphCoef(float coef)
    {
        // fournir le coefficient à tous les matériaux
        for (MorphMaterial material: m_Materials.values()) {
            material.setMorphCoef(coef);
        }
    }


    /**
     * transforme l'objet
     * @param mat4Projection matrice de transformation caméra
     */
    public void transform(mat4 mat4Projection)
    {
    }


    /**
     * dessine l'objet
     * @param mat4Projection matrice de projection
     * @param mat4ModelView matrice de transformation caméra
     */
    public void onDraw(mat4 mat4Projection, mat4 mat4ModelView)
    {
        // dessiner les maillages
        for (VBOset vboset: m_VBOsets.values()) {
            if (vboset != null) {
                vboset.onDraw(mat4Projection, mat4ModelView);
            }
        }
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    public void destroy()
    {
        for (VBOset vboset: m_VBOsets.values()) {
            vboset.destroy();
        }
        for (MorphMaterial material: m_Materials.values()) {
            material.destroy();
        }
    }
}
