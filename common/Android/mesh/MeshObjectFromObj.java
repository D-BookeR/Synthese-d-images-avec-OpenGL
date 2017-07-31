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
public class MeshObjectFromObj extends MeshObject
{
    private String m_Folder;
    private String m_ObjFilename;
    private float m_ScaleFactor;


    /**
     * Crée un ensemble de maillages et de matériaux à partir de deux fichiers OBJ et MTL associés
     * @param folder : chemin d'accès aux deux fichiers obj et mtl
     * @param objfilename : nom du fichier .obj
     * @param mtlfilename : nom du fichier .mtl
     * @param scale : échelle à appliquer sur le maillage lors du chargement
     * @throws Exception
     */
    public MeshObjectFromObj(String folder, String objfilename, String mtlfilename, float scale) throws Exception
    {
        // paramètres
        m_Folder = folder;
        m_ObjFilename = objfilename;
        m_ScaleFactor = scale;

        // charger les matériaux du fichier MTL
        m_Materials = Material.loadMTL(m_Folder, mtlfilename);

        // module de chargement
        MeshModuleLoading loader = new MeshModuleLoading();

        // module de dessin
        MeshModuleDrawing renderer = new MeshModuleDrawing();

        // charger les maillages, un par matériau
        for (String matname: m_Materials.keySet()) {
            Material material = m_Materials.get(matname);
            Mesh mesh = new Mesh(m_ObjFilename+"@"+matname);
            m_Meshes.put(matname, mesh);
            loader.setMesh(mesh);
            loader.loadObjFile(m_Folder+"/"+m_ObjFilename, matname, m_ScaleFactor);
            renderer.setMesh(mesh);
            m_VBOsets.put(matname, renderer.createStripVBOset(material, true));
        }
    }


    /**
     * Crée un ensemble de maillages et de matériaux à partir de deux fichiers OBJ et MTL associés
     * @param folder : chemin d'accès aux deux fichiers obj et mtl
     * @param objfilename : nom du fichier .obj
     * @param materials : collection de (nom, matériau) à appliquer au maillage
     * @param scale : échelle à appliquer sur le maillage lors du chargement
     * @throws Exception
     */
    public MeshObjectFromObj(String folder, String objfilename, Map<String, Material> materials, float scale) throws Exception
    {
        // paramètres
        m_Folder = folder;
        m_ObjFilename = objfilename;
        m_ScaleFactor = scale;

        // module de chargement
        MeshModuleLoading loader = new MeshModuleLoading();

        // module de dessin
        MeshModuleDrawing renderer = new MeshModuleDrawing();

        // charger les maillages, un par matériau
        for (String matname: materials.keySet()) {
            Material material = materials.get(matname);
            Mesh mesh = new Mesh(m_ObjFilename+"@"+matname);
            m_Meshes.put(matname, mesh);
            loader.setMesh(mesh);
            loader.loadObjFile(m_Folder+"/"+m_ObjFilename, matname, m_ScaleFactor);
            renderer.setMesh(mesh);
            m_VBOsets.put(matname, renderer.createStripVBOset(material, true));
        }
    }


    /**
     * Crée un ensemble de maillages à partir d'un fichier OBJ, et avec un matériau fourni en paramètre
     * @param folder chemin d'accès aux deux fichiers obj et mtl
     * @param objfilename nom du fichier .obj
     * @param material matériau à appliquer au maillage
     * @param scale échelle à appliquer sur le maillage lors du chargement
     * @throws Exception
     */
    public MeshObjectFromObj(String folder, String objfilename, Material material, float scale) throws Exception
    {
        // paramètres
        m_Folder = folder;
        m_ObjFilename = objfilename;
        m_ScaleFactor = scale;

        // nom du matériau
        String matname = material.getName();

        // module de chargement
        MeshModuleLoading loader = new MeshModuleLoading();

        // module de dessin
        MeshModuleDrawing renderer = new MeshModuleDrawing();

        // charger le maillage, il n'a qu'un seul matériau
        Mesh mesh = new Mesh(m_ObjFilename+"@"+matname);
        m_Meshes.put(matname, mesh);
        loader.setMesh(mesh);
        loader.loadObjFile(m_Folder+"/"+m_ObjFilename, "", m_ScaleFactor);
        renderer.setMesh(mesh);
        m_VBOsets.put(matname, renderer.createStripVBOset(material, true));
    }


    /** destructeur */
    public void destroy()
    {
        super.destroy();
    }
}
