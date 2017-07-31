#ifndef MESH_MESH_H
#define MESH_MESH_H

// Définition de la classe Mesh

#include <vector>
#include <map>
#include <list>
#include <string>

#include <gl-matrix.h>
#include <utils.h>



// pré-déclarations
class Mesh;
class MeshTriangle;
class MeshVertex;
class MeshEdge;
class MeshHalfEdge;
class Material;


#include <Material.h>

#include <MeshHalfEdge.h>
#include <MeshVertex.h>
#include <MeshTriangle.h>
#include <MeshEdge.h>


/**
 * Cette classe représente le maillage entier : ensemble de sommets et de triangles
 */
class Mesh
{
private:

    /// nom du maillage
    std::string m_Name;

    /// liste des sommets (accès direct => vector)
    std::vector<MeshVertex*> m_VertexList;

    /// liste des triangles (accès direct => vector)
    std::vector<MeshTriangle*> m_TriangleList;

    /// liste des arêtes (accès direct => vector)
    std::vector<MeshEdge*> m_EdgeList;

    /// liste des demi-arêtes (accès direct => vector)
    std::vector<MeshHalfEdge*> m_HalfEdgeList;


public:

    /**
     * constructeur
     */
    Mesh(std::string name="indef");

    /** destructeur */
    virtual ~Mesh();

    /**
     * retourne le nom du maillage
     * @return nom du maillage
     */
    std::string getName()
    {
        return m_Name;
    }


    /**
     * retourne la liste des sommets du maillage
     * @return liste des sommets
     */
    std::vector<MeshVertex*>& getVertexList()
    {
        return m_VertexList;
    }


    /**
     * retourne la liste des demi-arêtes du maillage
     * @return liste des demi-arêtes
     */
    std::vector<MeshHalfEdge*>& getHalfEdgeList()
    {
        return m_HalfEdgeList;
    }


    /**
     * retourne la liste des arêtes du maillage
     * @return liste des arêtes
     */
    std::vector<MeshEdge*>& getEdgeList()
    {
        return m_EdgeList;
    }


    /**
     * retourne la liste des triangles du maillage
     * @return liste des triangles
     */
    std::vector<MeshTriangle*>& getTriangleList()
    {
        return m_TriangleList;
    }


    /**
     * retourne le nombre de sommets
     * @return nombre de sommets
     */
    int getVertexCount()
    {
        return m_VertexList.size();
    }


    /**
     * retourne le nombre de triangles
     * @return nombre de triangles
     */
    int getTriangleCount()
    {
        return m_TriangleList.size();
    }

    /**
     * retourne le triangle n°i (0..) du maillage, ou nullptr si i n'est pas correct
     * @param i : numéro 0..NT-1 du triangle
     * @return le Triangle() demandé ou nullptr si i n'est pas dans les bornes
     */
    MeshVertex* getVertex(int i);

    /**
     * Cette fonction retourne le sommet qui porte le nom indiqué
     * NB : cette fonction est très lente dans un grand maillage
     * @param name nom du vertex à retourner
     * @return vertex ou null
     */
    MeshVertex* getVertexByName(std::string name);

    /**
     * retourne le triangle n°i (0..) du maillage, ou nullptr si i n'est pas correct
     * @param i : numéro 0..NT-1 du triangle
     * @return le Triangle() demandé ou nullptr si i n'est pas dans les bornes
     */
    MeshTriangle* getTriangle(int i);

    /**
     * affiche le nombre de sommets et de triangles sur stdout
     */
    void info();

    /**
     * Cette méthode ajoute le sommet fourni à la fin de la liste
     * @param vertex sommet à rajouter
     */
    void pushVertex(MeshVertex* vertex);

    /**
     * Cette méthode enlève le sommet fourni de la liste
     * NB: la méthode ne le supprime pas (voir son destructeur pour cela)
     * @see #delVertex
     * @param vertex sommet à enlever
     */
    void popVertex(MeshVertex* vertex);

    /**
     * Cette méthode ajoute le triangle fourni à la fin de la liste
     * @param triangle à rajouter
     */
    void pushTriangle(MeshTriangle* triangle);

    /**
     * Cette méthode enlève le triangle fourni de la liste
     * NB: la méthode ne le supprime pas (voir son destructeur pour cela)
     * @see #delTriangle
     * @param triangle à enlever
     */
    void popTriangle(MeshTriangle* triangle);

    /**
     * Cette méthode ajoute l'arête fournie à la fin de la liste
     * @param edge : arête à rajouter
     */
    void pushEdge(MeshEdge* edge);

    /**
     * Cette méthode enlève l'arête fournie de la liste
     * NB: la méthode ne le supprime pas (voir son destructeur pour cela)
     * @param edge : arête à enlever
     */
    void popEdge(MeshEdge* edge);

    /**
     * Cette méthode ajoute la demi-arête fournie à la fin de la liste
     * @param halfedge : demi-arête à rajouter
     */
    void pushHalfEdge(MeshHalfEdge* halfedge);

    /**
     * Cette méthode enlève la demi-arête fournie de la liste
     * NB: la méthode ne le supprime pas (voir son destructeur pour cela)
     * @param halfedge : halfedge à enlever
     */
    void popHalfEdge(MeshHalfEdge* halfedge);

    /**
     * Cette méthode crée et rajoute un sommet au maillage
     * @param name : nom du sommet à créer
     * @return le nouveau sommet
     */
    MeshVertex* addVertex(std::string name);

    /**
     * Cette méthode crée et rajoute un triangle au maillage.
     * NB: l'ordre de rotation des sommets est crucial pour le calcul des normales.
     * Il faut tourner dans le sens trigonométrique, comme dans OpenGL.
     * @param v1 : le premier coin du triangle
     * @param v2 : le deuxième coin du triangle
     * @param v3 : le troisième coin du triangle
     * @return le nouveau triangle, ajouté au maillage
     */
    MeshTriangle* addTriangle(MeshVertex* v1, MeshVertex* v2, MeshVertex* v3);

    /**
     * Cette méthode crée et rajoute un quadrilatère au maillage. En fait, cela
     * revient à créer deux triangles, (v1, v2, s4) et (s4, v2, v3).
     * NB: l'ordre de rotation des sommets est crucial pour le calcul des normales.
     * Il faut tourner dans le sens trigonométrique, comme dans OpenGL.
     * NB : les triangles sont définis dans l'ordre qui facilite la création de rubans
     * @param v1 : l'un des coins du quad
     * @param v2 : l'un des coins du quad
     * @param v3 : l'un des coins du quad
     * @param s4 : l'un des coins du quad
     */
    void addQuad(MeshVertex* v1, MeshVertex* v2, MeshVertex* v3, MeshVertex* s4);

    /**
     * Cette méthode crée et rajoute un éventail de triangles au maillage.
     * NB: l'ordre de rotation des sommets est crucial pour le calcul des normales.
     * Il faut tourner dans le sens trigonométrique, comme dans OpenGL.
     * @param vertices : liste des sommets du polygone
     */
    void addPolygonConvex(std::vector<MeshVertex*> vertices);

    /**
     * Cette méthode crée et rajoute un polygone au maillage. En fait, cela
     * revient à créer un éventail de triangles autour du premier point
     * NB: l'ordre de rotation des sommets est crucial pour le calcul des normales.
     * Il faut tourner dans le sens trigonométrique, comme dans OpenGL.
     * @param vertices : liste des sommets du polygone
     * @param normal direction approximative de la normale
     */
    void addPolygon(std::vector<MeshVertex*> vertices, vec3 normal);

    /**
     * Cette méthode supprime le triangle du maillage en mettant à jour toutes
     * les listes
     * @param triangle : celui qu'il faut supprimer
     */
    void delTriangle(MeshTriangle* triangle);

    /**
     * Cette méthode supprime le sommet du maillage en mettant à jour toutes
     * les listes : tous les triangles qui le contiennent sont aussi supprimés.
     * @param vertex : celui qu'il faut supprimer
     */
    void delVertex(MeshVertex* vertex);

    /**
     * Cette méthode recalcule les normales des triangles et sommets.
     * Les normales des triangles sont calculées d'après leurs côtés.
     * Les normales des sommets sont les moyennes des normales des triangles
     * auxquels ils appartiennent.
     */
    void computeNormals();

    /**
     * Cette méthode recalcule les tangentes des triangles et sommets.
     * Les tangentes des triangles sont calculées d'après leurs côtés et les coordonnées de texture.
     * Les tangentes des sommets sont les moyennes des tangentes des triangles
     * auxquels ils appartiennent.
     */
    void computeTangents();

};



#endif
