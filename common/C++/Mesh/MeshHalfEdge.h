#ifndef MESH_MESHHALFEDGE_H
#define MESH_MESHHALFEDGE_H

// Définition de la classe MeshHalfEdge

#include <vector>
#include <map>
#include <list>
#include <string>

#include <gl-matrix.h>
#include <utils.h>

#include <Mesh.h>
#include <MeshTriangle.h>
#include <MeshEdge.h>
#include <MeshVertex.h>

//#define DEBUG

/**
 * Cette classe représente le bord d'un triangle quand on le parcourt dans le sens trigonométrique,
 * C'est le couple orienté de deux sommets appartenant au même triangle
 */
class MeshHalfEdge
{
private:

    // appartenance au maillage
    Mesh* m_Mesh;

    // relations avec les autres demi-arêtes
    MeshHalfEdge* m_Sibling;        // demi-arête suivante autour du même sommet d'origine
    MeshHalfEdge* m_Next;           // demi-arête suivante dans le même triangle, son origine = la cible de this
    MeshHalfEdge* m_Opposite;       // demi-arête inverse, appartient au triangle de droite

    // relations avec les éléments du maillage
    MeshVertex* m_Origin;           // sommet situé au début de la demi-arête
    MeshTriangle* m_Triangle;       // triangle à gauche de la demi-arête
    MeshEdge* m_Edge;               // arête contenant cette demi-arête


#ifdef DEBUG
private:

    static int m_DebugGenNumber;
    int m_DebugNumber;
#endif


public:

    /**
     * crée une demi-arête partant d'un sommet
     * @param mesh maillage d'appartenance de cette demi-arête
     * @param origin sommet de départ de la demi-arête
     * @param target sommet de fin de la demi-arête
     * @param triangle situé à gauche de cette demi-arête
     */
    MeshHalfEdge(Mesh* mesh, MeshVertex* origin, MeshVertex* target, MeshTriangle* triangle);

    /**
     * destructeur
     * Il est appelé par celui d'un triangle
     * NB : il ne détruit ni le triangle contenant cette demi-arête, ni les sommets s'ils deviennent isolés.
     * Par contre, il peut détruire l'arête correspondante si elle n'a plus aucune demi-arête.
     */
    ~MeshHalfEdge();

    /**
     * retourne une chaîne représentant cette demi-arête
     * @return nom de la demi-arête
     */
    std::string toString();

    /**
     * retourne une chaîne représentant cette demi-arête de manière détaillée
     * @return description de la demi-arête
     */
    std::string toFullString();

    /**
     * retourne le maillage contenant this
     * @return maillage
     */
    Mesh* getMesh()
    {
        return m_Mesh;
    }

    /**
     * modifie le maillage associé à this
     * @param mesh
     */
    void setMesh(Mesh* mesh);

    /**
     * retourne le sommet qui est au départ de la demi-arête
     * @return sommet d'origine
     */
    MeshVertex* getOrigin()
    {
        return m_Origin;
    }

    /**
     * retourne le sommet qui est à l'extrémité de la demi-arête
     * @return sommet d'extrémité
     */
    MeshVertex* getTarget()
    {
        if (m_Next == nullptr) return nullptr;
        return m_Next->m_Origin;
    }

    /**
     * retourne la demi-arête opposée (null si aucune)
     * @return demi-arête opposée
     */
    MeshHalfEdge* getOpposite()
    {
        return m_Opposite;
    }

    /**
     * retourne le triangle d'appartenance de cette demi-arête
     * @return triangle
     */
    MeshTriangle* getTriangle()
    {
        return m_Triangle;
    }

    /**
     * retourne l'arête correspondant à cette demi-arête. Cette arête est partagée
     * avec la demi-arête opposée si elle est présente.
     * @return arete
     */
    MeshEdge* getEdge()
    {
        return m_Edge;
    }

    /**
     * retourne la demi-arête suivante dans le triangle
     * @return demi-arête suivante
     */
    MeshHalfEdge* getNext()
    {
        return m_Next;
    }

    /**
     * modifie la demi-arête suivante de this
     * Cette méthode est appelée uniquement par le triangle qui crée cette demi-arête.
     * @param next demi-arête suivante pour tourner autour du triangle
     */
    void setNext(MeshHalfEdge* next);

    /**
     * retourne la demi-arête suivante autour du sommet d'origine
     * @return demi-arête suivante
     */
    MeshHalfEdge* getSibling()
    {
        return m_Sibling;
    }

    /**
     * modifie la demi-arête suivante de this autour du sommet d'origine
     * @param sibling demi-arête suivante pour tourner autour du triangle
     */
    void setSibling(MeshHalfEdge* sibling);

private:

    /**
     * Cette méthode effectue la suppression du triangle associé à cette demi-arête
     * Elle reconstruit une arête, s'il faut, reliant les anciennes deux autres demi-arêtes
     * @param halfedge halfedge à supprimer
     */
    static void collapseTriangle(MeshHalfEdge* halfedge);

public:

    /**
     * modifie le maillage pour faire disparaître cette demi-arête :
     * son sommet cible est fusionné sur le sommet origine, faisant disparaître le triangle de gauche et
     * celui de droite s'il existe. L'autre demi-arête disparaît aussi.
     * NB: la méthode est statique car la demi-arête disparaît au cours de l'opération
     * @param halfedge halfedge à supprimer
     */
    static void collapse(MeshHalfEdge* halfedge);

    /**
     * Cette méthode renverse l'arête diagonale this entre deux triangles jointifs,
     * c'est à dire que les triangles sont reconstruits à partir des 4 sommets,
     * avec l'arête commune placée sur l'autre diagonale.
     * Soient (A,B,C) et (C,D,A) deux triangles. Ils sont jointifs sur l'arête (A,C).
     * Le renversement change les triangles en (A,B,D) et (C,D,B).
     */
    void flip();

};

#endif
