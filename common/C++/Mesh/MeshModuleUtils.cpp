/**
 * Ce module rajoute des fonctions diverses pour les maillages
 *
 */

#include <iostream>
#include <algorithm>
#include <math.h>

#include <Mesh.h>
#include <MeshModuleUtils.h>


/**
 * initialise le module sur le maillage fourni
 * @param mesh : maillage concerné
 */
MeshModuleUtils::MeshModuleUtils(Mesh* mesh): MeshModule(mesh)
{
}


/**
 * Cette méthode produit le nom du milieu entre p1 et p2, c'est la concaténation
 * du nom de p1 et de p2 mais dans l'ordre lexicographique afin que
 * nomMilieu(p1, p2) == nomMilieu(p2, p1) quels que soient p1 et p2
 * @param p1 : vertex
 * @param p2 : vertex
 * @return "m" + nom du plus petit sommet + nom du plus grand
 */
std::string MeshModuleUtils::getMidName(MeshVertex* p1, MeshVertex* p2)
{
    // définir le nom du milieu à partir des noms de p1 et p2
    if (p1->getName() < p2->getName()) {
        // p1 a le plus petit nom dans l'ordre lexicographique
        return "m"+p1->getName()+p2->getName();
    } else {
        // p2 a le plus petit nom dans l'ordre lexicographique
        return "m"+p2->getName()+p1->getName();
    }
}


/**
 * calcule la moyenne des normales des triangles fournis
 * @param triangles : liste des triangles à considérer
 * @return moyenne des normales
 */
vec3 MeshModuleUtils::averageNormals(std::vector<MeshTriangle*>& triangles)
{
    // accumuler les normales puis normaliser
    vec3 sum = vec3::create();
    for (MeshTriangle* triangle: triangles) {
        // normale du triangle courant
        vec3 n = vec3::clone(triangle->getNormal());
        // prendre en compte la surface du triangle
        vec3::scale(n, n, triangle->getSurface());
        // cumuler les normales pondérées
        vec3::add(sum, sum, n);
    }
    // normaliser le résultat
    vec3::normalize(sum, sum);
    return sum;
}


/**
 * calcule la moyenne des tangentes des triangles fournis
 * @param triangles : liste des triangles à considérer
 * @return moyenne des tangentes
 */
vec3 MeshModuleUtils::averageTangents(std::vector<MeshTriangle*>& triangles)
{
    // accumuler les tangentes puis normaliser
    vec3 sum = vec3::create();
    for (MeshTriangle* triangle: triangles) {
        // tangente du triangle courant
        vec3 t = vec3::clone(triangle->getTangent());
        // prendre en compte la surface du triangle
        vec3::scale(t, t, triangle->getSurface());
        // cumuler les tangentes pondérées
        vec3::add(sum, sum, t);
    }
    // normaliser le résultat
    vec3::normalize(sum, sum);
    return sum;
}


/**
 * Cette fonction retourne la liste des sommets qui sont contenus dans les triangles fournis.
 * @param triangles : liste de triangles
 * @return tableau des sommets
 */
std::set<MeshVertex*> MeshModuleUtils::getVerticesFromTriangles(std::vector<MeshTriangle*>& triangles)
{
    std::set<MeshVertex*> result;
    for (MeshTriangle* triangle: triangles) {
        for (MeshVertex* vertex: triangle->getVertexList()) {
            result.insert(vertex);
        }
    }
    return result;
}


/**
 * Détermine les (deux) triangles qui sont à gauche de la demi-arête mais sans sortir
 * de la zone délimitée par le contour. Rajoute aussi tous les triangles qui sont en contact
 * avec les triangles déterminés au premier coup (appels récursifs).
 * En fait, c'est un algo de remplissage de tache appliqué à un maillage de triangles.
 * @param trianglelist : liste à laquelle sont rajoutés les triangles
 * @param halfedge : demi-arête de départ
 * @param border : liste de demi-arêtes qui délimitent les triangles intéressants
 */
void MeshModuleUtils::appendNeighborTriangles(std::vector<MeshTriangle*>& trianglelist, MeshHalfEdge* halfedge, std::vector<MeshHalfEdge*>& border)
{
    // triangle à gauche de cette demi-arête
    MeshTriangle* triangle = halfedge->getTriangle();
    if (triangle == nullptr) return;

    // ajouter ce triangle à la liste sauf s'il y est déjà
    if (indexOf(trianglelist, triangle) < 0) {
        trianglelist.push_back(triangle);

        // demi-arête suivante et son opposée
        halfedge = halfedge->getNext();
        MeshHalfEdge* opposite = halfedge->getOpposite();

        // si elle n'est pas dans le contour, on va voir à sa droite
        if (indexOf(border, halfedge) < 0 && opposite != nullptr) {
            appendNeighborTriangles(trianglelist, opposite, border);
        }

        // demi-arête suivante et son opposée
        halfedge = halfedge->getNext();
        opposite = halfedge->getOpposite();

        // si elle n'est pas dans le contour, on va voir à sa droite
        if (indexOf(border, halfedge) < 0 && opposite != nullptr) {
            appendNeighborTriangles(trianglelist, opposite, border);
        }
    }
}


/**
 * retourne la liste des triangles qui sont délimités par une succession de demi-arêtes
 * Le contour doit être refermé sur lui-même ou constituer une frontière d'un bord
 * à l'autre du maillage, sinon tous les triangles seront sélectionnés.
 * Attention à l'ordre de spécification des sommets du contour : ce sont les triangles
 * à gauche qui sont spécifiés (tourner dans le sens trigo pour entourer une zone).
 * @param border : liste des demi-arêtes. Les triangles sont à gauche d'elles
 * @return liste des triangles compris dans le contour
 */
std::vector<MeshTriangle*> MeshModuleUtils::getTrianglesInsideBorder(std::vector<MeshHalfEdge*>& border)
{
    // résultat, initialisé à vide
    std::vector<MeshTriangle*> trianglelist;

    // si la liste est trop courte, sortir sans rien faire
    if (border.size() < 3) return trianglelist;

    // partir de la première arête
    MeshHalfEdge* halfedge = border[0];

    // appels récursifs pour rajouter les triangles qui sont en contact avec cette arête
    appendNeighborTriangles(trianglelist, halfedge, border);

    return trianglelist;
}


/**
 * retourne la liste des demi-arêtes correspondant à une liste de vertex formant un contour fermé
 * @param border : liste de sommets classés dans l'ordre, le dernier doit être lié au premier
 * @return liste des demi-arêtes
 * @throws Exception
 */
std::vector<MeshHalfEdge*> MeshModuleUtils::getHalfEdgesAlongBorder(std::vector<MeshVertex*>& border)
{
    std::vector<MeshHalfEdge*> halfedgeslist;

    MeshVertex* prec = nullptr;
    for (MeshVertex* v: border) {
        if (prec != nullptr) {
            MeshHalfEdge* halfedge = prec->getHalfEdgeTo(v);
            if (halfedge == nullptr) throw std::runtime_error("Vertex "+prec->toString()+" has no half-edge to "+v->toString());
            halfedgeslist.push_back(halfedge);
        }
        prec = v;
    }
    // dernier sommet à lier avec le premier
    if (prec != nullptr) {
        MeshVertex* v = border[0];
        MeshHalfEdge* halfedge = prec->getHalfEdgeTo(v);
        if (halfedge == nullptr) throw std::runtime_error("Vertex "+prec->toString()+" has no half-edge to "+v->toString());
        halfedgeslist.push_back(halfedge);
    }
    return halfedgeslist;
}


/**
 * Cette méthode calcule la valeur minimum des coordonnées
 * @return vec3 contenant min(x),min(y),min(z) des coordonnées des sommets
 */
vec3 MeshModuleUtils::computeMinAABB()
{
    // déterminer les bornes min du maillage
    vec3 vmin = vec3::clone(m_Mesh->getVertexList()[0]->getCoord());
    for (MeshVertex* vertex: m_Mesh->getVertexList()) {
        vec3::min(vmin, vmin, vertex->getCoord());
    }
    return vmin;
}


/**
 * Cette méthode calcule la valeur maximale des coordonnées
 * @return vec3 contenant max(x),max(y),max(z) des coordonnées des sommets
 */
vec3 MeshModuleUtils::computeMaxAABB()
{
    // déterminer les bornes max du maillage
    vec3 vmax = vec3::clone(m_Mesh->getVertexList()[0]->getCoord());
    for (MeshVertex* vertex: m_Mesh->getVertexList()) {
        vec3::max(vmax, vmax, vertex->getCoord());
    }
    return vmax;
}


/**
 * Cette méthode construit la boîte englobante du maillage this
 * @return maillage en forme de parallépipède rectangle
 */
Mesh* MeshModuleUtils::createAABB()
{
    // déterminer les bornes du maillage
    vec3 vmin = vec3::clone(m_Mesh->getVertexList()[0]->getCoord());
    vec3 vmax = vec3::clone(m_Mesh->getVertexList()[0]->getCoord());
    for (MeshVertex* vertex: m_Mesh->getVertexList()) {
        vec3::min(vmin, vmin, vertex->getCoord());
        vec3::max(vmax, vmax, vertex->getCoord());
    }

    // maillage en retour
    Mesh* aabb = new Mesh("AABB");

    // sommets de ce maillage
    MeshVertex* xyz = aabb->addVertex("xyz");
    MeshVertex* Xyz = aabb->addVertex("Xyz");
    MeshVertex* xYz = aabb->addVertex("xYz");
    MeshVertex* XYz = aabb->addVertex("XYz");
    MeshVertex* xyZ = aabb->addVertex("xyZ");
    MeshVertex* XyZ = aabb->addVertex("XyZ");
    MeshVertex* xYZ = aabb->addVertex("xYZ");
    MeshVertex* XYZ = aabb->addVertex("XYZ");

    // coordonnées des coins
    xyz->setCoord(vec3::fromValues(vmin[0], vmin[1], vmin[2]));
    Xyz->setCoord(vec3::fromValues(vmax[0], vmin[1], vmin[2]));
    xYz->setCoord(vec3::fromValues(vmin[0], vmax[1], vmin[2]));
    XYz->setCoord(vec3::fromValues(vmax[0], vmax[1], vmin[2]));
    xyZ->setCoord(vec3::fromValues(vmin[0], vmin[1], vmax[2]));
    XyZ->setCoord(vec3::fromValues(vmax[0], vmin[1], vmax[2]));
    xYZ->setCoord(vec3::fromValues(vmin[0], vmax[1], vmax[2]));
    XYZ->setCoord(vec3::fromValues(vmax[0], vmax[1], vmax[2]));

    // construire les quads de la boîte
    aabb->addQuad(XyZ,Xyz,XYz,XYZ);
    aabb->addQuad(Xyz,xyz,xYz,XYz);
    aabb->addQuad(xyz,xyZ,xYZ,xYz);
    aabb->addQuad(xyZ,XyZ,XYZ,xYZ);
    aabb->addQuad(XYZ,XYz,xYz,xYZ);
    aabb->addQuad(Xyz,XyZ,xyZ,xyz);

    // finaliser cette boîte
    aabb->computeNormals();

    return aabb;
}


/**
 * Cette méthode vérifie la cohérence des informations du maillage.
 * La référence est la liste des sommets et la liste des triangles
 * NB : cette fonction ne teste pas toutes les erreurs possibles, mais
 * seulement quelques unes des plus graves.
 * @param repair : true s'il faut tenter de réparer les erreurs, ex: vertex non utilisé
 * @return true si le maillage semble correct, false s'il y a une grave erreur
 */
bool MeshModuleUtils::checkMesh(bool repair)
{
    // vérifier les triangles
    int triangle_count = m_Mesh->getTriangleCount();
    for (int it=0; it<triangle_count; it++) {
        MeshTriangle* triangle = m_Mesh->getTriangleList()[it];
        // le triangle doit être du maillage
        if (triangle->getMesh() != m_Mesh) {
            std::cerr << "Triangle #"<<it<<" does not belong to the mesh" << std::endl;
            return false;
        }
        // la demi-arête doit être dans le maillage
        if (indexOf(m_Mesh->getHalfEdgeList(), triangle->getHalfEdge()) < 0) {
            std::cerr << "Half-edge of Triangle #"<<it<<" does not belong to the mesh" << std::endl;
            return false;
        }
        // sa demi-arête doit le désigner
        if (triangle->getHalfEdge()->getTriangle() != triangle) {
            std::cerr << "Half-edge of Triangle #"<<it<<" does not point to it" << std::endl;
            return false;
        }
        // vérifier que les points sont tous différents
        if (triangle->getVertex0() == triangle->getVertex1() ||
            triangle->getVertex0() == triangle->getVertex2() ||
            triangle->getVertex1() == triangle->getVertex2()) {
            std::cerr << "Vertices of triangle #"<<it<<" are not all distincts" << std::endl;
            return false;
        }
        // vérifier les points
        for (int i=0; i<3; i++) {
            MeshVertex* vertex = triangle->getVertex(i);
            // ce sommet doit être dans la liste des sommets du maillage
            if (indexOf(m_Mesh->getVertexList(), vertex) < 0) {
                std::cerr << "Vertex #"<<i<<"="<<vertex->getName()<<" of triangle #"<<it<<" does not belong to the mesh" << std::endl;
                return false;
            }
        }
    }

    // liste des sommets à supprimer
    std::vector<MeshVertex*> suppr;

    // vérifier les sommets
    int vertex_count = m_Mesh->getVertexCount();
    for (int iv=0; iv<vertex_count; iv++) {
        MeshVertex* vertex = m_Mesh->getVertexList()[iv];
        // le sommet doit être du maillage
        if (vertex->getMesh() != m_Mesh) {
            std::cerr << "Vertex #"<<iv<<"="<<vertex->getName()<<" does not belong to the mesh" << std::endl;
            return false;
        }
        // la demi-arête doit être nullptr ou dans le maillage
        if (vertex->getHalfEdge() != nullptr && indexOf(m_Mesh->getHalfEdgeList(), vertex->getHalfEdge()) < 0) {
            std::cerr << "Half-edge of vertex #"<<iv<<"="<<vertex->getName()<<" does not belong to the mesh" << std::endl;
            return false;
        }
        // sa demi-arête doit le désigner
        if (vertex->getHalfEdge() != nullptr && vertex->getHalfEdge()->getOrigin() != vertex) {
            std::cerr << "Half-edge of vertex #"<<iv<<"="<<vertex->getName()<<" does not point to it" << std::endl;
            return false;
        }
        // ce sommet doit être dans l'un des triangles du maillage
        bool present = false;
        for (int it=0; it<triangle_count && !present; it++) {
            MeshTriangle* triangle = m_Mesh->getTriangleList()[it];
            for (int i=0; i<3; i++) {
                if (triangle->getVertex(i) == vertex) {
                    present = true;
                    break;
                }
            }
        }
        if (! present) {
            if (repair) {
                suppr.push_back(vertex);
            } else {
                std::cerr << "Vertex "<<vertex->toString()<<" does not belong to any triangle of mesh" << std::endl;
                //return false;
            }
        }
        // vérifier le chaînage m_Sibling
        MeshHalfEdge* halfedge = vertex->getHalfEdge();
        while (halfedge != nullptr) {
            // l'origine de cette demi-arête doit être le sommet
            if (halfedge->getOrigin() != vertex) {
                std::cerr << "Half-edge "<<halfedge->toString()<<" is chained to vertex "<<vertex->toString()<<" but does not point to it" << std::endl;
                return false;
            }
            // il ne faut pas qu'il y ait un cycle dans le chaînage
            MeshHalfEdge* other = vertex->getHalfEdge();
            while (other != nullptr && other != halfedge) {
                if (halfedge->getSibling() == other) {
                    std::cerr << "Half-edge "<<halfedge->toString()<<" belongs to a cycle in siblings of vertex "<<vertex->toString() << std::endl;
                }
                other = other->getSibling();
            }
            halfedge = halfedge->getSibling();
        }
    }

    // supprimer les sommets qui ne sont dans aucun triangle (si l'option repair a été activée)
    for (MeshVertex* vertex: suppr) {
        delete vertex;
    }

    // vérifier les demi-arêtes
    int halfedge_count = m_Mesh->getHalfEdgeList().size();
    for (int id=0; id<halfedge_count; id++) {
        MeshHalfEdge* halfedge = m_Mesh->getHalfEdgeList()[id];
        // la demi-arête doit être du maillage
        if (halfedge->getMesh() != m_Mesh) {
            std::cerr << "Half-Edge #"<<id<<" does not belong to the mesh" << std::endl;
            return false;
        }
        // son origine doit être un des sommets du maillage
        if (indexOf(m_Mesh->getVertexList(), halfedge->getOrigin()) < 0) {
            std::cerr << "Origin of Opposite Half-edge of #"<<id<<" does not belong to the mesh" << std::endl;
            return false;
        }
        // vérifier l'opposée de la demi-arête
        if (halfedge->getOpposite() != nullptr) {
            // son opposée doit être dans le maillage
            if (indexOf(m_Mesh->getHalfEdgeList(), halfedge->getOpposite()) < 0) {
                std::cerr << "Opposite Half-edge of #"<<id<<" does not belong to the mesh" << std::endl;
                return false;
            }
            // l'opposée de son opposée doit être elle-même
            if (halfedge->getOpposite()->getOpposite() != halfedge) {
                std::cerr << "Opposite of opposite Half-edge of #"<<id<<" is not itself" << std::endl;
                return false;
            }
        }
        // vérifier le chaînage m_Next
        if (halfedge->getNext() == nullptr) {
            std::cerr << "Next half-edge of #"<<id<<" is null" << std::endl;
            return false;
        }
        if (indexOf(m_Mesh->getHalfEdgeList(), halfedge->getNext()) < 0) {
            std::cerr << "Next half-edge of #"<<id<<" does not belong to the mesh" << std::endl;
            return false;
        }
        // vérifier le chaînage m_Next->m_Next
        if (halfedge->getNext()->getNext() == nullptr) {
            std::cerr << "Second next of half-edge #"<<id<<" is null" << std::endl;
            return false;
        }
        if (indexOf(m_Mesh->getHalfEdgeList(), halfedge->getNext()->getNext()) < 0) {
            std::cerr << "Second next of half-edge #"<<id<<" does not belong to the mesh" << std::endl;
            return false;
        }
        // vérifier le chaînage m_Next->m_Next->m_Next
        if (halfedge->getNext()->getNext()->getNext() == nullptr) {
            std::cerr << "Third next of half-edge #"<<id<<" is null" << std::endl;
            return false;
        }
        if (halfedge->getNext()->getNext()->getNext() != halfedge) {
            std::cerr << "Third next of half-edge #"<<id<<" n'est pas elle-même" << std::endl;
            return false;
        }
    }

    // vérifier les arêtes
    int edge_count = m_Mesh->getEdgeList().size();
    for (int ia=0; ia<edge_count; ia++) {
        MeshEdge* edge = m_Mesh->getEdgeList()[ia];
        // l'arête doit être du maillage
        if (edge->getMesh() != m_Mesh) {
            std::cerr << "Edge #"<<ia<<" does not belong to the mesh" << std::endl;
            return false;
        }
    }

    // bilan positif
    std::cerr << "Mesh checked OK" << std::endl;

    // succès, mais attention, ça ne valide que ce qui a été testé
    return true;
}


/**
 * Cette méthode affiche la totalité des structures de données du maillage
 */
void MeshModuleUtils::dump()
{
    // sommets
    for (MeshVertex* vertex: m_Mesh->getVertexList())
        std::cout << vertex->toFullString() << std::endl;
    // arêtes
    //for (MeshEdge* edge: m_Mesh->getEdgeList())
    //    std::cout << edge->toFullString()) << std::endl;
    // triangles
    for (MeshTriangle* triangle: m_Mesh->getTriangleList())
        std::cout << triangle->toFullString() << std::endl;
    // demi-arêtes
    for (MeshHalfEdge* halfedge: m_Mesh->getHalfEdgeList())
        std::cout << halfedge->toFullString() << std::endl;
    std::cout << std::endl;
}
