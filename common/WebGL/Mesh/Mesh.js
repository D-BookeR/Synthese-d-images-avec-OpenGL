// Définition des classes permettant de gérer des maillages



/**
 * Cette classe représente un maillage 3D : triangles et sommets
 */
class Mesh
{
    /** variables membres
    private static final String TAG = "OpenGL";

    private String m_Name;

    private ArrayList<MeshVertex>   m_VertexList;       // liste des sommets
    private ArrayList<MeshHalfEdge> m_HalfEdgeList;     // liste des demi-arêtes
    private ArrayList<Edge>         m_EdgeList;         // liste des arêtes
    private ArrayList<Triangle>     m_TriangleList;     // liste des triangles
    **/


    /**
     * constructeur d'un maillage
     * @param name : nom du maillage
     */
    constructor(name="indef")
    {
        // nom
        this.m_Name = name;
        this.m_VertexList = [];
        this.m_HalfEdgeList = [];
        this.m_EdgeList = [];
        this.m_TriangleList = [];
        // indique que les VBOs sont alloués, qu'on peut le dessiner
        this.m_IsReady = true;
    }


    /**
     * destructeur
     */
    destroy()
    {
        // supprimer les triangles
        for (let triangle of this.m_TriangleList) {
            triangle.destroy();
        }
        // supprimer les sommets
        for (let vertex of this.m_VertexList) {
            vertex.destroy();
        }
        // supprimer les arêtes qui resteraient
        for (let edge of this.m_EdgeList) {
            edge.destroy();
        }
        // supprimer les demi-arêtes qui resteraient
        for (let halfedge of this.m_HalfEdgeList) {
            halfedge.destroy();
        }
    }


    /**
     * retourne le nom du maillage
     * @return nom du maillage
     */
    getName()
    {
        return this.m_Name;
    }


    /**
     * retourne la liste des sommets du maillage
     * @return liste des sommets
     */
    getVertexList()
    {
        return this.m_VertexList;
    }


    /**
     * retourne la liste des demi-arêtes du maillage
     * @return liste des demi-arêtes
     */
    getHalfEdgeList()
    {
        return this.m_HalfEdgeList;
    }


    /**
     * retourne la liste des arêtes du maillage
     * @return liste des arêtes
     */
    getEdgeList()
    {
        return this.m_EdgeList;
    }


    /**
     * retourne la liste des triangles du maillage
     * @return liste des triangles
     */
    getTriangleList()
    {
        return this.m_TriangleList;
    }


    /**
     * retourne vrai si le maillage est dessinable (en cas de chargement asynchrone)
     * @return
     */
    isReady()
    {
        return this.m_IsReady;
    }


    /**
     * indique si le maillage est dessinable (en cas de chargement asynchrone)
     * @param ready
     */
    setReady(ready=true)
    {
        this.m_IsReady = ready;
    }


    /**
     * retourne le nombre de sommets
     * @return nombre de sommets
     */
    getVertexCount()
    {
        return this.m_VertexList.length;
    }


    /**
     * retourne le nombre de triangles
     * @return nombre de triangles
     */
    getTriangleCount()
    {
        return this.m_TriangleList.length;
    }


    /**
     * retourne le sommet n°i (0..) du maillage, ou null si i n'est pas correct
     * @param i : numéro 0..NV-1 du sommet
     * @return le sommet demandé ou nullptr si i n'est pas dans les bornes
     */
    getVertex(i)
    {
        if (i<0 || i>= this.m_VertexList.length) return null;
        return this.m_VertexList[i];
    }


    /**
     * Cette fonction retourne le sommet qui porte le nom indiqué
     * NB : cette fonction est très lente dans un grand maillage
     * @param name : nom du vertex à retourner
     * @return vertex ou nullptr
     */
    getVertexByName(name)
    {
        for (let vertex of this.m_VertexList) {
            if (vertex.getName() == name) return vertex;
        }
        return null;
    }


    /**
     * retourne le triangle n°i (0..) du maillage, ou null si i n'est pas correct
     * @param i : numéro 0..NT-1 du triangle
     * @return le Triangle() demandé ou null si i n'est pas dans les bornes
     */
    getTriangle(i)
    {
        if (i<0 || i>= this.m_TriangleList.length) return null;
        return this.m_TriangleList[i];
    }


    /**
     * affiche le nombre de sommets et de triangles sur stdout
     */
    info()
    {
        console.log(this.m_VertexList.length+" vertices and "+this.m_TriangleList.length+" triangles");
    }


    /**
     * Cette méthode ajoute le sommet fourni à la fin de la liste
     * @param vertex : sommet à rajouter
     */
    pushVertex(vertex)
    {
        this.m_VertexList.push(vertex);
    }


    /**
     * Cette méthode enlève le sommet fourni de la liste
     * NB: la méthode ne le supprime pas (voir son destructeur pour cela)
     * @see #delVertex
     * @param vertex : sommet à enlever
     */
    popVertex(vertex)
    {
        list_remove(this.m_VertexList, vertex);
    }


    /**
     * Cette méthode ajoute le triangle fourni à la fin de la liste
     * @param triangle à rajouter
     */
    pushTriangle(triangle)
    {
        this.m_TriangleList.push(triangle);
    }


    /**
     * Cette méthode enlève le triangle fourni de la liste
     * NB: la méthode ne le supprime pas (voir son destructeur pour cela)
     * @see #delTriangle
     * @param triangle à enlever
     */
    popTriangle(triangle)
    {
        list_remove(this.m_TriangleList, triangle);
    }


    /**
     * Cette méthode ajoute l'arête fournie à la fin de la liste
     * @param edge : arete à rajouter
     */
    pushEdge(edge)
    {
        this.m_EdgeList.push(edge);
    }


    /**
     * Cette méthode enlève l'arête fournie de la liste
     * NB: la méthode ne le supprime pas (voir son destructeur pour cela)
     * @param edge : arete à enlever
     */
    popEdge(edge)
    {
        list_remove(this.m_EdgeList, edge);
    }


    /**
     * Cette méthode ajoute la demi-arête fournie à la fin de la liste
     * @param halfedge : demi-arête à rajouter
     */
    pushHalfEdge(halfedge)
    {
        this.m_HalfEdgeList.push(halfedge);
    }


    /**
     * Cette méthode enlève la demi-arête fournie de la liste
     * NB: la méthode ne le supprime pas (voir son destructeur pour cela)
     * @param halfedge à enlever
     */
    popHalfEdge(halfedge)
    {
        list_remove(this.m_HalfEdgeList, halfedge);
    }


    /**
     * Cette méthode crée et rajoute un sommet au maillage
     * @param name : nom du sommet à créer
     * @return le nouveau sommet
     */
    addVertex(name)
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
    addTriangle(v1, v2, v3)
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
    addQuad(v1, v2, v3, s4)
    {
        this.addTriangle(v1, v2, s4);
        this.addTriangle(s4, v2, v3);
    }


    /**
     * Cette méthode crée et rajoute un éventail de triangles au maillage.
     * NB: l'ordre de rotation des sommets est crucial pour le calcul des normales.
     * Il faut tourner dans le sens trigonométrique, comme dans OpenGL.
     * @param vertices : liste des sommets du polygone
     */
    addPolygonConvex(vertices)
    {
        // créer un éventail autour du premier point fourni
        let pivot = vertices[0];
        let count = vertices.length;
        for (let i=0; i<count-2; i++) {
            this.addTriangle(pivot, vertices[i+1], vertices[i+2]);
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
    addPolygon(vertices, normal)
    {
        // vecteurs temporaires
        let AB = vec3.create();
        let BC = vec3.create();
        let CA = vec3.create();
        let N = vec3.create();
        let T = vec3.create();
        let P = vec3.create();

        // tant qu'il reste au moins 3 sommets
        let vertice_count = vertices.length;
        while (vertice_count >= 3) {
            // chercher trois sommets successifs formant une arête convexe
            let ok = false;
            for (let i=0; i<vertice_count-2; i++) {
                let A = vertices[(i+0)%vertice_count];
                let B = vertices[(i+1)%vertice_count];
                let C = vertices[(i+2)%vertice_count];
                let cA = A.getCoord();
                let cB = B.getCoord();
                let cC = C.getCoord();
                // est-ce que les trois premiers forment une arête convexe ?
                vec3.subtract(AB, cB, cA);
                vec3.subtract(BC, cC, cB);
                vec3.cross(N, AB, BC);
                // si le produit scalaire entre N et la normale est positif, alors c'est convexe
                if (vec3.dot(N, normal) >= 0.0) {
                    // vérifier qu'aucun des autres points du contour n'est dans ce triangle
                    let inside = false;
                    for (let j=0; j<vertice_count; j++) {
                        if (i <= j && j <= i+2) continue;
                        let cP = vertices[j].getCoord();
                        // comparaison avec le côté AB
                        vec3.cross(T, AB, N);
                        vec3.subtract(P, cP, cA);
                        let lefttoAB = (vec3.dot(P, T) >= 0.0);
                        // comparaison avec le côté BC
                        vec3.cross(T, BC, N);
                        vec3.subtract(P, cP, cB);
                        let lefttoBC = (vec3.dot(P, T) >= 0.0);
                        // comparaison avec le côté CA
                        vec3.subtract(CA, cA, cC);
                        vec3.cross(T, CA, N);
                        vec3.subtract(P, cP, cC);
                        let lefttoCA = (vec3.dot(P, T) >= 0.0);
                        // dans le futur triangle ?
                        if (lefttoAB && lefttoBC && lefttoCA) {
                            inside = true;
                            break;          // inutile de regarder les autres sommets
                        }
                    }
                    // si aucun point n'est dans le futur triangle, on peut le créer
                    if (! inside) {
                        this.addTriangle(A, B, C);
                        // enlever B de la liste
                        vertices.splice((i+1)%vertice_count, 1);
                        vertice_count = vertice_count - 1;
                        // on a pu construire au moins un triangle
                        ok = true;
                    }
                }
            }
            // si on n'a pas pu faire un seul triangle, il y a un souci
            if (!ok) throw "impossible de construire le polygone";
        }
    }


    /**
     * Cette méthode supprime le triangle du maillage en mettant à jour toutes
     * les listes
     * @param triangle : celui qu'il faut supprimer
     */
    delTriangle(triangle)
    {
        triangle.destroy();
    }


    /**
     * Cette méthode supprime le sommet du maillage en mettant à jour toutes
     * les listes : tous les triangles qui le contiennent sont aussi supprimés.
     * @param vertex : celui qu'il faut supprimer
     */
    delVertex(vertex)
    {
        vertex.destroy();
    }


    /**
     * Cette méthode recalcule les normales des triangles et sommets.
     * Les normales des triangles sont calculées d'après leurs côtés.
     * Les normales des sommets sont les moyennes des normales des triangles
     * auxquels ils appartiennent.
     */
    computeNormals()
    {
        // calculer les normales des triangles
        for (let triangle of this.m_TriangleList) {
            triangle.computeNormal();
        }

        // calculer les normales des sommets
        let iv = 0;
        for (let vertex of this.m_VertexList) {
            // renuméroter le sommet (futur numéro dans les VBOs)
            vertex.setNumber(iv);
            iv++;
            // recalculer la normale de ce sommet
            vertex.computeNormal();
        }
    }


    /**
     * Cette méthode recalcule les tangentes des triangles et sommets.
     * Les tangentes des triangles sont calculées d'après leurs côtés et les coordonnées de texture.
     * Les tangentes des sommets sont les moyennes des tangentes des triangles
     * auxquels ils appartiennent.
     */
    computeTangents()
    {
        // calculer les tangentes des triangles
        for (let triangle of this.m_TriangleList) {
            triangle.computeTangent();
        }

        // calculer les tangentes des sommets
        for (let vertex of this.m_VertexList) {
            // recalculer la tangente de ce sommet
            vertex.computeTangent();
        }
    }
}
