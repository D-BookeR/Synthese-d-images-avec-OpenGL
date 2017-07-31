// Définition de la classe MeshObjectFromObj

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>
#include <math.h>

#include <utils.h>

#include <MeshObjectFromObj.h>
#include <MeshModuleLoading.h>
#include <MeshModuleDrawing.h>


/**
 * Crée un ensemble de maillages et de matériaux à partir de deux fichiers OBJ et MTL associés
 * @param folder : chemin d'accès aux deux fichiers obj et mtl
 * @param objfilename : nom du fichier .obj
 * @param mtlfilename : nom du fichier .mtl
 * @param scale : échelle à appliquer sur le maillage lors du chargement
 */
MeshObjectFromObj::MeshObjectFromObj(std::string folder, std::string objfilename, std::string mtlfilename, float scale)
{
    // paramètres
    m_Folder = folder;
    m_ObjFilename = objfilename;
    m_ScaleFactor = scale;

    // charger les matériaux du fichier MTL
    m_Materials = Material::loadMTL(m_Folder, mtlfilename);

    // module de chargement
    MeshModuleLoading loader;

    // module de dessin
    MeshModuleDrawing renderer;

    // charger les maillages, un par matériau
    for (auto const& it: m_Materials) {
        std::string matname = it.first;
        Material* material = it.second;
        Mesh* mesh = new Mesh(m_ObjFilename+"@"+matname);
        m_Meshes[matname] = mesh;
        loader.setMesh(mesh);
        loader.loadObjFile(m_Folder+"/"+m_ObjFilename, matname, m_ScaleFactor);
        renderer.setMesh(mesh);
        m_VBOsets[matname] = renderer.createStripVBOset(material, true);
    }
}


/**
 * Crée un ensemble de maillages et de matériaux à partir de deux fichiers OBJ et MTL associés
 * @param folder : chemin d'accès aux deux fichiers obj et mtl
 * @param objfilename : nom du fichier .obj
 * @param materials : collection de (nom, matériau) à appliquer au maillage
 * @param scale : échelle à appliquer sur le maillage lors du chargement
 */
MeshObjectFromObj::MeshObjectFromObj(std::string folder, std::string objfilename, std::map<std::string, Material*> materials, float scale)
{
    // paramètres
    m_Folder = folder;
    m_ObjFilename = objfilename;
    m_ScaleFactor = scale;

    // module de chargement
    MeshModuleLoading loader;

    // module de dessin
    MeshModuleDrawing renderer;

    // charger les maillages, un par matériau
    for (auto const& it: materials) {
        std::string matname = it.first;
        Material* material = it.second;
        // créer un maillage pour ce matériau
        Mesh* mesh = new Mesh(m_ObjFilename+"@"+matname);
        m_Meshes[matname] = mesh;
        loader.setMesh(mesh);
        loader.loadObjFile(m_Folder+"/"+m_ObjFilename, matname, m_ScaleFactor);
        renderer.setMesh(mesh);
        m_VBOsets[matname] = renderer.createStripVBOset(material, true);
    }
}


/**
 * Crée un ensemble de maillages à partir d'un fichier OBJ, et avec un matériau fourni en paramètre
 * @param folder : chemin d'accès aux deux fichiers obj et mtl
 * @param objfilename : nom du fichier .obj
 * @param material : matériau à appliquer au maillage
 * @param scale : échelle à appliquer sur le maillage lors du chargement
 */
MeshObjectFromObj::MeshObjectFromObj(std::string folder, std::string objfilename, Material* material, float scale)
{
    // paramètres
    m_Folder = folder;
    m_ObjFilename = objfilename;
    m_ScaleFactor = scale;

    // nom du matériau
    std::string matname = material->getName();

    // module de chargement
    MeshModuleLoading loader;

    // module de dessin
    MeshModuleDrawing renderer;

    // charger le maillage, il n'a qu'un seul matériau
    Mesh* mesh = new Mesh(m_ObjFilename+"@"+matname);
    m_Meshes[matname] = mesh;
    loader.setMesh(mesh);
    loader.loadObjFile(m_Folder+"/"+m_ObjFilename, "", m_ScaleFactor);
    renderer.setMesh(mesh);
    m_VBOsets[matname] = renderer.createStripVBOset(material, true);
}


/**
 * destructeur
 */
MeshObjectFromObj::~MeshObjectFromObj()
{
}
