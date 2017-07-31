/**
 * Ce module rajoute des fonctions pour réduire le nombre de sommets d'un maillage
 * NB: penser à appeler creerVBO() après cette opération
 *
 * Biblio :  Surface Simplification Using Quadric Error Metrics - Garland 1997
 * http://www1.cs.columbia.edu/~cs4162/html05s/garland97.pdf
 */

#include <math.h>

#include <algorithm>
#include <list>
#include <set>
#include <iostream>

#include <gl-matrix.h>

#include <MeshModuleRedux.h>


/**
 * initialise le module sur le maillage fourni
 * @param mesh : maillage concerné
 */
MeshModuleRedux::MeshModuleRedux(Mesh* mesh): MeshModuleUtils(mesh)
{
}


/**
 * Cette méthode supprime nombre sommets du maillage, en les
 * fusionnant avec leurs voisins.
 * @param count : nombre de sommets à enlever
 */
void MeshModuleRedux::reduxCount(int count)
{
    // calculer les coûts de fusion de toutes les arêtes
    computeAllEdgesCollapseCosts();

    // réduire autant de fois que demandé
    for (int i=0; i<count; i++) {
        if (!collapseMinimumCostEdge(1e40)) break;
    }
}


/**
 * Cette méthode supprime des sommets du maillage, en les fusionnant avec leurs voisins.
 * @param maxcost : cout maximal des suppressions, ex: 0.0 pour les triangles coplanaires
 */
void MeshModuleRedux::reduxCost(float maxcost)
{
    // calculer les coûts de fusion de toutes les arêtes
    computeAllEdgesCollapseCosts();

    // réduire tant qu'on ne dépasse pas le cout prévu
    while (collapseMinimumCostEdge(maxcost)) {
        // ne rien faire
    }
}


/**
 * calcule les coûts de fusion de tous les sommets, chacun vers l'un de ses voisins
 */
void MeshModuleRedux::computeAllEdgesCollapseCosts()
{
    // calculer les normales
    m_Mesh->computeNormals();

    // calculer les quadriques
    for (MeshVertex* vertex: m_Mesh->getVertexList()) {
        computeQuadric(vertex);
    }

    // calculer les coûts de fusion
    for (MeshVertex* vertex: m_Mesh->getVertexList()) {
        computeEdgesCollapseCost(vertex);
    }
}


/**
 * cherche la meilleure cible pour une fusion du sommet vers l'un de ses voisins
 * @param vertex : sommet dont on cherche le voisin le moins cher
 */
void MeshModuleRedux::computeEdgesCollapseCost(MeshVertex* vertex)
{
    // établir la liste des candidats pour une fusion du sommet vers eux : ses voisins
    std::set<MeshVertex*> candidates = vertex->getNeighborVertices();

    // chercher le voisin le moins cher pour fusionner le sommet dessus
    float min_cost = 1e38;
    MeshVertex* min_target = nullptr;
    for (MeshVertex* candidate: candidates) {
        // calculer le coût de fusion du sommet vers ce candidat
        float cost = computeEdgeCollapseCost(vertex, candidate);
        // on garde le candidat ayant le plus faible coût
        if (cost < min_cost) {
            min_target = candidate;
            min_cost = cost;
            if (min_cost <= 0.0) break;
        }
    }
    vertex->setCollapseTarget(min_target);
    vertex->setCollapseCost(min_cost);
}


/**
 * calcule le coût de la fusion entre le sommet U et le sommet V
 * @param U : sommet concerné pour une disparition au profit de V
 * @param V : sommet qui serait la cible de U s'il y avait fusion
 * @return le coût
 */
float MeshModuleRedux::computeEdgeCollapseCost(MeshVertex* U, MeshVertex* V)
{
    Quadric Q;
    Q.addQuadric(U->getQuadric());
    Q.addQuadric(V->getQuadric());
    return Q.getCost(V->getCoord());
}


/**
 * fusionne un sommet avec son meilleur voisin, sauf si son coût dépasse le paramètre fourni
 * @param maxcost : cout maximal autorisé pour la suppression
 * @return true si la fusion a pu se faire, false sinon
 */
bool MeshModuleRedux::collapseMinimumCostEdge(float maxcost)
{
    MeshVertex* min_vertex = findMinimumCostVertex(maxcost);
    if (min_vertex == nullptr) return false;
    collapseVertex(min_vertex, min_vertex->getCollapseTarget());
    return true;
}


/**
 * cherche le sommet qui coûte le moins cher à fusionner avec l'un de
 * ses voisins
 * @param maxcost : cout maximal autorisé pour la suppression
 * @return le sommet le moins cher à supprimer
 */
MeshVertex* MeshModuleRedux::findMinimumCostVertex(float maxcost)
{
    MeshVertex* min_vertex = nullptr;
    float min_cost = maxcost;
    for (MeshVertex* vertex: m_Mesh->getVertexList()) {
        if (vertex->getCollapseCost() < min_cost) {
            min_vertex = vertex;
            min_cost = min_vertex->getCollapseCost();
        }
    }
    return min_vertex;
}


/**
 * fusionne U sur V
 * @param U : vertex qui disparaît, car déplacé sur V
 * @param V : cible de U
 */
void MeshModuleRedux::collapseVertex(MeshVertex* U, MeshVertex* V)
{
    // chercher la demi-arête entre V et U, attention à la direction
    MeshHalfEdge* halfedge = V->getHalfEdgeTo(U);
    if (halfedge == nullptr) return;

    // écraser cette demi-arête
    MeshHalfEdge::collapse(halfedge);

    // comme U a été fusionné, il n'est plus dans aucun triangle
    delete U;

    // recalculer les normales des anciens et nouveaux triangles autour de V
    for (MeshTriangle* triangle: V->getTrianglesAround()) {
        triangle->computeNormal();
    }

    // sommets voisins anciens et nouveaux de V
    std::set<MeshVertex*> neighbors = V->getNeighborVertices();

    // recalculer les normales de V et ses voisins
    V->computeNormal();
    for (MeshVertex* neighbor: neighbors) {
        neighbor->computeNormal();
    }

    // recalculer les quadriques de V et ses voisins
    computeQuadric(V);
    for (MeshVertex* neighbor: neighbors) {
        computeQuadric(neighbor);
    }

    // recalculer les coûts de fusion de V et ses voisins
    computeEdgesCollapseCost(V);
    for (MeshVertex* neighbor: neighbors) {
        computeEdgesCollapseCost(neighbor);
    }
}


/**
 * calcule la quadrique représentant les orientations et tailles des
 * triangles contenant ce sommet
 * @param vertex : sommet pour lequel il faut calculer la quadrique
 */
void MeshModuleRedux::computeQuadric(MeshVertex* vertex)
{
    // initialiser la quadrique à la matrice nulle
    if (vertex->getQuadric() != nullptr) {
        vertex->getQuadric()->zero();
    } else {
        vertex->setQuadric(new Quadric());
    }

    // ajouter la quadrique due à chaque triangle
    for (MeshTriangle* triangle: vertex->getTrianglesAround()) {
        // coefficient d'importance de cette quadrique : inverse de la surface du polygone
        float weight = 1.0/sqrt(triangle->getSurface());
        // calculer la quadrique correspondant à ce triangle (il est préférable de prémultiplier la normale par le coef d'importance)
        vec3 N = vec3::create();
        vec3::scale(N, triangle->getNormal(), weight);
        // accumuler les quadriques de chaque triangle
        vertex->getQuadric()->addPlane(N, vertex->getCoord());

        // NB: j'ai enlevé tout le code qui gère les frontières (si une arête = bord, alors rajouter un "mur" orthogonal et très grand)
    }
}


// Définition de la classe Quadric


/**
 * Constructeur de la classe Quadrique
 */
Quadric::Quadric()
{
    zero();
}


/**
 * met les coefficients de la quadrique à zéro
 */
void Quadric::zero()
{
    for (int i=0; i<6; i++) m_A[i] = 0.0;
    for (int i=0; i<3; i++) m_B[i] = 0.0;
    m_C = 0.0;
}


/**
 * Ajoute à this la quadrique due au plan N,V
 * @param N vecteur normal du plan
 * @param V point par lequel passe le plan
 */
void Quadric::addPlane(vec3 N, vec3 V)
{
    m_A[0] += N[0] * N[0];
    m_A[1] += N[0] * N[1];
    m_A[2] += N[0] * N[2];
    m_A[3] += N[1] * N[1];
    m_A[4] += N[1] * N[2];
    m_A[5] += N[2] * N[2];

    float d = -vec3::dot(V, N);

    m_B[0] += d * N[0];
    m_B[1] += d * N[1];
    m_B[2] += d * N[2];

    m_C += d * d;
}


/**
 * Effectue this = this + Q
 * @param Q la quadrique à additionner à this
 */
void Quadric::addQuadric(Quadric* Q)
{
    m_A[0] += Q->m_A[0];
    m_A[1] += Q->m_A[1];
    m_A[2] += Q->m_A[2];
    m_A[3] += Q->m_A[3];
    m_A[4] += Q->m_A[4];
    m_A[5] += Q->m_A[5];

    m_B[0] += Q->m_B[0];
    m_B[1] += Q->m_B[1];
    m_B[2] += Q->m_B[2];

    m_C += Q->m_C;
}


/**
 * calcule le coût d'un vec3 P par rapport à la quadrique this
 * @param P : vec3 à évaluer
 * @return coût de P par rapport à this
 */
float Quadric::getCost(vec3 P)
{
    float x = m_A[0] * P[0] + m_A[1] * P[1] + m_A[2] * P[2];
    float y = m_A[1] * P[0] + m_A[3] * P[1] + m_A[4] * P[2];
    float z = m_A[2] * P[0] + m_A[4] * P[1] + m_A[5] * P[2];
    float VtAV = x * P[0] + y * P[1] + z * P[2];

    float BtV = m_B[0] * P[0] + m_B[1] * P[1] + m_B[2] * P[2];

    return VtAV + 2.0*BtV + m_C;
}
