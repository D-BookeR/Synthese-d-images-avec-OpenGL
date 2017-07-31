#ifndef MESH_MESHMODULELOADING_H
#define MESH_MESHMODULELOADING_H

#include <gl-matrix.h>

#include <MeshModule.h>
#include <MeshVertex.h>



class MeshModuleLoading: public MeshModule
{

public:

    /**
     * initialise un loader de fichier obj sur le maillage fourni
     * @param mesh maillage à compléter avec le fichier obj
     */
    MeshModuleLoading(Mesh* mesh=nullptr);

    /**
     * Charge les données du fichier OBJ indiqué et applique une homothétie aux sommets.
     * @param objfilename : nom complet du fichier à charger
     * @param materialname : nom du matériau à lire, les autres sont ignorés, s'il vaut "", tous sont acceptés
     * @param scale : rapport d'agrandissement à appliquer
     */
    void loadObjFile(std::string objfilename, std::string materialname, float scale);


protected:

    /**
     * Cette méthode recherche dans sommets celui qui possède exactement les
     * mêmes coordonnées : 3D, de texture et normale qu'indiqué
     * @param nvntnn : mot contenant les numéros nn/nt/nn
     * @param vertexlist : tableau indexé par iv de sommets au même endroit
     * @param coordlist : tableau de vec3 donnant les coordonnées des sommets
     * @param texcoordlist : tableau de vec2 donnant les coordonnées de texture
     * @param normallist : tableau de vec3 donnant les normallist
     * @return le sommet correspondant au triplet (nv,nt,nn)
     */
    MeshVertex* findOrCreateVertex(
        char* nvntnn,
        std::map<int, std::list<MeshVertex*>> &vertexlist,
        std::vector<vec3> &coordlist,
        std::vector<vec2> &texcoordlist,
        std::vector<vec3> &normallist);

};

#endif
