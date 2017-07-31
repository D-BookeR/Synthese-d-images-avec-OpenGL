class MeshTriangle
{
    /** variables membre
    // appartenance au maillage
    private Mesh m_Mesh;
    private MeshHalfEdge m_HalfEdge;

    // attributs de triangle
    private vec3 m_Normal;
    private vec3 m_Tangent;
    private vec3 m_Center;
    private float m_Surface;
    private float m_W;
    **/

    /**
     * Constructeur de la classe Triangle à partir de trois sommets
     * NB: l'ordre de rotation des sommets est crucial pour le calcul des normales.
     * Il faut tourner dans le sens trigonométrique, comme dans OpenGL.
     * @param mesh : maillage dans lequel on rajoute ce triangle
     * @param v1 : l'un des coins du triangle, null si création manuelle
     * @param v2 : l'un des coins du triangle, null si création manuelle
     * @param v3 : l'un des coins du triangle, null si création manuelle
     */
    constructor(mesh, v1=null, v2=null, v3=null)
    {
        // appartenance au maillage
        this.m_Mesh = mesh;
        if (mesh != null) this.m_Mesh.pushTriangle(this);

        // créer trois demi-arêtes sauf si les sommets sont nuls
        if (v1 != null && v1 != null && v3 != null) {
            let he12 = new MeshHalfEdge(mesh, v1, v2, this);
            let he23 = new MeshHalfEdge(mesh, v2, v3, this);
            let he31 = new MeshHalfEdge(mesh, v3, v1, this);

            // chaîner ces demi-arêtes
            he12.setNext(he23);
            he23.setNext(he31);
            he31.setNext(he12);

            // le triangle est constitué de cette chaîne
            this.m_HalfEdge = he12;
        } else {
            this.m_HalfEdge = null;
        }

        // seront initialisées dans calcNormale()
        this.m_Normal  = vec3.create();
        this.m_Tangent = vec3.create();
        this.m_Center  = vec3.create();
        this.m_Surface = 0.0;
        this.m_W       = 0.0;
    }


    /**
     * Cette méthode supprime ce triangle du maillage en mettant à jour toutes
     * les listes. Cela peut supprimer des arêtes et rendre des sommets isolés.
     */
    destroy()
    {
        // supprimer ce triangle de la liste du maillage
        if (this.m_Mesh != null) this.m_Mesh.popTriangle(this);
        this.m_Mesh = null;

        // supprimer les trois demi-arêtes de ce triangle
        if (this.m_HalfEdge != null) {
            this.m_HalfEdge.getNext().getNext().destroy();
            this.m_HalfEdge.getNext().destroy();
            this.m_HalfEdge.destroy();
        }
        this.m_HalfEdge = null;
    }


    /**
     * retourne une chaîne qui décrit le triangle
     * @return nom du triangle
     */
    toString()
    {
        // récupérer les trois sommets
        let A = this.getVertex0();
        let nA = (A == null) ? "?" : A.getName();
        let B = this.getVertex1();
        let nB = (B == null) ? "?" : B.getName();
        let C = this.getVertex2();
        let nC = (C == null) ? "?" : C.getName();

        // chaîne mentionnant les noms des sommets
        return "Triangle("+nA+","+nB+","+nC+")";
    }


    /**
     * retourne une chaîne qui décrit le triangle de manière détaillée
     * @return description du triangle
     */
    toFullString()
    {
        if (this.m_HalfEdge != null) {
            return this.toString()+" -> "+this.m_HalfEdge.toString();
        } else {
            return this.toString();
        }
    }


    /**
     * retourne le maillage contenant this
     * @return maillage
     */
    getMesh()
    {
        return this.m_Mesh;
    }


    /**
     * retourne les sommets du triangle sous forme d'un itérable
     * @return itérable sur les trois sommets du triangle
     */
    getVertexList()
    {
        let that = this;        // pour passer this à l'objet interne
        return {
            [Symbol.iterator]() {
                let i = 3;                          // nombre de sommets restant à retourner
                let halfedge = that.m_HalfEdge;   // demi-arête courante
                return {
                    next() {
                        if (i <= 0) return { done: true, value: undefined };
                        halfedge = halfedge.getNext();
                        i--;
                        return { done: false, value: halfedge.getOrigin() };
                    }
                };
            }
        };
    }


    /**
     * retourne le sommet n°n (0..2) du triangle, ou null si n n'est pas correct
     * @param n : numéro 0..2 du sommet
     * @return le sommet demandé ou null si n n'est pas dans les bornes
     */
    getVertex(n)
    {
        if (n < 0 || n > 2) return null;

        let halfedge = this.m_HalfEdge;
        for (let i=0; i<n; i++) {
            halfedge = halfedge.getNext();
        }
        return halfedge.getOrigin();
    }


    /**
     * retourne la demi-arête du triangle
     * @return demi-arête
     */
    getHalfEdge()
    {
        return this.m_HalfEdge;
    }


    /**
     * retourne le premier sommet du triangle
     * @return premier sommet
     */
    getVertex0()
    {
        if (this.m_HalfEdge == null) return null;
        return this.m_HalfEdge.getOrigin();
    }


    /**
     * retourne le deuxième sommet du triangle
     * @return deuxième sommet
     */
    getVertex1()
    {
        if (this.m_HalfEdge == null) return null;
        if (this.m_HalfEdge.getNext() == null) return null;
        return this.m_HalfEdge.getNext().getOrigin();
    }


    /**
     * retourne le troisième sommet du triangle
     * @return troisième sommet
     */
    getVertex2()
    {
        if (this.m_HalfEdge == null) return null;
        if (this.m_HalfEdge.getNext() == null) return null;
        if (this.m_HalfEdge.getNext().getNext() == null) return null;
        return this.m_HalfEdge.getNext().getNext().getOrigin();
    }


    /**
     * retourne la valeur de la normale
     * @see #calcNormale pour la calculer auparavant
     * @return normale du triangle
     */
    getNormal()
    {
        return this.m_Normal;
    }


    /**
     * retourne la valeur de la surface
     * @see #calcNormale pour la calculer auparavant
     * @return surface du triangle
     */
    getSurface()
    {
        return this.m_Surface;
    }


    /**
     * retourne le centre du triangle
     * @see #calcNormale pour le calculer auparavant
     * @return centre du triangle
     */
    getCenter()
    {
        return this.m_Center;
    }


    /**
     * retourne la valeur w du triangle
     * @return w
     */
    getW()
    {
        return this.m_W;
    }


    /**
     * affecte la variable w du triangle
     * @param w
     */
    setW(w)
    {
        this.m_W = w;
    }


    /**
     * recalcule les informations géométriques du triangle : centre, normale, surface...
     */
    computeNormal()
    {
        // les trois sommets
        let A = this.m_HalfEdge.getOrigin();
        let B = this.m_HalfEdge.getNext().getOrigin();
        let C = this.m_HalfEdge.getNext().getNext().getOrigin();

        // les coordonnées des trois sommets
        let cA = A.getCoord();
        let cB = B.getCoord();
        let cC = C.getCoord();

        // vecteurs AB et AC
        let cAB = vec3.create();
        vec3.subtract(cAB, cB, cA);
        let cAC = vec3.create();
        vec3.subtract(cAC, cC, cA);

        // calculer les coordonnées du centre du triangle
        vec3.zero(this.m_Center);
        vec3.add(this.m_Center, cA, cB);
        vec3.add(this.m_Center, this.m_Center, cC);
        vec3.scale(this.m_Center, this.m_Center, 1.0/3.0);

        // calculer le vecteur normal
        vec3.cross(this.m_Normal, cAB, cAC);

        // surface et coefficient W
        this.m_Surface = 0.5 * vec3.length(this.m_Normal);
        vec3.normalize(this.m_Normal, this.m_Normal);
        this.m_W = -vec3.dot(this.m_Normal, cA);
    }


    /**
     * retourne la valeur de la tangente
     * @see #calcTangente pour la calculer auparavant
     * @return tangente du triangle
     */
    getTangent()
    {
        return this.m_Tangent;
    }


    /**
     * recalcule la tangente du triangle à l'aide de la normale et des coordonnées de texture
     */
    computeTangent()
    {
        // les trois sommets
        let A = this.m_HalfEdge.getOrigin();
        let B = this.m_HalfEdge.getNext().getOrigin();
        let C = this.m_HalfEdge.getNext().getNext().getOrigin();

        // les coordonnées des trois sommets
        let cA = A.getCoord();
        let cB = B.getCoord();
        let cC = C.getCoord();

        // vecteurs AB et AC
        let cAB = vec3.create();
        vec3.subtract(cAB, cB, cA);
        let cAC = vec3.create();
        vec3.subtract(cAC, cC, cA);

        // récupération de leur 2e coordonnée de texture
        let tA = A.getTexCoord()[1];
        let tB = B.getTexCoord()[1];
        let tC = C.getTexCoord()[1];

        // vecteurs dans l'espace (s,t), et uniquement la coordonnée t
        let tAB = tB - tA;
        let tAC = tC - tA;

        // TODO s'il n'y a pas de coordonnées de texture, alors tAB et tAC sont nuls, les remplacer par AB et AC

        // calcul de la tangente
        vec3.scale(cAB, cAB, tAC);
        vec3.scale(cAC, cAC, tAB);
        vec3.subtract(this.m_Tangent, cAB, cAC);

        // normalisation
        vec3.normalize(this.m_Tangent, this.m_Tangent);
    }


    /**
     * Cette méthode fait tourner les sommets du triangle afin que
     * celui qui est passé en paramètre se trouve en premier
     * @param vertex : sommet à mettre en tête de liste (sauf s'il en est absent)
     * @return true si ok, false si le sommet est absent du triangle
     */
    cycleVertexFirst(vertex)
    {
        // est-ce que sommet = origine de la première demi-arête ?
        if (this.m_HalfEdge.getOrigin() == vertex) return true;

        // est-ce que sommet = origine de la deuxième demi-arête ?
        this.m_HalfEdge = this.m_HalfEdge.getNext();
        if (this.m_HalfEdge.getOrigin() == vertex) return true;

        // est-ce que sommet = origine de la troisième demi-arête ?
        this.m_HalfEdge = this.m_HalfEdge.getNext();
        if (this.m_HalfEdge.getOrigin() == vertex) return true;

        // échec, le sommet n'est origine d'aucune demi-arête
        return false;
    }


    /**
     * Cette méthode indique si le triangle this contient le sommet indiqué
     * @param vertex : sommet dont il faut vérifier l'appartenance à this
     * @return true si ok, false si le sommet est absent du triangle
     */
    containsVertex(vertex)
    {
        if (this.m_HalfEdge.getOrigin() == vertex) return true;
        if (this.m_HalfEdge.getNext().getOrigin() == vertex) return true;
        if (this.m_HalfEdge.getNext().getNext().getOrigin() == vertex) return true;
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
    containsEdge(vertex1, vertex2)
    {
        if (! this.cycleVertexFirst(vertex1)) return false;
        return this.m_HalfEdge.getNext().getOrigin() == vertex2;
    }


    /**
     * remplace un sommet par un autre dans ce triangle
     * @param A sommet à remplacer
     * @param A1 sommet à mettre à la place
     */
    replaceVertex(A, A1)
    {
        // mettre A en premier
        if (! this.cycleVertexFirst(A)) return;

        // les deux autres sommets actuels du triangle
        let B = this.getVertex1();
        let C = this.getVertex2();

        // demi-arêtes du triangle
        let daAB = this.m_HalfEdge;
        let daBC = daAB.getNext();
        let daCA = daBC.getNext();

        // supprimer les deux demi-arêtes AB et CA
        daAB.destroy();
        daCA.destroy();
        daBC.setNext(null);

        // créer deux demi-arêtes allant de et vers A1
        daAB = new MeshHalfEdge(this.m_Mesh, A1, B, this);
        daCA = new MeshHalfEdge(this.m_Mesh, C, A1, this);

        // chaîner ces demi-arêtes
        daAB.setNext(daBC);
        daBC.setNext(daCA);
        daCA.setNext(daAB);

        // le triangle est constitué de cette chaîne
        this.m_HalfEdge = daAB;
    }
}
