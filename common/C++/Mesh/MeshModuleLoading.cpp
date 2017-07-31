// Compléments à la classe Maillage

#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#include <MeshModuleLoading.h>


/**
 * initialise un loader de fichier obj sur le maillage fourni
 * @param mesh maillage à compléter avec le fichier obj
 */
MeshModuleLoading::MeshModuleLoading(Mesh* mesh): MeshModule(mesh)
{
}


/**
 * Cette méthode recherche dans sommets celui qui possède exactement les
 * mêmes coordonnées : 3D, de texture et normale qu'indiqué
 * @param nvntnn : mot contenant les numéros nn/nt/nn
 * @param vertexlist : tableau indexé par iv de sommets au même endroit
 * @param coordlist : tableau de vec3 donnant les coordonnées des sommets
 * @param texcoordlist : tableau de vec2 donnant les coordonnées de texture
 * @param normallist : tableau de vec3 donnant les normales
 * @return le sommet correspondant au triplet (nv,nt,nn)
 */
MeshVertex* MeshModuleLoading::findOrCreateVertex(
    char* nvntnn,
    std::map<int, std::list<MeshVertex*>> &vertexlist,
    std::vector<vec3> &coordlist,
    std::vector<vec2> &texcoordlist,
    std::vector<vec3> &normallist)
{
    // indices des coordonnées 3D, des coordonnées de texture et de la normale
    int nv = 0;
    int nt = 0;
    int nn = 0;
    sscanf(nvntnn, "%d", &nv);
    sscanf(nvntnn, "%d//%d", &nv, &nn);
    sscanf(nvntnn, "%d/%d", &nv, &nt);
    sscanf(nvntnn, "%d/%d/%d", &nv, &nt, &nn);
    if (nv == 0) return nullptr;
    if (nv < 0) nv = coordlist.size() + nv; else nv = nv - 1;
    if (nt < 0) nt = texcoordlist.size() + nt; else nt = nt - 1;
    if (nt >= texcoordlist.size()) nt = -1;
    if (nn < 0) nn = normallist.size() + nn; else nn = nn - 1;
    if (nn >= normallist.size()) nn = -1;

    // nom du sommet courant, c'est son identifiant
    std::stringstream ss;
    ss << "v(" << nv << "," << nt << "," << nn << ")";
    std::string name = ss.str();

    // parcourir les sommets de la liste nv et voir s'il y a le même
    std::list<MeshVertex*>& siblings = vertexlist[nv];
    for (MeshVertex* other: siblings) {
        // si les noms sont égaux, c'est le même sommet
        if (name == other->getName()) return other;
    }

    // il faut créer un nouveau sommet car soit nouveau, soit un peu différent des autres
    MeshVertex* vertex = m_Mesh->addVertex(name);
    vertex->setCoord(coordlist[nv]);
    if (nt >= 0) vertex->setTexCoord(texcoordlist[nt]);
    if (nn >= 0) vertex->setNormal(normallist[nn]);

    // on ajoute ce sommet dans la liste de ceux qui ont le même numéro nv
    siblings.push_front(vertex);
    return vertex;
}


/**
 * Charge les données du fichier OBJ indiqué et applique une homothétie aux sommets.
 * @param objfilename : nom complet du fichier à charger
 * @param materialname : nom du matériau à lire, les autres sont ignorés, s'il vaut "", tous sont acceptés
 * @param scale : rapport d'agrandissement à appliquer
 */
void MeshModuleLoading::loadObjFile(std::string objfilename, std::string materialname, float scale)
{
    // tableaux des sommets qu'on va extraire du fichier obj, ils sont groupés par indice nv
    std::map<int, std::list<MeshVertex*>> vertexlist;

    // tableau des coordonnées, des textures et des normales
    std::vector<vec3> coordlist;
    std::vector<vec2> texcoordlist;
    std::vector<vec3> normallist;
    bool computeNormals = true;

    // matériau à prendre en compte (tous si materialname est "")
    std::string usemtl = "";

    // ouverture du fichier
    std::ifstream inputStream;
    inputStream.open(objfilename.c_str(), std::ifstream::in);
    if (! inputStream.is_open()) {
        std::cerr << "Error : \"" << objfilename << "\" cannot be loaded, check pathname and permissions." << std::endl;
        return;
    }

    // parcourir le fichier obj ligne par ligne
    char* word = NULL;
    char* saveptr_mot = NULL;
    int line_number = 0;
    char line[180];
    while (inputStream.getline(line, sizeof(line))) {
        line_number++;
        // extraire le premier mot de la ligne
        word = strtok_r(line," \t", &saveptr_mot);
        if (word == NULL) continue;
        // mettre le mot en minuscules
        for (char* c=word; *c!='\0'; ++c) *c = tolower(*c);

        if (strcmp(word,"f") == 0) {
            // le matériau est-il celui qu'on veut ?
            if (! materialname.empty() &&  materialname != usemtl) continue;
            // lire les numéros du premier point
            if (! (word = strtok_r(NULL, " \t", &saveptr_mot))) continue;
            MeshVertex* v1 = findOrCreateVertex(word, vertexlist, coordlist, texcoordlist, normallist);
            // lire les numéros du deuxième point
            if (! (word = strtok_r(NULL, " \t", &saveptr_mot))) continue;
            MeshVertex* v2 = findOrCreateVertex(word, vertexlist, coordlist, texcoordlist, normallist);
            // lire et traiter les points suivants
            while ((word = strtok_r(NULL, " \t", &saveptr_mot))) {
                MeshVertex* v3 = findOrCreateVertex(word, vertexlist, coordlist, texcoordlist, normallist);
                // ajouter un triangle v1,v2,v3
                if (v1 != nullptr && v2 != nullptr && v3 != nullptr) {
                    m_Mesh->addTriangle(v1,v2,v3);
                }
                // préparer le passage au triangle suivant
                v2 = v3;
            }
        } else
        if (strcmp(word,"v") == 0) {
            // coordonnées du sommet
            float x = atof(strtok_r(NULL, " \t", &saveptr_mot));
            float y = atof(strtok_r(NULL, " \t", &saveptr_mot));
            float z = atof(strtok_r(NULL, " \t", &saveptr_mot));
            vec3 coords = vec3::fromValues(x,y,z);
            vec3::scale(coords, coords, scale);
            coordlist.push_back(coords);
        } else
        if (strcmp(word,"vt") == 0) {
            // coordonnées de texture
            float u = atof(strtok_r(NULL, " \t", &saveptr_mot));
            float v = atof(strtok_r(NULL, " \t", &saveptr_mot));
            texcoordlist.push_back(vec2::fromValues(u,v));
        } else
        if (strcmp(word,"vn") == 0) {
            // coordonnées de la normale
            float nx = atof(strtok_r(NULL, " \t", &saveptr_mot));
            float ny = atof(strtok_r(NULL, " \t", &saveptr_mot));
            float nz = atof(strtok_r(NULL, " \t", &saveptr_mot));
            normallist.push_back(vec3::fromValues(nx,ny,nz));
            computeNormals = false;
        } else
        if (strcmp(word,"usemtl") == 0) {
            usemtl = strtok_r(NULL, " \t", &saveptr_mot);
        }
    }
    if (computeNormals) m_Mesh->computeNormals();
}
