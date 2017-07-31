#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <stdexcept>

#include <utils.h>
#include <MeshTriangle.h>


#ifdef DEBUG
int MeshTriangle::m_DebugGenNumero = 0;
#endif


/**
 * Constructeur de la classe Triangle à partir de trois sommets
 * NB: l'ordre de rotation des sommets est crucial pour le calcul des normales.
 * Il faut tourner dans le sens trigonométrique, comme dans OpenGL.
 * @param mesh : maillage dans lequel on rajoute ce triangle
 * @param v1 : l'un des coins du triangle, nullptr si création manuelle
 * @param v2 : l'un des coins du triangle, nullptr si création manuelle
 * @param v3 : l'un des coins du triangle, nullptr si création manuelle
 */
MeshTriangle::MeshTriangle(Mesh* mesh, MeshVertex* v1, MeshVertex* v2, MeshVertex* v3)
{
    // appartenance au maillage
    m_Mesh = mesh;
    if (mesh != nullptr) m_Mesh->pushTriangle(this);

    // créer trois demi-arêtes sauf si les sommets sont nuls
    if (v1 != nullptr && v1 != nullptr && v3 != nullptr) {
        MeshHalfEdge* he12 = new MeshHalfEdge(mesh, v1, v2, this);
        MeshHalfEdge* he23 = new MeshHalfEdge(mesh, v2, v3, this);
        MeshHalfEdge* he31 = new MeshHalfEdge(mesh, v3, v1, this);

        // chaîner ces demi-arêtes
        he12->setNext(he23);
        he23->setNext(he31);
        he31->setNext(he12);

        // le triangle est constitué de cette chaîne
        m_HalfEdge = he12;
    } else {
        m_HalfEdge = nullptr;
    }

    // seront initialisées dans calcNormale()
    m_Normal  = vec3::create();
    m_Tangent = vec3::create();
    m_Center  = vec3::create();
    m_Surface = 0.0;
    m_W       = 0.0;

    #ifdef DEBUG
    m_DebugGenNumero++;
    m_DebugNumero = m_DebugGenNumero;
    std::cerr << "new " << toString() << std::endl;
    #endif
}


MeshTriangle::MeshTriangle(Mesh* mesh)
{
    // appartenance au maillage
    m_Mesh = mesh;
    if (mesh != nullptr) m_Mesh->pushTriangle(this);

    // pas de demi-arête (création manuelle)
    m_HalfEdge = nullptr;

    // seront initialisées dans calcNormale()
    m_Normal  = vec3::create();
    m_Tangent = vec3::create();
    m_Center  = vec3::create();
    m_Surface = 0.0;
    m_W       = 0.0;
}


/**
 * Cette méthode supprime ce triangle du maillage en mettant à jour toutes
 * les listes. Cela peut supprimer des arêtes et rendre des sommets isolés.
 */
MeshTriangle::~MeshTriangle()
{
    #ifdef DEBUG
    std::cerr << "del " << toString() << std::endl;
    #endif

    // supprimer ce triangle de la liste du maillage
    if (m_Mesh != nullptr) m_Mesh->popTriangle(this);
    m_Mesh = nullptr;

    // supprimer les trois demi-arêtes de ce triangle
    if (m_HalfEdge != nullptr) {
        delete m_HalfEdge->getNext()->getNext();
        delete m_HalfEdge->getNext();
        delete m_HalfEdge;
    }
    m_HalfEdge = nullptr;
}


/**
 * retourne une chaîne qui décrit le triangle
 * @return nom du triangle
 */
std::string MeshTriangle::toString()
{
    // récupérer les trois sommets
    MeshVertex* A = getVertex0();
    std::string nA = (A == nullptr) ? "?" : A->getName();
    MeshVertex* B = getVertex1();
    std::string nB = (B == nullptr) ? "?" : B->getName();
    MeshVertex* C = getVertex2();
    std::string nC = (C == nullptr) ? "?" : C->getName();

    // chaîne mentionnant les noms des sommets
    #ifdef DEBUG
    std::ostringstream str;
    str << "Triangle "<<m_DebugNumero<<" ("<<nA+","<<nB<<","<<nC<<")";
    return str.str();
    #else
    return "Triangle("+nA+","+nB+","+nC+")";
    #endif
}


/**
 * retourne une chaîne qui décrit le triangle de manière détaillée
 * @return description du triangle
 */
std::string MeshTriangle::toFullString()
{
    if (m_HalfEdge != nullptr) {
        return toString()+" -> "+m_HalfEdge->toString();
    } else {
        return toString();
    }
}


/**
 * retourne le maillage contenant this
 * @return maillage
 */
//Mesh* MeshTriangle::getMesh();


/**
 * modifie le maillage associé à this
 * @param maillage
 */
void MeshTriangle::setMesh(Mesh* mesh)
{
    m_Mesh = mesh;
}


/**
 * retourne les sommets du triangle sous forme d'un itérable
 * @return itérable sur les trois sommets du triangle
 */
VertexList MeshTriangle::getVertexList()
{
    return VertexList(this);
}

VertexList::iterator::iterator(MeshTriangle* triangle, int i)
{
    m_I = i;
    m_HalfEdge = triangle->getHalfEdge();
}

MeshVertex* VertexList::iterator::operator*() const
{
    return m_HalfEdge->getOrigin();
}

const VertexList::iterator& VertexList::iterator::operator++()
{
    if (m_I>0) {
        m_I--;
        m_HalfEdge = m_HalfEdge->getNext();
    }
    return *this;
}

bool VertexList::iterator::operator!=(const iterator& other) const
{
    return m_I != other.m_I;
}


/**
 * retourne le sommet n°n (0..2) du triangle, ou nullptr si n n'est pas correct
 * @param n : numéro 0..2 du sommet
 * @return le sommet demandé ou nullptr si n n'est pas dans les bornes
 */
MeshVertex* MeshTriangle::getVertex(int n)
{
    if (n < 0 || n > 2) return nullptr;

    MeshHalfEdge* halfedge = m_HalfEdge;
    for (int i=0; i<n; i++) {
        halfedge = halfedge->getNext();
    }
    return halfedge->getOrigin();
}


/**
 * retourne la demi-arête du triangle
 * @return demi-arête
 */
//MeshHalfEdge* Triangle::getHalfEdge();


/**
 * retourne le premier sommet du triangle
 * @return premier sommet
 */
MeshVertex* MeshTriangle::getVertex0()
{
    if (m_HalfEdge == nullptr) return nullptr;
    return m_HalfEdge->getOrigin();
}


/**
 * retourne le deuxième sommet du triangle
 * @return deuxième sommet
 */
MeshVertex* MeshTriangle::getVertex1()
{
    if (m_HalfEdge == nullptr) return nullptr;
    if (m_HalfEdge->getNext() == nullptr) return nullptr;
    return m_HalfEdge->getNext()->getOrigin();
}


/**
 * retourne le troisième sommet du triangle
 * @return troisième sommet
 */
MeshVertex* MeshTriangle::getVertex2()
{
    if (m_HalfEdge == nullptr) return nullptr;
    if (m_HalfEdge->getNext() == nullptr) return nullptr;
    if (m_HalfEdge->getNext()->getNext() == nullptr) return nullptr;
    return m_HalfEdge->getNext()->getNext()->getOrigin();
}


/**
 * retourne la valeur de la normale
 * @see #calcNormale pour la calculer auparavant
 * @return normale du triangle
 */
//vec3 MeshTriangle::getNormal();


/**
 * retourne la valeur de la surface
 * @see #calcNormale pour la calculer auparavant
 * @return surface du triangle
 */
//float MeshTriangle::getSurface();


/**
 * retourne le centre du triangle
 * @see #calcNormale pour le calculer auparavant
 * @return centre du triangle
 */
//vec3 MeshTriangle::getCenter();


/**
 * retourne la valeur w du triangle
 * @return w
 */
//float Triangle::getW();


/**
 * affecte la variable w du triangle
 * @param w variable
 */
void MeshTriangle::setW(float w)
{
    m_W = w;
}


/**
 * recalcule les informations géométriques du triangle : centre, normale, surface...
 */
void MeshTriangle::computeNormal()
{
    // les trois sommets
    MeshVertex* A = m_HalfEdge->getOrigin();
    MeshVertex* B = m_HalfEdge->getNext()->getOrigin();
    MeshVertex* C = m_HalfEdge->getNext()->getNext()->getOrigin();

    // les coordonnées des trois sommets
    vec3 cA = A->getCoord();
    vec3 cB = B->getCoord();
    vec3 cC = C->getCoord();

    // vecteurs AB et AC
    vec3 cAB = vec3::create();
    vec3::subtract(cAB, cB, cA);
    vec3 cAC = vec3::create();
    vec3::subtract(cAC, cC, cA);

    // calculer les coordonnées du centre du triangle
    vec3::zero(m_Center);
    vec3::add(m_Center, cA, cB);
    vec3::add(m_Center, m_Center, cC);
    vec3::scale(m_Center, m_Center, 1.0/3.0);

    // calculer le vecteur normal
    vec3::cross(m_Normal, cAB, cAC);

    // surface et coefficient W
    m_Surface = 0.5 * vec3::length(m_Normal);
    vec3::normalize(m_Normal, m_Normal);
    m_W = -vec3::dot(m_Normal, cA);
}


/**
 * retourne la valeur de la tangente
 * @see #calcTangente pour la calculer auparavant
 * @return tangente du triangle
 */
//vec3 Triangle::getTangent();


/**
 * recalcule la tangente du triangle à l'aide de la normale et des coordonnées de texture
 */
void MeshTriangle::computeTangent()
{
    // les trois sommets
    MeshVertex* A = m_HalfEdge->getOrigin();
    MeshVertex* B = m_HalfEdge->getNext()->getOrigin();
    MeshVertex* C = m_HalfEdge->getNext()->getNext()->getOrigin();

    // les coordonnées des trois sommets
    vec3 cA = A->getCoord();
    vec3 cB = B->getCoord();
    vec3 cC = C->getCoord();

    // vecteurs AB et AC
    vec3 cAB = vec3::create();
    vec3::subtract(cAB, cB, cA);
    vec3 cAC = vec3::create();
    vec3::subtract(cAC, cC, cA);

    // récupération de leur 2e coordonnée de texture
    float tA = A->getTexCoord()[1];
    float tB = B->getTexCoord()[1];
    float tC = C->getTexCoord()[1];

    // vecteurs dans l'espace (s,t), et uniquement la coordonnée t
    float tAB = tB - tA;
    float tAC = tC - tA;

    // TODO s'il n'y a pas de coordonnées de texture, alors tAB et tAC sont nuls, les remplacer par AB et AC

    // calcul de la tangente
    vec3::scale(cAB, cAB, tAC);
    vec3::scale(cAC, cAC, tAB);
    vec3::subtract(m_Tangent, cAB, cAC);

    // normalisation
    vec3::normalize(m_Tangent, m_Tangent);
}


/**
 * Cette méthode fait tourner les sommets du triangle afin que
 * celui qui est passé en paramètre se trouve en premier
 * @param vertex : sommet à mettre en tête de liste (sauf s'il en est absent)
 * @return true si ok, false si le sommet est absent du triangle
 */
bool MeshTriangle::cycleVertexFirst(MeshVertex* vertex)
{
    // est-ce que sommet = origine de la première demi-arête ?
    if (m_HalfEdge->getOrigin() == vertex) return true;

    // est-ce que sommet = origine de la deuxième demi-arête ?
    m_HalfEdge = m_HalfEdge->getNext();
    if (m_HalfEdge->getOrigin() == vertex) return true;

    // est-ce que sommet = origine de la troisième demi-arête ?
    m_HalfEdge = m_HalfEdge->getNext();
    if (m_HalfEdge->getOrigin() == vertex) return true;

    // échec, le sommet n'est origine d'aucune demi-arête
    return false;
}


/**
 * Cette méthode indique si le triangle this contient le sommet indiqué
 * @param vertex : sommet dont il faut vérifier l'appartenance à this
 * @return true si ok, false si le sommet est absent du triangle
 */
bool MeshTriangle::containsVertex(MeshVertex* vertex)
{
    if (m_HalfEdge->getOrigin() == vertex) return true;
    if (m_HalfEdge->getNext()->getOrigin() == vertex) return true;
    if (m_HalfEdge->getNext()->getNext()->getOrigin() == vertex) return true;
    return false;
}


/**
 * Cette méthode indique si le triangle this contient l'arête indiquée. Et
 * si c'est le cas, alors vertex1 est mis en premier dans le triangle. Le
 * triangle est transformé en (vertex1, vertex2, 3e sommet).
 * @param vertex1 : premier sommet de l'arête (elle est orientée de 1 à 2)
 * @param vertex2 : second sommet
 * @return true si oui, false si l'arête n'est pas dans le triangle
 */
bool MeshTriangle::containsEdge(MeshVertex* vertex1, MeshVertex* vertex2)
{
    if (! cycleVertexFirst(vertex1)) return false;
    return m_HalfEdge->getNext()->getOrigin() == vertex2;
}


/**
 * remplace un sommet par un autre dans ce triangle
 * @param A sommet à remplacer
 * @param A1 sommet à mettre à la place
 */
void MeshTriangle::replaceVertex(MeshVertex* A, MeshVertex* A1)
{
    // mettre A en premier
    if (! cycleVertexFirst(A)) return;

    // les deux autres sommets actuels du triangle
    MeshVertex* B = getVertex1();
    MeshVertex* C = getVertex2();

    // demi-arêtes du triangle
    MeshHalfEdge* daAB = m_HalfEdge;
    MeshHalfEdge* daBC = daAB->getNext();
    MeshHalfEdge* daCA = daBC->getNext();

    // supprimer les deux demi-arêtes AB et CA
    delete daAB;
    delete daCA;
    daBC->setNext(nullptr);

    // créer deux demi-arêtes allant de et vers A1
    daAB = new MeshHalfEdge(m_Mesh, A1, B, this);
    daCA = new MeshHalfEdge(m_Mesh, C, A1, this);

    // chaîner ces demi-arêtes
    daAB->setNext(daBC);
    daBC->setNext(daCA);
    daCA->setNext(daAB);

    // le triangle est constitué de cette chaîne
    m_HalfEdge = daAB;
}
