/**
 * Cette classe représente le bord d'un triangle quand on le parcourt dans le sens trigonométrique,
 * C'est le couple orienté de deux sommets appartenant au même triangle
 */

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <stdexcept>

#include <utils.h>
#include <MeshHalfEdge.h>


#ifdef DEBUG
int MeshHalfEdge::m_DebugGenNumber = 0;
#endif


/**
 * crée une demi-arête partant d'un sommet
 * @param mesh : maillage d'appartenance de cette demi-arête
 * @param origin : sommet de départ de la demi-arête
 * @param target : sommet de fin de la demi-arête
 * @param triangle : celui situé à gauche de cette demi-arête
 */
MeshHalfEdge::MeshHalfEdge(Mesh* mesh, MeshVertex* origin, MeshVertex* target, MeshTriangle* triangle)
{
    // appartenance au maillage
    m_Mesh = mesh;
    if (mesh != nullptr) m_Mesh->pushHalfEdge(this);

    // sommet cible de la demi-arête
    m_Origin   = origin;
    m_Triangle = triangle;
    m_Edge     = nullptr;
    m_Next     = nullptr;
    m_Sibling  = nullptr;

    // vérifier que le sommet d'origine n'a pas déjà une demi-arête allant vers target
    if (origin->getHalfEdgeTo(target) != nullptr) {
        if (mesh == nullptr) {
            throw std::runtime_error("Already a half-edge between "+origin->toString()+" and "+target->toString());
        } else {
            throw std::runtime_error("In "+mesh->getName()+", already a half-edge between "+origin->toString()+" and "+target->toString());
        }
    }

    // est-ce que le sommet cible contient une demi-arête qui va vers origin ?
    m_Opposite = nullptr;
    MeshHalfEdge* other = target->getHalfEdgeTo(origin);
    if (other != nullptr) {
        // vérifier que cette autre demi-arête n'a pas d'opposée (sinon maillage pas manifold)
        if (other->getOpposite() != nullptr) {
            //std::cerr << "other="<<other->toString()<<" its target="<<other->getTarget()->toString()<<std::endl;
            if (mesh == nullptr) {
                throw std::runtime_error("Opposite half-edge already reserved between "+origin->getName()+" and "+target->getName());
            } else {
                throw std::runtime_error("In "+mesh->getName()+", opposite half-edge already reserved between "+origin->getName()+" and "+target->getName());
            }
        }
        // cette autre arête devient l'opposée de this et réciproquement
        m_Opposite = other;
        other->m_Opposite = this;
        m_Edge = m_Opposite->m_Edge;
    }

    // s'il n'y a pas d'arête, la créer
    if (m_Edge == nullptr) {
        m_Edge = new MeshEdge(mesh, origin, target);
    }

    // chaînage de la demi-arête autour du sommet origin
    origin->linkSibling(this);

    #ifdef DEBUG
    m_DebugGenNumber++;
    m_DebugNumber = m_DebugGenNumber;
    std::cerr << "new " << toString() << std::endl;
    #endif
}


/**
 * destructeur
 * Il est appelé par celui d'un triangle
 * NB : il ne détruit ni le triangle contenant cette demi-arête, ni les sommets s'ils deviennent isolés.
 * Par contre, il peut détruire l'arête correspondante si elle n'a plus aucune demi-arête.
 */
MeshHalfEdge::~MeshHalfEdge()
{
    #ifdef DEBUG
    std::cerr << "del " << toString() << std::endl;
    #endif

    // si cette demi-arête a une opposée, les séparer
    if (m_Opposite != nullptr) {
        m_Opposite->m_Opposite = nullptr;
        m_Opposite = nullptr;
    } else {
        // cette demi-arête était isolée, alors supprimer l'arête
        delete m_Edge;
    }
    m_Edge = nullptr;
    m_Next = nullptr;

    // retirer cette demi-arête du chaînage de son origine
    m_Origin->unlinkSibling(this);

    // supprimer cette demi-arête du maillage
    if (m_Mesh != nullptr) m_Mesh->popHalfEdge(this);
    m_Mesh = nullptr;

    // oublier le lien vers le triangle
    m_Triangle = nullptr;
}


/**
 * retourne une chaîne représentant cette demi-arête
 * @return nom de la demi-arête
 */
std::string MeshHalfEdge::toString()
{
    std::ostringstream str;
    #ifdef DEBUG
    str << "MeshHalfEdge "<<m_DebugNumber<<" ";
    #else
    str << "MeshHalfEdge";
    #endif
    if (m_Origin == nullptr) {
        str<<"(?,?)";
    } else {
        if (m_Opposite == nullptr || m_Opposite->m_Origin == nullptr) {
            str << "("+ m_Origin->getName()+",?)";
        } else {
            str << "("<<m_Origin->getName()<<","<<m_Opposite->m_Origin->getName()<<")";
        }
    }
    return str.str();
}


/**
 * retourne une chaîne représentant cette demi-arête de manière détaillée
 * @return description de la demi-arête
 */
std::string MeshHalfEdge::toFullString()
{
    std::ostringstream result; result << toString();

    result << ", orig="<<((m_Origin != nullptr)?m_Origin->toString():"null");
    result << ", tri="<<((m_Triangle != nullptr)?m_Triangle->toString():"null");
    result << ", edge="<<((m_Edge != nullptr)?m_Edge->toString():"null");

    result << ", sib="<<((m_Sibling != nullptr)?m_Sibling->toString():"null");
    result << ", next="<<((m_Next != nullptr)?m_Next->toString():"null");
    result << ", oppo="<<((m_Opposite != nullptr)?m_Opposite->toString():"null");

    return result.str();
}


/**
 * retourne le maillage contenant this
 * @return maillage
 */
//Mesh* MeshHalfEdge::getMesh();


/**
 * modifie le maillage associé à this
 * @param mesh : maillage
 */
void MeshHalfEdge::setMesh(Mesh* mesh)
{
    m_Mesh = mesh;
}


/**
 * modifie la demi-arête suivante de this
 * Cette méthode est appelée uniquement par le triangle qui crée cette demi-arête.
 * @param next demi-arête suivante pour tourner autour du triangle
 */
void MeshHalfEdge::setNext(MeshHalfEdge* next)
{
    m_Next = next;
}


/**
 * modifie la demi-arête suivante de this autour du sommet d'origine
 * @param sibling demi-arête suivante pour tourner autour du triangle
 */
void MeshHalfEdge::setSibling(MeshHalfEdge* sibling)
{
    m_Sibling = sibling;
}


/**
 * Cette méthode effectue la suppression du triangle associé à cette demi-arête
 * Elle reconstruit une arête, s'il faut, reliant les anciennes deux autres demi-arêtes
 * @param halfedge : demi-arête à supprimer
 */
void MeshHalfEdge::collapseTriangle(MeshHalfEdge* halfedge)
{
    // copie des variables membre qui vont disparaître
    Mesh* mesh = halfedge->m_Mesh;
    MeshVertex* origin = halfedge->m_Origin;

    // sommet opposé à cette demi-arête
    MeshVertex* vertex = halfedge->m_Next->m_Next->m_Origin;

    // demi-arête opposées côté "droit" et "gauche"
    MeshHalfEdge* oppNN = halfedge->m_Next->m_Next->m_Opposite;
    MeshHalfEdge* oppN = halfedge->m_Next->m_Opposite;

    // supprimer les arêtes de ces deux demi-arêtes
    if (oppN != nullptr) delete oppN->m_Edge;
    if (oppNN != nullptr) delete oppNN->m_Edge;

    // supprimer le triangle contenant cette demi-arête => suppression de ses 3 demi-arêtes
    delete halfedge->m_Triangle;

    // reconstruire les demi-arêtes opposées et l'arête commune
    if (oppN != nullptr || oppNN != nullptr) {
        MeshEdge* edge = new MeshEdge(mesh, origin, vertex);
        if (oppN != nullptr) {
            oppN->m_Opposite = oppNN;
            oppN->m_Edge = edge;
        }
        if (oppNN != nullptr) {
            oppNN->m_Opposite = oppN;
            oppNN->m_Edge = edge;
        }
    }
}


/**
 * modifie le maillage pour faire disparaître cette demi-arête :
 * son sommet cible est fusionné sur le sommet origine, faisant disparaître le triangle de gauche et
 * celui de droite s'il existe. L'autre demi-arête disparaît aussi.
 * NB: la méthode est statique car la demi-arête disparaît au cours de l'opération
 * @param halfedge : demi-arête à supprimer
 */
void MeshHalfEdge::collapse(MeshHalfEdge* halfedge)
{
    // copie des variables membre qui vont disparaître
    MeshVertex* origin = halfedge->m_Origin;

    // vertex cible qui va être fusionné sur origin
    MeshVertex* target = halfedge->getTarget();

    // disparition du triangle sur l'arête opposée, avec échange des demi-arêtes opposées
    if (halfedge->m_Opposite != nullptr) collapseTriangle(halfedge->m_Opposite);

    // disparition du triangle contenant cette arête avec échange des demi-arêtes opposées
    collapseTriangle(halfedge);

    // modifier l'origine de toutes les demi-arêtes sortantes entourant target
    MeshHalfEdge* other = target->getHalfEdge();
    while (other != nullptr) {
        MeshHalfEdge* next_other = other->m_Sibling;
        // enlever cette arête de la liste de la cible
        target->unlinkSibling(other);
        // changer l'origine de cette arete
        origin->linkSibling(other);
        // changer l'origine de cette demi-arête
        other->m_Origin = origin;
        // modifier l'arête
        other->m_Edge->replaceVertex(target, origin);
        // passer à la demi-arête sortante suivante
        other = next_other;
    }
}


/**
 * Cette méthode renverse l'arête diagonale this entre deux triangles jointifs,
 * c'est à dire que les triangles sont reconstruits à partir des 4 sommets,
 * avec l'arête commune placée sur l'autre diagonale.
 * Soient (A,B,C) et (C,D,A) deux triangles. Ils sont jointifs sur l'arête (A,C).
 * Le renversement change les triangles en (A,B,D) et (C,D,B).
 */
void MeshHalfEdge::flip()
{
    // demi-arête opposée
    MeshHalfEdge* opposite = m_Opposite;
    if (opposite == nullptr) return;

    // autres demi-arêtes des deux triangles
    MeshHalfEdge* halfedgenext = m_Next;
    MeshHalfEdge* halfedgenextnext = halfedgenext->m_Next;
    MeshHalfEdge* oppositenext = opposite->m_Next;
    MeshHalfEdge* oppositenextnext = oppositenext->m_Next;

    // modifier le chaînage de la demi-arête this
    m_Next = halfedgenextnext;
    halfedgenextnext->m_Next = oppositenext;
    oppositenext->m_Next = this;

    // modifier le chaînage de la demi-arête opposée
    opposite->m_Next = oppositenextnext;
    oppositenextnext->m_Next = halfedgenext;
    halfedgenext->m_Next = opposite;

    // enlever le chaînage m_Sibling des anciens sommets
    m_Origin->unlinkSibling(this);
    opposite->m_Origin->unlinkSibling(opposite);

    // modifier l'origine de this et son opposée
    m_Origin = oppositenextnext->m_Origin;
    opposite->m_Origin = halfedgenextnext->m_Origin;

    // remettre le chaînage m_Sibling sur les nouveaux sommets
    m_Origin->linkSibling(this);
    opposite->m_Origin->linkSibling(opposite);

    // refaire l'arête
    delete m_Edge;
    m_Edge = new MeshEdge(m_Mesh, m_Origin, opposite->m_Origin);
}
