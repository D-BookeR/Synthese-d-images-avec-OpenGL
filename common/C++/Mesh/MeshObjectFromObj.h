#ifndef MESH_MESHOBJECTFROMOBJ_H
#define MESH_MESHOBJECTFROMOBJ_H

// Définition de la classe MeshObjectFromObj

#include <map>

#include <Mesh.h>
#include <Material.h>
#include <SceneElement.h>
#include <MeshObject.h>


class MeshObjectFromObj: public MeshObject
{
public:

    /**
     * Crée un ensemble de maillages et de matériaux à partir de deux fichiers OBJ et MTL associés
     * @param folder chemin d'accès aux deux fichiers obj et mtl
     * @param objfilename nom du fichier .obj
     * @param mtlfilename nom du fichier .mtl
     * @param scale échelle à appliquer sur le maillage lors du chargement
     */
    MeshObjectFromObj(std::string folder, std::string objfilename, std::string mtlfilename, float scale=1.0);

    /**
     * Crée un ensemble de maillages à partir d'un fichier OBJ, et avec un matériau fourni en paramètre
     * @param folder chemin d'accès aux deux fichiers obj et mtl
     * @param objfilename nom du fichier .obj
     * @param materials collection de (nom, matériau) à appliquer au maillage
     * @param scale échelle à appliquer sur le maillage lors du chargement
     */
    MeshObjectFromObj(std::string folder, std::string objfilename, std::map<std::string, Material*> materials, float scale=1.0);

    /**
     * Crée un ensemble de maillages à partir d'un fichier OBJ, et avec un matériau fourni en paramètre
     * @param folder chemin d'accès aux deux fichiers obj et mtl
     * @param objfilename nom du fichier .obj
     * @param material unique à appliquer au maillage
     * @param scale échelle à appliquer sur le maillage lors du chargement
     */
    MeshObjectFromObj(std::string folder, std::string objfilename, Material* material, float scale=1.0);

    /** Destructeur */
    virtual ~MeshObjectFromObj();


protected:

    std::string m_Folder;
    std::string m_ObjFilename;
    float m_ScaleFactor;
};

#endif
