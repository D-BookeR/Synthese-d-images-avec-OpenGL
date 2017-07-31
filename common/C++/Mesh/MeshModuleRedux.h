#ifndef MESH_MESHMODULEREDUX_H
#define MESH_MESHMODULEREDUX_H

/**
 * Ce module rajoute des fonctions pour réduire le nombre de sommets
 * d'un Maillage
 * NB: penser à appeler creerVBO() après cette opération
 *
 * Biblio :  Surface Simplification Using Quadric Error Metrics - Garland 1997
 * http://www1.cs.columbia.edu/~cs4162/html05s/garland97.pdf
 */

#include <MeshModuleUtils.h>
#include <MeshVertex.h>


class MeshModuleRedux: public MeshModuleUtils
{

public:

    /**
     * initialise le module sur le maillage fourni
     * @param mesh : maillage concerné
     */
    MeshModuleRedux(Mesh* mesh=nullptr);

    /**
     * Cette méthode supprime nombre sommets du maillage, en les
     * fusionnant avec leurs voisins.
     * @param count : nombre de sommets à enlever
     */
    void reduxCount(int count);

    /**
     * Cette méthode supprime des sommets du maillage, en les fusionnant avec leurs voisins.
     * @param maxcost : cout maximal des suppressions, ex: 0.0 pour les triangles coplanaires
     */
    void reduxCost(float maxcost);


protected:

    /**
     * calcule les coûts de fusion de tous les sommets, chacun vers l'un de ses voisins
     */
    void computeAllEdgesCollapseCosts();

    /**
     * cherche la meilleure cible pour une fusion du sommet vers l'un de ses voisins
     * @param vertex : sommet dont on cherche le voisin le moins cher
     */
    void computeEdgesCollapseCost(MeshVertex* vertex);

    /**
     * calcule le coût de la fusion entre le sommet U et le sommet V
     * @param U : sommet concerné pour une disparition au profit de V
     * @param V : sommet qui serait la cible de U s'il y avait fusion
     * @return le coût
     */
    float computeEdgeCollapseCost(MeshVertex* U, MeshVertex* V);

    /**
     * fusionne un sommet avec son meilleur voisin, sauf si son coût dépasse le paramètre fourni
     * @param maxcost : cout maximal autorisé pour la suppression
     * @return true si la fusion a pu se faire, false sinon
     */
    MeshVertex* findMinimumCostVertex(float maxcost);

    /**
     * cherche le sommet qui coûte le moins cher à fusionner avec l'un de
     * ses voisins
     * @param maxcost : cout maximal autorisé pour la suppression
     * @return le sommet le moins cher à supprimer
     */
    bool collapseMinimumCostEdge(float maxcost);

    /**
     * fusionne U sur V
     * @param U : vertex qui disparaît, car déplacé sur V
     * @param V : cible de U
     */
    void collapseVertex(MeshVertex* U, MeshVertex* V);

    /**
     * calcule la quadrique représentant les orientations et tailles des
     * triangles contenant ce sommet
     * @param vertex : sommet pour lequel il faut calculer la quadrique
     */
    void computeQuadric(MeshVertex* vertex);
};



class Quadric
{
private:

    float m_A[6];
    float m_B[3];
    float m_C;


public:
    /**
     * Constructeur de la classe Quadrique
     */
    Quadric();

    /**
     * met les coefficients de la quadrique à zéro
     */
    void zero();

    /**
     * Ajoute à this la quadrique due au plan N,V
     * @param N vecteur normal du plan
     * @param V point par lequel passe le plan
     */
    void addPlane(vec3 N, vec3 V);

    /**
     * Effectue this = this + Q
     * @param Q la quadrique à additionner à this
     */
    void addQuadric(Quadric* Q);

    /**
     * calcule le coût d'un vec3 P par rapport à la quadrique this
     * @param P : vec3 à évaluer
     * @return coût de P par rapport à this
     */
    float getCost(vec3 P);

};

#endif
