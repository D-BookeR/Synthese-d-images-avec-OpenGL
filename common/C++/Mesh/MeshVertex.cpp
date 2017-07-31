#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <stdexcept>

#include <utils.h>
#include <MeshVertex.h>
#include <MeshModuleRedux.h>


#ifdef DEBUG
int MeshVertex::m_DebugGenNumber = 0;
#endif


/**
 * Constructeur de la classe MeshVertex qui représente un sommet dans
 * le maillage. Initialise un sommet non connecté au maillage, employer
 * Mesh.addVertex pour l'insérer puis new Triangle(...) pour le mettre
 * dans un triangle, setCoord et setColor pour lui donner des coordonnées et
 * des couleurs.
 * @param mesh : maillage d'appartenance de ce sommet
 * @param name : nom du sommet à créer
 */
MeshVertex::MeshVertex(Mesh* mesh, std::string name)
{
    // appartenance au maillage
    m_Mesh = mesh;
    if (mesh != nullptr) {
        m_Mesh->pushVertex(this);

        // numéro du vertex dans les VBO
        m_Number = m_Mesh->getVertexCount();
    }

    // nom du sommet
    m_Name = name;

    // attributs du sommet, voir setCoord(), setColor(), setNormal()...
    m_Attributes.push_back(vec4::create());      // ID_ATTR_VERTEX
    m_Attributes.push_back(vec4::create());      // ID_ATTR_COLOR
    m_Attributes.push_back(vec4::create());      // ID_ATTR_NORMAL
    m_Attributes.push_back(vec4::create());      // ID_ATTR_TANGENT
    m_Attributes.push_back(vec4::create());      // ID_ATTR_TEXCOORD
    m_Attributes.push_back(vec4::create());      // ID_ATTR_VERTEX1
    m_Attributes.push_back(vec4::create());      // ID_ATTR_NORMAL1
    m_Attributes.push_back(vec4::create());      // ID_ATTR_IDBONES
    m_Attributes.push_back(vec4::create());      // ID_ATTR_WEIGHTS

    // demi-arête
    m_HalfEdge = nullptr;

    // divers
    m_Quadric = nullptr;
    m_CollapseTarget = nullptr;
    m_CollapseCost = 1e38;

    #ifdef DEBUG
    m_DebugGenNumber++;
    m_DebugNumber = m_DebugGenNumber;
    std::cerr << "new " << toString() << std::endl;
    #endif
}


/** destructeur */
MeshVertex::~MeshVertex()
{
    #ifdef DEBUG
    std::cerr << "del " << toString() << std::endl;
    #endif

    // supprimer ce sommet de la liste du maillage
    if (m_Mesh != nullptr) m_Mesh->popVertex(this);
    m_Mesh = nullptr;

    // supprimer la quadrique si elle existe
    if (m_Quadric != nullptr) delete m_Quadric;

    // enlever tous les triangles autour de ce sommet
    MeshHalfEdge* halfedge = m_HalfEdge;
    while (halfedge != nullptr) {
        MeshHalfEdge* next = halfedge->getSibling();
        MeshTriangle* triangle = halfedge->getTriangle();
        delete halfedge->getTriangle();
        halfedge = next;
    }
}


/**
 * retourne le maillage contenant this
 * @return maillage
 */
//Mesh* MeshVertex::getMesh();


/**
 * modifie le maillage associé à this
 * @param maillage
 */
void MeshVertex::setMesh(Mesh* mesh)
{
    m_Mesh = mesh;
}


/**
 * retourne la première MeshHalfEdge de this
 * @return MeshHalfEdge
 */
//MeshHalfEdge* MeshVertex::getHalfEdge();


/**
 * cette méthode rajoute la demi-arête dans le chaînage m_Sibling (demi-arêtes autour de this)
 * @see #unlinkSibling qui fait l'inverse
 * @param halfedge à rajouter dans la liste m_Sibling de this
 */
void MeshVertex::linkSibling(MeshHalfEdge* halfedge)
{
    // mettre la demi-arête en tête de la liste
    halfedge->setSibling(m_HalfEdge);
    m_HalfEdge = halfedge;
}


/**
 * cette méthode retire cette demi-arête de la liste m_Sibling (demi-arêtes autour de this)
 * @see #linkSibling qui fait l'inverse
 * @param halfedge à enlever de la liste m_Sibling de this
 */
void MeshVertex::unlinkSibling(MeshHalfEdge* halfedge)
{
    // si le sommet this désigne la demi-arête, alors le faire désigner la suivante
    if (m_HalfEdge == halfedge) {
        // this doit maintenant désigner la demi-arête suivante dans sa liste
        m_HalfEdge = m_HalfEdge->getSibling();
        // si m_HalfEdge==nullptr, alors le sommet this devient libre (supprimable)
    } else {
        // le sommet désigne une autre demi-arête, il faut parcourir le chaînage
        MeshHalfEdge* other = m_HalfEdge;
        while (other != nullptr) {
            if (other->getSibling() == halfedge) {
                other->setSibling(halfedge->getSibling());
                break;
            }
            other = other->getSibling();
        }
    }
    // la demi-arête n'a plus de suivant
    halfedge->setSibling(nullptr);
}


/**
 * retourne la liste des triangles autour du sommet this
 * Cette liste est basée sur les liens m_Sibling entre triangles
 * NB : la liste n'est pas du tout triée par ordre autour du sommet
 * @see #getTrianglesOrderedAround pour une liste triée mais pas forcément complète
 * @return liste des triangles
 */
std::vector<MeshTriangle*> MeshVertex::getTrianglesAround()
{
    std::vector<MeshTriangle*> triangles;
    MeshHalfEdge* halfedge = m_HalfEdge;  // demi-arête courante

    while (halfedge != nullptr) {
        MeshTriangle* triangle = halfedge->getTriangle();
        triangles.push_back(triangle);
        halfedge = halfedge->getSibling();
    }
    return triangles;
}


/**
 * retourne la liste des triangles autour du sommet this
 * Cette liste est basée sur les liens m_Next et m_Opposite entre triangles
 * NB : la liste est triée par ordre autour du sommet, mais si le maillage n'est
 * pas complet, alors il manquera une partie des triangles
 * @see #getTrianglesAutour pour une liste non triée mais complète
 * @return liste des triangles
 */
std::vector<MeshTriangle*> MeshVertex::getTrianglesOrderedAround()
{
    std::vector<MeshTriangle*> triangles;
    MeshHalfEdge* halfedge = m_HalfEdge;   // demi-arête courante
    MeshHalfEdge* first    = m_HalfEdge;   // première demi-arête vue

    while (halfedge != nullptr) {
        MeshTriangle* triangle = halfedge->getTriangle();
        triangles.push_back(triangle);
        // on essaie d'aller sur la m_Next.m_Next.m_Opposite
        if (halfedge->getNext() != nullptr) halfedge = halfedge->getNext();
        if (halfedge->getNext() != nullptr) halfedge = halfedge->getNext();
        if (halfedge->getOpposite() != nullptr) halfedge = halfedge->getOpposite();
        // sommes-nous revenus sur la première demi-arête ?
        if (halfedge == first) break;
    }
    return triangles;
}


/**
 * Cette méthode retourne la collection des sommets contenus dans les triangles
 * qui contiennent le sommet this, c'est à dire les voisins de this.
 * @return liste des sommets voisins du sommet, mais sans this
 */
std::set<MeshVertex*> MeshVertex::getNeighborVertices()
{
    // collection retournée par la méthode
    std::set<MeshVertex*> neighbors;

    // parcourir les demi-arêtes autour de this
    MeshHalfEdge* trianglehalfedge = m_HalfEdge;
    while (trianglehalfedge != nullptr) {
        MeshTriangle* triangle = trianglehalfedge->getTriangle();

        // ajouter les trois sommets du triangle sauf s'ils y sont déjà
        MeshHalfEdge* vertexhalfedge = triangle->getHalfEdge();
        for (int i=0; i<3; i++) {
            if (vertexhalfedge->getOrigin() != this) {
                neighbors.insert(vertexhalfedge->getOrigin());
            }
            // passer à la demi-arête suivante sur ce triangle
            vertexhalfedge = vertexhalfedge->getNext();
        }

        // passer à la demi-arête suivante autour de this
        trianglehalfedge = trianglehalfedge->getSibling();
    }

    return neighbors;
}


/**
 * retourne la demi-arête allant de this à vertex
 * @param vertex : destination de la demi-arête recherchée
 * @return demi-arête allant de this à vertex, ou nullptr si absente
 */
MeshHalfEdge* MeshVertex::getHalfEdgeTo(MeshVertex* vertex)
{
    MeshHalfEdge* halfedge = m_HalfEdge;
    while (halfedge != nullptr) {
        if (halfedge->getTarget() == vertex) return halfedge;
        halfedge = halfedge->getSibling();
    }
    // pas trouvée
    return nullptr;
}


/**
 * retourne le triangle situé à droite de l'arête allant de this à vertex
 * @param vertex : second sommet
 * @return triangle ou nullptr si aucun
 */
MeshTriangle* MeshVertex::getTriangleRightTo(MeshVertex* vertex)
{
    // chercher la demi-arête entre this et vertex
    MeshHalfEdge* halfedge = getHalfEdgeTo(vertex);
    if (halfedge == nullptr || halfedge->getOpposite() == nullptr) return nullptr;
    return halfedge->getOpposite()->getTriangle();
}


/**
 * retourne le triangle situé à gauche de l'arête allant de this à vertex
 * @param vertex : second sommet
 * @return triangle ou nullptr si aucun
 */
MeshTriangle* MeshVertex::getTriangleLeftTo(MeshVertex* vertex)
{
    // chercher la demi-arête entre this et vertex
    MeshHalfEdge* halfedge = getHalfEdgeTo(vertex);
    if (halfedge == nullptr) return nullptr;
    return halfedge->getTriangle();
}


/**
 * retourne le nom du sommet
 * @return le nom du sommet
 */
std::string MeshVertex::toString()
{
    #ifdef DEBUG
    std::ostringstream str;
    str << "Sommet "<<m_DebugNumber<<" "<<m_Name;
    return str.str();
    #else
    return m_Name;
    #endif
}


/**
 * retourne une chaîne représentant ce sommet de manière détaillée
 * @return description du sommet
 */
std::string MeshVertex::toFullString()
{
    std::string result = toString();
    MeshHalfEdge* halfedge = m_HalfEdge;
    while (halfedge != nullptr) {
        result += " -> "+halfedge->toString();
        halfedge = halfedge->getSibling();
    }
    return result;
}


/**
 * retourne le nom du vertex
 * @return nom du sommet
 */
//std:string MeshVertex::getName();


/**
 * modifie le numéro du vertex
 * @param number : numero du sommet
 */
void MeshVertex::setNumber(int number)
{
    m_Number = number;
}


/**
 * retourne le numéro du vertex
 * @return numéro du sommet
 */
//int MeshVertex::getNumber();


/**
 * définit les coordonnées du sommet
 * @param xyz coordonnées
 * @return this pour pouvoir chaîner les affectations
 */
MeshVertex* MeshVertex::setCoord(vec3 xyz)
{
    vec3::copy((vec3&)m_Attributes[ID_ATTR_VERTEX], xyz);
    return this;
}


/**
 * retourne les coordonnées du sommet
 * @return coordonnées 3D du sommet
 */
//vec3& MeshVertex::getCoord();


/**
 * définit la couleur du sommet
 * @param rgba couleur (r,g,b,a)
 * @return this pour pouvoir chaîner les affectations
 */
MeshVertex* MeshVertex::setColor(vec4 rgba)
{
    vec4::copy(m_Attributes[ID_ATTR_COLOR], rgba);
    return this;
}
/**
 * définit la couleur du sommet
 * @param rgb couleur (r,g,b)
 * @return this pour pouvoir chaîner les affectations
 */
MeshVertex* MeshVertex::setColor(vec3 rgb)
{
    vec3::copy((vec3&)m_Attributes[ID_ATTR_COLOR], rgb);
    m_Attributes[ID_ATTR_COLOR][3] = 1.0;
    return this;
}


/**
 * retourne la couleur du sommet
 * @return couleur (r,g,b,a)
 */
//vec4& MeshVertex::getColor();


/**
 * définit les coordonnées de la normale du sommet
 * @param normal : normale à affecter
 * @return this pour pouvoir chaîner les affectations
 */
MeshVertex* MeshVertex::setNormal(vec3 normal)
{
    vec3::copy((vec3&)m_Attributes[ID_ATTR_NORMAL], normal);
    return this;
}


/**
 * retourne la normale du sommet
 * @return normale
 */
//vec3& MeshVertex::getNormal();


/**
 * retourne la tangente du sommet
 * @return tangente
 */
//vec3& MeshVertex::getTangent();


/**
 * définit les coordonnées de texture du sommet
 * @param uv coordonnées de texture
 * @return this pour pouvoir chaîner les affectations
 */
MeshVertex* MeshVertex::setTexCoord(vec2 uv)
{
    vec2::copy((vec2&)m_Attributes[ID_ATTR_TEXCOORD], uv);
    return this;
}


/**
 * retourne les coordonnées de texture du sommet
 * @return coordonnées de texture
 */
//vec2& MeshVertex::getTexCoord();


/**
 * retourne l'attribut dont on fournit l'identifiant
 * @param idattr : numéro de l'attribut, ex: MeshVertex::ID_ATTR_VERTEX
 * @return vec4 contenant l'attribut
 */
//vec4 MeshVertex::getAttribute(int idattr);


/**
 * affecte ou définit l'attribut dont on fournit l'identifiant
 * @param idattr : numéro de l'attribut, ex: MeshVertex.ID_ATTR_VERTEX
 * @param value : valeur à affecter à l'attribut
 * @return this pour pouvoir chaîner les affectations
 */
MeshVertex* MeshVertex::setAttribute(int idattr, vec4 value)
{
    vec4::copy(m_Attributes[idattr], value);
    return this;
}
MeshVertex* MeshVertex::setAttribute(int idattr, vec3 value)
{
    vec3::copy((vec3&)m_Attributes[idattr], value);
    return this;
}
MeshVertex* MeshVertex::setAttribute(int idattr, vec2 value)
{
    vec2::copy((vec2&)m_Attributes[idattr], value);
    return this;
}


/**
 * Cette méthode calcule la normale du sommet = moyenne des normales des
 * triangles contenant ce sommet.
 */
void MeshVertex::computeNormal()
{
    // calculer la moyenne des normales des triangles contenant ce sommet
    vec3::zero((vec3&)m_Attributes[ID_ATTR_NORMAL]);
    MeshHalfEdge* halfedge = m_HalfEdge;
    while (halfedge != nullptr) {
        MeshTriangle* triangle = halfedge->getTriangle();
        // normale du triangle courant
        vec3 normal = vec3::clone(triangle->getNormal());
        // prendre en compte la surface du triangle
        vec3::scale(normal, normal, triangle->getSurface());
        // cumuler les normales pondérées
        vec3::add((vec3&)m_Attributes[ID_ATTR_NORMAL], (vec3&)m_Attributes[ID_ATTR_NORMAL], normal);
        // passer à la demi-arête suivante autour de ce sommet
        halfedge = halfedge->getSibling();
    }
    // normaliser le résultat
    vec3::normalize((vec3&)m_Attributes[ID_ATTR_NORMAL], (vec3&)m_Attributes[ID_ATTR_NORMAL]);
}


/**
 * Cette méthode calcule la tangente du sommet = moyenne des tangentes des
 * triangles contenant ce sommet.
 */
void MeshVertex::computeTangent()
{
    // calculer la moyenne des tangentes des triangles contenant ce sommet
    vec3::zero((vec3&)m_Attributes[ID_ATTR_TANGENT]);
    MeshHalfEdge* halfedge = m_HalfEdge;
    while (halfedge != nullptr) {
        MeshTriangle* triangle = halfedge->getTriangle();
        // tangente du triangle courant
        vec3 tangent = vec3::clone(triangle->getTangent());
        // prendre en compte la surface du triangle
        vec3::scale(tangent, tangent, triangle->getSurface());
        // cumuler les tangentes pondérées
        vec3::add((vec3&)m_Attributes[ID_ATTR_TANGENT], (vec3&)m_Attributes[ID_ATTR_TANGENT], tangent);
        // passer à la demi-arête suivante autour de ce sommet
        halfedge = halfedge->getSibling();
    }
    // normaliser le résultat
    vec3::normalize((vec3&)m_Attributes[ID_ATTR_TANGENT], (vec3&)m_Attributes[ID_ATTR_TANGENT]);
}


/**
 * retourne le sommet visé pour une fusion
 * @see MeshModuleRedux
 * @return sommet
 */
//MeshVertex* MeshVertex::getCollapseTarget();


/**
 * modifie le sommet visé pour une fusion
 * @see MeshModuleRedux
 * @param vertex : sommet à affecter
 */
void MeshVertex::setCollapseTarget(MeshVertex* vertex)
{
    m_CollapseTarget = vertex;
}


/**
 * retourne le coût d'une fusion vers m_CollapseTarget
 * @see MeshModuleRedux
 * @return coût
 */
//float MeshVertex::getCollapseCost();


/**
 * modifie le coût d'une fusion vers m_CollapseTarget
 * @see MeshModuleRedux
 * @param cost : coût à affecter
 */
void MeshVertex::setCollapseCost(float cost)
{
    m_CollapseCost = cost;
}


/**
 * retourne la quadrique permettant de calculer le coût d'une réduction
 * @see MeshModuleRedux
 * @return quadrique
 */
//Quadric* MeshVertex::getQuadric();


/**
 * modifie la quadrique permettant de calculer le coût d'une réduction
 * @see MeshModuleRedux
 * @param quadric : quadrique à affecter
 */
void MeshVertex::setQuadric(Quadric* quadric)
{
    m_Quadric = quadric;
}


/**
 * retourne une copie de ce sommet (this)
 * @param suffix : chaine à rajouter au nom de this
 * @return copie du sommet this
 */
MeshVertex* MeshVertex::clone(std::string suffix)
{
    MeshVertex* copie = new MeshVertex(m_Mesh, this->m_Name+suffix);
    // recopier les attributs du vertex
    for (int i=0; i<m_Attributes.size(); i++) {
        vec4::copy(copie->m_Attributes[i], m_Attributes[i]);
    }
    return copie;
}


/**
 * Cette méthode modifie les attributs du vertex pour qu'ils soient interpolés
 * linéairement entre ceux de v0 et de v1, d'un facteur k
 * @param v0 : premier vertex, (k=0)
 * @param v1 : second vertex (k=1)
 * @param k : valeur entre 0 et 1 pour l'interpolation
 */
void MeshVertex::lerp(const MeshVertex* v0, const MeshVertex* v1, const float k)
{
    // interpoler les attributs
    for (int i=0; i<m_Attributes.size(); i++) {
        vec4::lerp(m_Attributes[i], v0->m_Attributes[i], v1->m_Attributes[i], k);
    }
    // normaliser les vecteurs
    vec3::normalize((vec3&)m_Attributes[ID_ATTR_NORMAL],  (vec3&)m_Attributes[ID_ATTR_NORMAL]);
    vec3::normalize((vec3&)m_Attributes[ID_ATTR_TANGENT], (vec3&)m_Attributes[ID_ATTR_TANGENT]);
    vec3::normalize((vec3&)m_Attributes[ID_ATTR_NORMAL1], (vec3&)m_Attributes[ID_ATTR_NORMAL1]);
}


/**
 * Cette méthode modifie les attributs du vertex pour qu'ils soient interpolés
 * selon Hermite entre ceux de v0 et de v1, d'un facteur k
 * @param v0 : premier vertex, (k=0)
 * @param t0 : tangente à k=0
 * @param v1 : second vertex (k=1)
 * @param t1 : tangente à k=1
 * @param k : coefficient d'interpolation
 */
void MeshVertex::hermite(const MeshVertex* v0, const vec3& t0, const MeshVertex* v1, const vec3& t1, const float k)
{
    // interpoler les coordonnées par hermite
    vec3::hermite((vec3&)m_Attributes[ID_ATTR_VERTEX],
        (vec3&)v0->m_Attributes[ID_ATTR_VERTEX], t0,
        (vec3&)v1->m_Attributes[ID_ATTR_VERTEX], t1,
        k);
    // interpoler linéairement les autres attributs (il faudrait leurs dérivées)
    for (int i=1; i<m_Attributes.size(); i++) {
        vec4::lerp(m_Attributes[i], v0->m_Attributes[i], v1->m_Attributes[i], k);
    }
    // normaliser les vecteurs
    vec3::normalize((vec3&)m_Attributes[ID_ATTR_NORMAL], (vec3&)m_Attributes[ID_ATTR_NORMAL]);
    vec3::normalize((vec3&)m_Attributes[ID_ATTR_TANGENT], (vec3&)m_Attributes[ID_ATTR_TANGENT]);
    vec3::normalize((vec3&)m_Attributes[ID_ATTR_NORMAL1], (vec3&)m_Attributes[ID_ATTR_NORMAL1]);
}
