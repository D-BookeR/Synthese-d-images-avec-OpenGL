package com.dbooker.livreopengl.mesh;

import java.util.HashMap;
import java.util.Map;

import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.vec4;
import com.dbooker.livreopengl.mesh.Mesh;
import com.dbooker.livreopengl.mesh.MeshModuleDrawing;
import com.dbooker.livreopengl.mesh.MeshModuleLoading;
import com.dbooker.livreopengl.material.VBOset;
import com.dbooker.livreopengl.material.Material;


/**
 * Crée un ensemble de maillages et de matériaux à partir de deux fichiers OBJ et MTL associés
 */
@SuppressWarnings("unused")
public class MeshObject implements com.dbooker.livreopengl.misc.SceneElement.Drawable
{
    // dictionnaire des maillages (nom_matériau, maillage)
    protected Map<String, Mesh> m_Meshes = new HashMap<>();

    // dictionnaire des matériaux (nom_matériau, matériau)
    protected Map<String, Material> m_Materials;

    // dictionnaire des VBOsets (nom_matériau, VBOset)
    protected Map<String, VBOset> m_VBOsets = new HashMap<>();


    /**
     * Crée un ensemble de maillages et de matériaux
     * @throws Exception
     */
    public MeshObject() throws Exception
    {
    }


    /** destructeur */
    public void destroy()
    {
        for (VBOset vboset: m_VBOsets.values()) {
            vboset.destroy();
        }
        for (Mesh mesh: m_Meshes.values()) {
            mesh.destroy();
        }
        for (Material material: m_Materials.values()) {
            material.destroy();
        }
    }


    /**
     * transforme l'objet
     * @param mat4ModelView matrice qui positionne l'objet devant la caméra
     */
    public void transform(mat4 mat4ModelView)
    {
    }


    /**
     * dessine l'objet
     * @param mat4Projection matrice de projection
     * @param mat4ModelView matrice qui positionne l'objet devant la caméra
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
     * définit un plan de coupe pour les fragments. Ce plan est en coordonnées caméra
     * @param active : true s'il faut compiler un shader gérant le plan de coupe
     * @param plane vec4 en coordonnées caméra ou null pour supprimer le plan de coupe
     * @throws Exception
     */
    public void setClipPlane(boolean active, vec4 plane) throws Exception
    {
        // fournir le plan de coupe aux matériaux
        for (Material material: m_Materials.values()) {
            material.setClipPlane(active, plane);
        }
    }


    public void setClipPlane(boolean active) throws Exception
    {
        // fournir le plan de coupe aux matériaux
        for (Material material: m_Materials.values()) {
            material.setClipPlane(active);
        }
    }
}
