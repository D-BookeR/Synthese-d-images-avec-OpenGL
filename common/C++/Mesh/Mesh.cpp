#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <stdexcept>

#include <utils.h>
#include <Mesh.h>
#include <Material.h>


/**
 * Cette classe représente un maillage 3D : triangles et sommets
 * @param name nom du maillage
 */
Mesh::Mesh(std::string name)
{
    m_Name = name;
}


/**
 * destructeur
 */
Mesh::~Mesh()
{
    // supprimer les triangles
    for (MeshTriangle* triangle: m_TriangleList) {
        triangle->setMesh(nullptr);
        delete triangle;
    }
    // supprimer les sommets
    for (MeshVertex* vertex: m_VertexList) {
        vertex->setMesh(nullptr);
        delete vertex;
    }
    // supprimer les arêtes qui resteraient
    for (MeshEdge* edge: m_EdgeList) {
        edge->setMesh(nullptr);
        delete edge;
    }
    // supprimer les demi-arêtes qui resteraient
    for (MeshHalfEdge* halfedge: m_HalfEdgeList) {
        halfedge->setMesh(nullptr);
        delete halfedge;
    }
}


/**
 * retourne le nom du maillage
 * @return nom du maillage
 */
//std::string Mesh::getName();


/**
 * retourne la liste des sommets du maillage
 * @return liste des sommets
 */
//std::vector<MeshVertex>& Mesh::getVertexList();


/**
 * retourne la liste des demi-arêtes du maillage
 * @return liste des demi-arêtes
 */
//std::vector<MeshHalfEdge>& Mesh::getHalfEdgeList();


/**
 * retourne la liste des arêtes du maillage
 * @return liste des arêtes
 */
//std::vector<Edge>& Mesh::getEdgeList();


/**
 * retourne la liste des triangles du maillage
 * @return liste des triangles
 */
//std::vector<Triangle>& Mesh::getTriangleList();


/**
 * retourne le nombre de sommets
 * @return nombre de sommets
 */
//int Mesh::getVertexCount();


/**
 * retourne le nombre de triangles
 * @return nombre de triangles
 */
//int Mesh::getTriangleCount();


/**
 * retourne le sommet n°i (0..) du maillage, ou null si i n'est pas correct
 * @param i : numéro 0..NV-1 du sommet
 * @return le sommet demandé ou nullptr si i n'est pas dans les bornes
 */
MeshVertex* Mesh::getVertex(int i)
{
    if (i<0 || i>= m_VertexList.size()) return nullptr;
    return m_VertexList[i];
}


/**
 * Cette fonction retourne le sommet qui porte le nom indiqué
 * NB : cette fonction est très lente dans un grand maillage
 * @param name : nom du vertex à retourner
 * @return vertex ou nullptr
 */
MeshVertex* Mesh::getVertexByName(std::string name)
{
    for (MeshVertex* vertex: m_VertexList) {
        if (vertex->getName() == name) return vertex;
    }
    return nullptr;
}


/**
 * retourne le triangle n°i (0..) du maillage, ou nullptr si i n'est pas correct
 * @param i : numéro 0..NT-1 du triangle
 * @return le Triangle() demandé ou nullptr si i n'est pas dans les bornes
 */
MeshTriangle* Mesh::getTriangle(int i)
{
    if (i<0 || i>= m_TriangleList.size()) return nullptr;
    return m_TriangleList[i];
}


/**
 * affiche le nombre de sommets et de triangles sur stdout
 */
void Mesh::info()
{
    std::cout << m_VertexList.size() << " vertices and " << m_TriangleList.size() << " triangles" << std::endl;
}


/**
 * Cette méthode ajoute le sommet fourni à la fin de la liste
 * @param vertex : sommet à rajouter
 */
void Mesh::pushVertex(MeshVertex* vertex)
{
    m_VertexList.push_back(vertex);
}


/**
 * Cette méthode enlève le sommet fourni de la liste
 * NB: la méthode ne le supprime pas (voir son destructeur pour cela)
 * @see #delVertex
 * @param vertex : sommet à enlever
 */
void Mesh::popVertex(MeshVertex* vertex)
{
    list_remove(m_VertexList, vertex);
}


/**
 * Cette méthode ajoute le triangle fourni à la fin de la liste
 * @param triangle à rajouter
 */
void Mesh::pushTriangle(MeshTriangle* triangle)
{
    m_TriangleList.push_back(triangle);
}


/**
 * Cette méthode enlève le triangle fourni de la liste
 * NB: la méthode ne le supprime pas (voir son destructeur pour cela)
 * @see #delTriangle
 * @param triangle à enlever
 */
void Mesh::popTriangle(MeshTriangle* triangle)
{
    list_remove(m_TriangleList, triangle);
}


/**
 * Cette méthode ajoute l'arête fournie à la fin de la liste
 * @param edge : arête à rajouter
 */
void Mesh::pushEdge(MeshEdge* edge)
{
    m_EdgeList.push_back(edge);
}


/**
 * Cette méthode enlève l'arête fournie de la liste
 * NB: la méthode ne le supprime pas (voir son destructeur pour cela)
 * @param edge : arête à enlever
 */
void Mesh::popEdge(MeshEdge* edge)
{
    list_remove(m_EdgeList, edge);
}


/**
 * Cette méthode ajoute la demi-arête fournie à la fin de la liste
 * @param halfedge : demi-arête à rajouter
 */
void Mesh::pushHalfEdge(MeshHalfEdge* halfedge)
{
    m_HalfEdgeList.push_back(halfedge);
}


/**
 * Cette méthode enlève la demi-arête fournie de la liste
 * NB: la méthode ne le supprime pas (voir son destructeur pour cela)
 * @param halfedge : halfedge à enlever
 */
void Mesh::popHalfEdge(MeshHalfEdge* halfedge)
{
    list_remove(m_HalfEdgeList, halfedge);
}


/**
 * Cette méthode crée et rajoute un sommet au maillage
 * @param name : nom du sommet à créer
 * @return le nouveau sommet
 */
MeshVertex* Mesh::addVertex(std::string name)
{
    return new MeshVertex(this, name);
}


/**
 * Cette méthode crée et rajoute un triangle au maillage.
 * NB: l'ordre de rotation des sommets est crucial pour le calcul des normales.
 * Il faut tourner dans le sens trigonométrique, comme dans OpenGL.
 * @param v1 : le premier coin du triangle
 * @param v2 : le deuxième coin du triangle
 * @param v3 : le troisième coin du triangle
 * @return le nouveau triangle, ajouté au maillage
 */
MeshTriangle* Mesh::addTriangle(MeshVertex* v1, MeshVertex* v2, MeshVertex* v3)
{
    return new MeshTriangle(this, v1, v2, v3);
}


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
void Mesh::addQuad(MeshVertex* v1, MeshVertex* v2, MeshVertex* v3, MeshVertex* s4)
{
    addTriangle(v1, v2, s4);
    addTriangle(s4, v2, v3);
}


/**
 * Cette méthode crée et rajoute un éventail de triangles au maillage.
 * NB: l'ordre de rotation des sommets est crucial pour le calcul des normales.
 * Il faut tourner dans le sens trigonométrique, comme dans OpenGL.
 * @param vertices : liste des sommets du polygone
 */
void Mesh::addPolygonConvex(std::vector<MeshVertex*> vertices)
{
    // créer un éventail autour du premier point fourni
    MeshVertex* pivot = vertices[0];
    int count = vertices.size();
    for (int i=0; i<count-2; i++) {
        addTriangle(pivot, vertices[i+1], vertices[i+2]);
    }
}


/**
 * Cette méthode crée et rajoute un polygone au maillage. En fait, cela
 * revient à créer un éventail de triangles autour du premier point
 * NB: l'ordre de rotation des sommets est crucial pour le calcul des normales.
 * Il faut tourner dans le sens trigonométrique, comme dans OpenGL.
 * @param vertices : liste des sommets du polygone
 * @param normal : direction approximative de la normale
 */
void Mesh::addPolygon(std::vector<MeshVertex*> vertices, vec3 normal)
{
    // vecteurs temporaires
    vec3 AB = vec3::create();
    vec3 BC = vec3::create();
    vec3 CA = vec3::create();
    vec3 N = vec3::create();
    vec3 T = vec3::create();
    vec3 P = vec3::create();

    // tant qu'il reste au moins 3 sommets
    int vertice_count = vertices.size();
    while (vertice_count >= 3) {
        // chercher trois sommets successifs formant une arête convexe
        bool ok = false;
        for (int i=0; i<vertice_count-2; i++) {
            MeshVertex* A = vertices[(i+0)%vertice_count];
            MeshVertex* B = vertices[(i+1)%vertice_count];
            MeshVertex* C = vertices[(i+2)%vertice_count];
            vec3 cA = A->getCoord();
            vec3 cB = B->getCoord();
            vec3 cC = C->getCoord();
            // est-ce que les trois premiers forment une arête convexe ?
            vec3::subtract(AB, cB, cA);
            vec3::subtract(BC, cC, cB);
            vec3::cross(N, AB, BC);
            // si le produit scalaire entre N et la normale est positif, alors c'est convexe
            if (vec3::dot(N, normal) >= 0.0) {
                // vérifier qu'aucun des autres points du contour n'est dans ce triangle
                bool inside = false;
                for (int j=0; j<vertice_count; j++) {
                    if (i <= j && j <= i+2) continue;
                    vec3 cP = vertices[j]->getCoord();
                    // comparaison avec le côté AB
                    vec3::cross(T, AB, N);
                    vec3::subtract(P, cP, cA);
                    bool lefttoAB = (vec3::dot(P, T) >= 0.0);
                    // comparaison avec le côté BC
                    vec3::cross(T, BC, N);
                    vec3::subtract(P, cP, cB);
                    bool lefttoBC = (vec3::dot(P, T) >= 0.0);
                    // comparaison avec le côté CA
                    vec3::subtract(CA, cA, cC);
                    vec3::cross(T, CA, N);
                    vec3::subtract(P, cP, cC);
                    bool lefttoCA = (vec3::dot(P, T) >= 0.0);
                    // dans le futur triangle ?
                    if (lefttoAB && lefttoBC && lefttoCA) {
                        inside = true;
                        break;          // inutile de regarder les autres sommets
                    }
                }
                // si aucun point n'est dans le futur triangle, on peut le créer
                if (! inside) {
                    addTriangle(A, B, C);
                    // enlever B de la liste
                    list_remove_index(vertices, (i+1)%vertice_count);
                    vertice_count = vertice_count - 1;
                    // on a pu construire au moins un triangle
                    ok = true;
                }
            }
        }
        // si on n'a pas pu faire un seul triangle, il y a un souci
        if (!ok) throw std::runtime_error("impossible de construire le polygone");
    }
}


/**
 * Cette méthode supprime le triangle du maillage en mettant à jour toutes
 * les listes
 * @param triangle : celui qu'il faut supprimer
 */
void Mesh::delTriangle(MeshTriangle* triangle)
{
    delete triangle;
}


/**
 * Cette méthode supprime le sommet du maillage en mettant à jour toutes
 * les listes : tous les triangles qui le contiennent sont aussi supprimés.
 * @param vertex : celui qu'il faut supprimer
 */
void Mesh::delVertex(MeshVertex* vertex)
{
    delete vertex;
}


/**
 * Cette méthode recalcule les normales des triangles et sommets.
 * Les normales des triangles sont calculées d'après leurs côtés.
 * Les normales des sommets sont les moyennes des normales des triangles
 * auxquels ils appartiennent.
 */
void Mesh::computeNormals()
{
    // calculer les normales des triangles
    for (MeshTriangle* triangle: m_TriangleList) {
        triangle->computeNormal();
    }

    // calculer les normales des sommets
    int iv = 0;
    for (MeshVertex* vertex: m_VertexList) {
        // renuméroter le sommet (numéro dans les VBOs)
        vertex->setNumber(iv);
        iv++;
        // recalculer la normale de ce sommet
        vertex->computeNormal();
    }
}


/**
 * Cette méthode recalcule les tangentes des triangles et sommets.
 * Les tangentes des triangles sont calculées d'après leurs côtés et les coordonnées de texture.
 * Les tangentes des sommets sont les moyennes des tangentes des triangles
 * auxquels ils appartiennent.
 */
void Mesh::computeTangents()
{
    // calculer les tangentes des triangles
    for (MeshTriangle* triangle: m_TriangleList) {
        triangle->computeTangent();
    }

    // calculer les tangentes des sommets
    for (MeshVertex* vertex: m_VertexList) {
        // recalculer la tangente de ce sommet
        vertex->computeTangent();
    }
}
