#ifndef MESH_MESHMODULEDRAWING_H
#define MESH_MESHMODULEDRAWING_H

#include <MeshModule.h>
#include <Material.h>
#include <VBOset.h>
#include <MeshTriangle.h>


class MeshModuleDrawing: public MeshModule
{

public:

    /**
     * initialise un renderer sur le maillage fourni
     * @param mesh maillage à dessiner
     */
    MeshModuleDrawing(Mesh* mesh=nullptr);

    /**
     * Cette méthode crée tous les VBO pour dessiner le maillage à l'aide de triangles
     * @param material : celui qu'il faut employer pour dessiner les triangles
     * @param interleaved : true s'il faut entrelacer les données
     */
    VBOset* createVBOset(Material* material, bool interleaved=true);

    /**
     * Cette méthode crée tous les VBO pour dessiner le maillage à l'aide de triangle_strips
     * @param material : celui qu'il faut employer pour dessiner les triangles du ruban
     * @param interleaved : true s'il faut entrelacer les données
     */
    VBOset* createStripVBOset(Material* material, bool interleaved=true);

    /**
     * Cette méthode crée tous les VBO pour dessiner les arêtes du maillage
     * @param material : celui qu'il faut employer pour dessiner les arêtes
     */
    VBOset* createEdgesVBOset(Material* material);

    /**
     * Cette méthode crée tous les VBO pour dessiner les normales des facettes du maillage
     * @param material : celui qu'il faut employer pour dessiner les normales
     * @param length : nombre donnant la longueur des normales à afficher
     */
    VBOset* createFacesNormalsVBOset(Material* material, float length);

    /**
     * Cette méthode crée tous les VBO pour dessiner les normales des sommets du maillage
     * @param materiau : celui qu'il faut employer pour dessiner les normales
     * @param length : nombre donnant la longueur des normales à afficher
     */
    VBOset* createVertexNormalsVBOset(Material* material, float length);

};



/**
 * Cette classe représente un triangle strip
 * @param triangle : le premier triangle d'un ruban
 * @remark l'ordre des sommets du triangle est important pour commencer un ruban !
 * @remark Ruban hérite de Triangle, ou inversement
 */
class TriangleStrip
{

private:

    std::vector<MeshVertex*> m_VertexList;

public:

    /** constructeur d'un ruban ne contenant qu'un triangle initialement */
    TriangleStrip(MeshTriangle* triangle);

    /**
     * retourne la longueur du ruban, son nombre de sommets
     * @return nombre de sommets du ruban
     */
    int size()
    {
        return m_VertexList.size();
    }

    /**
     * retourne la liste des sommets du ruban
     * @return liste des sommets
     */
     std::vector<MeshVertex*> getVertexList()
     {
         return m_VertexList;
     }

    /**
     * retourne l'un des sommets du ruban, désigné par son numéro
     * @param i = le numéro du sommet, si <0 alors par rapport à la fin du tableau
     * @return le ie sommet du ruban
     */
    MeshVertex* getVertex(int i);

    /**
     * Cette méthode tente d'ajouter le triangle au ruban
     * @param triangle à rajouter
     * @return true si le triangle a pu être mis dans le ruban, false sinon
     */
    bool appendTriangle(MeshTriangle* triangle);

    /**
     * Cette méthode tente d'ajouter un ruban de 3 sommets au ruban
     * @param autre ruban à rajouter
     * @return true si l'autre ruban a pu être mis dans le ruban, false sinon
     */
    bool appendStrip3(TriangleStrip* autre);

private:

    /**
     * retourne true si le ruban contient 3 sommets, dont s1 suivi de s2
     * Si true, alors au retour, s1 et s2 sont dans les deux premières places du ruban
     * @param vertex0 : premier sommet de l'arête (elle est orientée de 1 à 2)
     * @param vertex1 : second sommet
     * @return true si oui, false si l'arête n'est pas dans le triangle
     */
    bool containsEdge(MeshVertex* vertex0, MeshVertex* vertex1);

public:

    /**
     * Cette méthode tente d'ajouter le ruban fourni au ruban this
     * @param ruban à rajouter
     * @return true si le ruban a pu être concaténé à this, false sinon
     */
    bool concat(TriangleStrip* ruban);

};

#endif
