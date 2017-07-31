// classes et modules nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshHalfEdge.js");
Requires("libs/Mesh/MeshEdge.js");
Requires("libs/Mesh/MeshVertex.js");
Requires("libs/Mesh/MeshTriangle.js");
Requires("libs/Mesh/MeshModuleUtils.js");


/**
 * Ce module rajoute des fonctions pour réduire le nombre de sommets d'un maillage
 * NB: penser à appeler creerVBO() après cette opération
 *
 * Biblio :  Surface Simplification Using Quadric Error Metrics - Garland 1997
 * http://www1.cs.columbia.edu/~cs4162/html05s/garland97.pdf
 */
class MeshModuleRedux extends MeshModuleUtils
{
    /**
     * initialise le module sur le maillage fourni
     * @param mesh : maillage concerné
     */
    constructor(mesh)
    {
        super(mesh);
    }


    /** destructeur */
    destroy()
    {
        super.destroy();
    }


    /**
     * Cette méthode supprime nombre sommets du maillage, en les
     * fusionnant avec leurs voisins.
     * @param count : nombre de sommets à enlever
     */
    reduxCount(count)
    {
        // calculer les coûts de fusion de toutes les arêtes
        this.computeAllEdgesCollapseCosts();

        // réduire autant de fois que demandé
        for (let i=0; i<count; i++) {
            if (!this.collapseMinimumCostEdge(1e40)) break;
        }
    }


    /**
     * Cette méthode supprime des sommets du maillage, en les fusionnant avec leurs voisins.
     * @param maxcost : cout maximal des suppressions, ex: 0.0 pour les triangles coplanaires
     */
    reduxCost(maxcost)
    {
        // calculer les coûts de fusion de toutes les arêtes
        this.computeAllEdgesCollapseCosts();

        // réduire tant qu'on ne dépasse pas le cout prévu
        while (this.collapseMinimumCostEdge(maxcost)) {
            // ne rien faire
        }
    }


    /**
     * calcule les coûts de fusion de tous les sommets, chacun vers l'un de ses voisins
     */
    computeAllEdgesCollapseCosts()
    {
        // calculer les normales
        this.m_Mesh.computeNormals();

        // calculer les quadriques
        for (let vertex of this.m_Mesh.getVertexList()) {
            this.calculateQuadric(vertex);
        }

        // calculer les coûts de fusion
        for (let vertex of this.m_Mesh.getVertexList()) {
            this.computeEdgesCollapseCost(vertex);
        }
    }


    /**
     * cherche la meilleure cible pour une fusion du sommet vers l'un de ses voisins
     * @param vertex : sommet dont on cherche le voisin le moins cher
     */
    computeEdgesCollapseCost(vertex)
    {
        // établir la liste des candidats pour une fusion du sommet vers eux : ses voisins
        let candidates = vertex.getNeighborVertices();

        // chercher le voisin le moins cher pour fusionner le sommet dessus
        let min_cost = 1e38;
        let min_target = null;
        for (let candidate of candidates) {
            // calculer le coût de fusion du sommet vers ce candidat
            let cost = this.computeEdgeCollapseCost(vertex, candidate);
            // on garde le candidat ayant le plus faible coût
            if (cost < min_cost) {
                min_target = candidate;
                min_cost = cost;
                if (min_cost <= 0.0) break;
            }
        }
        vertex.setCollapseTarget(min_target);
        vertex.setCollapseCost(min_cost);
    }


    /**
     * calcule le coût de la fusion entre le sommet U et le sommet V
     * @param U : sommet concerné pour une disparition au profit de V
     * @param V : sommet qui serait la cible de U s'il y avait fusion
     * @return le coût
     */
    computeEdgeCollapseCost(U, V)
    {
        let Q = new Quadric();
        Q.addQuadric(U.getQuadric());
        Q.addQuadric(V.getQuadric());
        return Q.getCost(V.getCoord());
    }


    /**
     * fusionne un sommet avec son meilleur voisin, sauf si son coût dépasse le paramètre fourni
     * @param maxcost : cout maximal autorisé pour la suppression
     * @return true si la fusion a pu se faire, false sinon
     */
    collapseMinimumCostEdge(maxcost)
    {
        let min_vertex = this.findMinimumCostVertex(maxcost);
        if (min_vertex == null) return false;
        this.collapseVertex(min_vertex, min_vertex.getCollapseTarget());
        return true;
    }


    /**
     * cherche le sommet qui coûte le moins cher à fusionner avec l'un de
     * ses voisins
     * @param maxcost : cout maximal autorisé pour la suppression
     * @return le sommet le moins cher à supprimer
     */
    findMinimumCostVertex(maxcost)
    {
        let min_vertex = null;
        let min_cost = maxcost;
        for (let vertex of this.m_Mesh.getVertexList()) {
            if (vertex.getCollapseCost() <= min_cost) {
                min_vertex = vertex;
                min_cost = min_vertex.getCollapseCost();
            }
        }
        return min_vertex;
    }


    /**
     * fusionne U sur V
     * @param U : vertex qui disparaît, car déplacé sur V
     * @param V : cible de U
     */
    collapseVertex(U, V)
    {
        // chercher la demi-arête entre V et U, attention à la direction
        let halfedge = V.getHalfEdgeTo(U);
        if (halfedge == null) return;

        // écraser cette demi-arête
        MeshHalfEdge.collapse(halfedge);

        // comme U a été fusionné, il n'est plus dans aucun triangle
        U.destroy();

        // recalculer les normales des anciens et nouveaux triangles autour de V
        for (let triangle of V.getTrianglesAround()) {
            triangle.computeNormal();
        }

        // sommets voisins anciens et nouveaux de V
        let neighbors = V.getNeighborVertices();

        // recalculer les normales de V et ses voisins
        V.computeNormal();
        for (let neighbor of neighbors) {
            neighbor.computeNormal();
        }

        // recalculer les quadriques de V et ses voisins
        this.calculateQuadric(V);
        for (let neighbor of neighbors) {
            this.calculateQuadric(neighbor);
        }

        // recalculer les coûts de fusion de V et ses voisins
        this.computeEdgesCollapseCost(V);
        for (let neighbor of neighbors) {
            this.computeEdgesCollapseCost(neighbor);
        }
    }


    /**
     * calcule la quadrique représentant les orientations et tailles des
     * triangles contenant ce sommet
     * @param vertex : sommet pour lequel il faut calculer la quadrique
     */
    calculateQuadric(vertex)
    {
        // initialiser la quadrique à la matrice nulle
        if (vertex.m_Quadric != null) {
            vertex.m_Quadric.zero();
        } else {
        vertex.m_Quadric = new Quadric();
        }

        // ajouter la quadrique due à chaque triangle
        for (let triangle of vertex.getTrianglesAround()) {
            // coefficient d'importance de cette quadrique : inverse de la surface du polygone
            let weight = 1.0/Math.sqrt(triangle.getSurface());
            // calculer la quadrique correspondant à ce triangle (il est préférable de prémultiplier la normale par le coef d'importance)
            let N = vec3.create();
            vec3.scale(N, triangle.getNormal(), weight);
            // accumuler les quadriques de chaque triangle
            vertex.m_Quadric.addPlane(N, vertex.getCoord());

            // NB: j'ai enlevé tout le code qui gère les frontières (si une arête = bord, alors rajouter un "mur" orthogonal et très grand)
        }
    }
}


// Définition de la classe Quadric
class Quadric
{
    /**
     * Constructeur de la classe Quadrique
     */
    constructor()
    {
        this.zero();
    }


    /** destructeur */
    destroy()
    {
    }


    /**
     * met les coefficients de la quadrique à zéro
     */
    zero()
    {
        this.m_A = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0];
        this.m_B = [0.0, 0.0, 0.0];
        this.m_C = 0.0;
    }


    /**
     * Ajoute à this la quadrique due au plan N,V
     * @param N vecteur normal du plan
     * @param V point par lequel passe le plan
     */
    addPlane(N, V)
    {
        this.m_A[0] += N[0] * N[0];
        this.m_A[1] += N[0] * N[1];
        this.m_A[2] += N[0] * N[2];
        this.m_A[3] += N[1] * N[1];
        this.m_A[4] += N[1] * N[2];
        this.m_A[5] += N[2] * N[2];

        let d = -vec3.dot(V, N);

        this.m_B[0] += d * N[0];
        this.m_B[1] += d * N[1];
        this.m_B[2] += d * N[2];

        this.m_C += d * d;
    }


    /**
     * Effectue this = this + Q
     * @param Q la quadrique à additionner à this
     */
    addQuadric(Q)
    {
        this.m_A[0] += Q.m_A[0];
        this.m_A[1] += Q.m_A[1];
        this.m_A[2] += Q.m_A[2];
        this.m_A[3] += Q.m_A[3];
        this.m_A[4] += Q.m_A[4];
        this.m_A[5] += Q.m_A[5];

        this.m_B[0] += Q.m_B[0];
        this.m_B[1] += Q.m_B[1];
        this.m_B[2] += Q.m_B[2];

        this.m_C += Q.m_C;
    }


    /**
     * calcule le coût d'un vec3 P par rapport à la quadrique this
     * @param P : vec3 à évaluer
     * @return coût de P par rapport à this
     */
    getCost(P)
    {
        let x = this.m_A[0] * P[0] + this.m_A[1] * P[1] + this.m_A[2] * P[2];
        let y = this.m_A[1] * P[0] + this.m_A[3] * P[1] + this.m_A[4] * P[2];
        let z = this.m_A[2] * P[0] + this.m_A[4] * P[1] + this.m_A[5] * P[2];
        let VtAV = x * P[0] + y * P[1] + z * P[2];

        let BtV = this.m_B[0] * P[0] + this.m_B[1] * P[1] + this.m_B[2] * P[2];

        return VtAV + 2.0*BtV + this.m_C;
    }
}
