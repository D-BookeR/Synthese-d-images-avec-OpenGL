/**
 * Cette classe représente l'un des sommets d'un maillage
 */
class MeshVertex
{
    /** variables membre
    // attributs de sommet
    private String m_Name;
    private vec3 m_Coord;
    private vec4 m_Color;
    private vec3 m_Normal;
    private vec3 m_Tangent;
    private vec2 m_TexCoord;

    // appartenance au maillage
    private Mesh m_Mesh;
    private MeshHalfEdge m_HalfEdge;       // demi-arête sortant de ce sommet

    // indice du vertex dans les VBO
    private int m_Number;

    // ajouts pour le module MeshModuleRedux
    private Quadric m_Quadric;
    private MeshVertex m_CollapseTarget;
    private float m_CollapseCost;
    **/


    /**
     * Constructeur de la classe MeshVertex qui représente un sommet dans
     * le maillage. Initialise un sommet non connecté au maillage, employer
     * Mesh.addVertex pour l'insérer puis new Triangle(...) pour le mettre
     * dans un triangle, setCoord et setColor pour lui donner des coordonnées et
     * des couleurs.
     * @param mesh : maillage d'appartenance de ce sommet
     * @param name : nom du sommet à créer
     */
    constructor(mesh, name)
    {
        // appartenance au maillage
        this.m_Mesh = mesh;
        if (mesh != null) {
            this.m_Mesh.pushVertex(this);

            // numéro du vertex dans les VBO
            this.m_Number = this.m_Mesh.getVertexCount();
        }

        // nom du sommet
        this.m_Name = name;

        // attributs du sommet, voir setCoord(), setColor(), setNormal()...
        this.m_Attributes = [
            vec4.create(),      // ID_ATTR_VERTEX
            vec4.create(),      // ID_ATTR_COLOR
            vec4.create(),      // ID_ATTR_NORMAL
            vec4.create(),      // ID_ATTR_TANGENT
            vec4.create(),      // ID_ATTR_TEXCOORD
            vec4.create(),      // ID_ATTR_VERTEX1
            vec4.create(),      // ID_ATTR_NORMAL1
            vec4.create(),      // ID_ATTR_IDBONES
            vec4.create(),      // ID_ATTR_WEIGHTS
        ];

        // demi-arête
        this.m_HalfEdge = null;

        // divers
        this.m_Quadric = null;
    }


    /** destructeur */
    destroy()
    {
        // supprimer ce sommet de la liste du maillage
        if (this.m_Mesh != null) this.m_Mesh.popVertex(this);
        this.m_Mesh = null;

        // supprimer la quadrique si elle existe
        if (this.m_Quadric != null) this.m_Quadric.destroy();

        // enlever tous les triangles autour de ce sommet
        let halfedge = this.m_HalfEdge;
        while (halfedge != null) {
            let next = halfedge.getSibling();
            let triangle = halfedge.getTriangle();
            triangle.destroy();
            halfedge = next;
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
     * retourne la première MeshHalfEdge de this
     * @return MeshHalfEdge
     */
    getHalfEdge()
    {
        return this.m_HalfEdge;
    }


    /**
     * cette méthode rajoute la demi-arête dans le chaînage m_Sibling (demi-arêtes autour de this)
     * @see #unlinkSibling qui fait l'inverse
     * @param halfedge à rajouter dans la liste m_Sibling de this
     */
    linkSibling(halfedge)
    {
        // mettre la demi-arête en tête de la liste
        halfedge.setSibling(this.m_HalfEdge);
        this.m_HalfEdge = halfedge;
    }


    /**
     * cette méthode retire cette demi-arête de la liste m_Sibling (demi-arêtes autour de this)
     * @see #linkSibling qui fait l'inverse
     * @param halfedge à enlever de la liste m_Sibling de this
     */
    unlinkSibling(halfedge)
    {
        // si le sommet this désigne la demi-arête, alors le faire désigner la suivante
        if (this.m_HalfEdge == halfedge) {
            // this doit maintenant désigner la demi-arête suivante dans sa liste
            this.m_HalfEdge = this.m_HalfEdge.getSibling();
            // si this.m_HalfEdge==null, alors le sommet this devient libre (supprimable)
        } else {
            // le sommet désigne une autre demi-arête, il faut parcourir le chaînage
            let other = this.m_HalfEdge;
            while (other != null) {
                if (other.getSibling() == halfedge) {
                    other.setSibling(halfedge.getSibling());
                    break;
                }
                other = other.getSibling();
            }
        }
        // la demi-arête n'a plus de suivant
        halfedge.setSibling(null);
    }


    /**
     * retourne la liste des triangles autour du sommet this
     * Cette liste est basée sur les liens m_Sibling entre triangles
     * NB : la liste n'est pas du tout triée par ordre autour du sommet
     * @see #getTrianglesOrderedAround pour une liste triée mais pas forcément complète
     * @return liste des triangles
     */
    getTrianglesAround()
    {
        let triangles = [];
        let halfedge = this.m_HalfEdge;  // demi-arête courante

        while (halfedge != null) {
            let triangle = halfedge.getTriangle();
            triangles.push(triangle);
            halfedge = halfedge.getSibling();
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
    getTrianglesOrderedAround()
    {
        let triangles = [];
        let halfedge = this.m_HalfEdge;   // demi-arête courante
        let first    = this.m_HalfEdge;   // première demi-arête vue

        while (halfedge != null) {
            let triangle = halfedge.getTriangle();
            triangles.push(triangle);
            // on essaie d'aller sur la m_Next.m_Next.m_Opposite
            if (halfedge.getNext() != null) halfedge = halfedge.getNext();
            if (halfedge.getNext() != null) halfedge = halfedge.getNext();
            if (halfedge.getOpposite() != null) halfedge = halfedge.getOpposite();
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
    getNeighborVertices()
    {
        // collection retournée par la méthode
        let neighbors = new Set();

        // parcourir les demi-arêtes autour de this
        let trianglehalfedge = this.m_HalfEdge;
        while (trianglehalfedge != null) {
            let triangle = trianglehalfedge.getTriangle();

            // ajouter les trois sommets du triangle sauf s'ils y sont déjà
            let vertexhalfedge = triangle.getHalfEdge();
            for (let i=0; i<3; i++) {
                if (vertexhalfedge.getOrigin() != this) {
                    neighbors.add(vertexhalfedge.getOrigin());
                }
                // passer à la demi-arête suivante sur ce triangle
                vertexhalfedge = vertexhalfedge.getNext();
            }

            // passer à la demi-arête suivante autour de this
            trianglehalfedge = trianglehalfedge.getSibling();
        }

        return [...neighbors];
    }


    /**
     * retourne la demi-arête allant de this à vertex
     * @param vertex : destination de la demi-arête recherchée
     * @return allant de this à vertex, ou null si absente
     */
    getHalfEdgeTo(vertex)
    {
        let halfedge = this.m_HalfEdge;
        while (halfedge != null) {
            if (halfedge.getTarget() == vertex) return halfedge;
            halfedge = halfedge.getSibling();
        }
        // pas trouvée
        return null;
    }


    /**
     * retourne le triangle situé à droite de l'arête allant de this à vertex
     * @param vertex : second sommet
     * @return triangle ou null si aucun
     */
    getTriangleRightTo(vertex)
    {
        // chercher la demi-arête entre this et vertex
        let halfedge = this.getHalfEdgeTo(vertex);
        if (halfedge == null || halfedge.getOpposite() == null) return null;
        return halfedge.getOpposite().getTriangle();
    }


    /**
     * retourne le triangle situé à gauche de l'arête allant de this à vertex
     * @param vertex : second sommet
     * @return triangle ou null si aucun
     */
    getTriangleLeftTo(vertex)
    {
        // chercher la demi-arête entre this et vertex
        let halfedge = this.getHalfEdgeTo(vertex);
        if (halfedge == null) return null;
        return halfedge.getTriangle();
    }


    /**
     * retourne le nom du sommet
     * @return le nom du sommet
     */
    toString()
    {
        return this.m_Name;
    }


    /**
     * retourne une chaîne représentant ce sommet de manière détaillée
     * @return description du sommet
     */
    toFullString()
    {
        let result = this.toString();
        let halfedge = this.m_HalfEdge;
        while (halfedge != null) {
            result += " -> "+halfedge.toString();
            halfedge = halfedge.getSibling();
        }
        return result;
    }


    /**
     * retourne le nom du vertex
     * @return nom du sommet
     */
    getName()
    {
        return this.m_Name;
    }


    /**
     * modifie le numéro du vertex
     * @param number : numero du sommet
     */
    setNumber(number)
    {
        this.m_Number = number;
    }


    /**
     * retourne le numéro du vertex
     * @return numéro du sommet
     */
    getNumber()
    {
        return this.m_Number;
    }


    /**
     * définit les coordonnées du sommet
     * @param xyz coordonnées
     * @return this pour pouvoir chaîner les affectations
     */
    setCoord(xyz)
    {
        vec3.copy(this.m_Attributes[MeshVertex.ID_ATTR_VERTEX], xyz);
        this.m_Attributes[MeshVertex.ID_ATTR_VERTEX][3] = 1.0;
        return this;
    }


    /**
     * retourne les coordonnées du sommet
     * @return coordonnées 3D du sommet
     */
    getCoord()
    {
        return this.m_Attributes[MeshVertex.ID_ATTR_VERTEX];
    }


    /**
     * définit la couleur du sommet
     * @param rgba couleur
     * @return this pour pouvoir chaîner les affectations
     */
    setColor(rgba)
    {
        if (rgba.length < 4) {
            vec3.copy(this.m_Attributes[MeshVertex.ID_ATTR_COLOR], rgba);
            this.m_Attributes[MeshVertex.ID_ATTR_COLOR][3] = 1.0;
        } else {
            vec4.copy(this.m_Attributes[MeshVertex.ID_ATTR_COLOR], rgba);
        }
        return this;
    }


    /**
     * retourne la couleur du sommet
     * @return couleur (r,g,b,a)
     */
    getColor()
    {
        return this.m_Attributes[MeshVertex.ID_ATTR_COLOR];
    }


    /**
     * définit les coordonnées de la normale du sommet
     * @param normal : normale à affecter
     * @return this pour pouvoir chaîner les affectations
     */
    setNormal(normal)
    {
        vec3.copy(this.m_Attributes[MeshVertex.ID_ATTR_NORMAL], normal);
        this.m_Attributes[MeshVertex.ID_ATTR_NORMAL][3] = 0.0;
        return this;
    }


    /**
     * retourne la normale du sommet
     * @return normale
     */
    getNormal()
    {
        return this.m_Attributes[MeshVertex.ID_ATTR_NORMAL];
    }


    /**
     * retourne la tangente du sommet
     * @return tangente
     */
    getTangent()
    {
        return this.m_Attributes[MeshVertex.ID_ATTR_TANGENT];
    }


    /**
     * définit les coordonnées de texture du sommet
     * @param uv coordonnées de texture
     * @return this pour pouvoir chaîner les affectations
     */
    setTexCoord(uv)
    {
        vec2.copy(this.m_Attributes[MeshVertex.ID_ATTR_TEXCOORD], uv);
        return this;
    }


    /**
     * retourne les coordonnées de texture du sommet
     * @return coordonnées de texture
     */
    getTexCoord()
    {
        return this.m_Attributes[MeshVertex.ID_ATTR_TEXCOORD];
    }


    /**
     * retourne l'attribut dont on fournit l'identifiant
     * @param idattr : numéro de l'attribut, ex: MeshVertex.ID_ATTR_VERTEX
     * @return vec4 contenant l'attribut
     */
    getAttribute(idattr)
    {
        return this.m_Attributes[idattr];
    }


    /**
     * affecte ou définit l'attribut dont on fournit l'identifiant
     * @param idattr : numéro de l'attribut, ex: MeshVertex.ID_ATTR_VERTEX
     * @param value : valeur à affecter à l'attribut
     * @return this pour pouvoir chaîner les affectations
     */
    setAttribute(idattr, value)
    {
        vec4.copy(this.m_Attributes[idattr], value);
        return this;
    }


    /**
     * Cette méthode calcule la normale du sommet = moyenne des normales des
     * triangles contenant ce sommet.
     */
    computeNormal()
    {
        // calculer la moyenne des normales des triangles contenant ce sommet
        vec3.zero(this.m_Attributes[MeshVertex.ID_ATTR_NORMAL]);
        let halfedge = this.m_HalfEdge;
        while (halfedge != null) {
            let triangle = halfedge.getTriangle();
            // normale du triangle courant
            let normal = vec3.clone(triangle.getNormal());
            // prendre en compte la surface du triangle
            vec3.scale(normal, normal, triangle.getSurface());
            // cumuler les normales pondérées
            vec3.add(this.m_Attributes[MeshVertex.ID_ATTR_NORMAL], this.m_Attributes[MeshVertex.ID_ATTR_NORMAL], normal);
            // passer à la demi-arête suivante autour de ce sommet
            halfedge = halfedge.getSibling();
        }
        // normaliser le résultat
        vec3.normalize(this.m_Attributes[MeshVertex.ID_ATTR_NORMAL], this.m_Attributes[MeshVertex.ID_ATTR_NORMAL]);
    }


    /**
     * Cette méthode calcule la tangente du sommet = moyenne des tangentes des
     * triangles contenant ce sommet.
     */
    computeTangent()
    {
        // calculer la moyenne des tangentes des triangles contenant ce sommet
        vec3.zero(this.m_Attributes[MeshVertex.ID_ATTR_TANGENT]);
        let halfedge = this.m_HalfEdge;
        while (halfedge != null) {
            let triangle = halfedge.getTriangle();
            // tangente du triangle courant
            let tangent = vec3.clone(triangle.getTangent());
            // prendre en compte la surface du triangle
            vec3.scale(tangent, tangent, triangle.getSurface());
            // cumuler les tangentes pondérées
            vec3.add(this.m_Attributes[MeshVertex.ID_ATTR_TANGENT], this.m_Attributes[MeshVertex.ID_ATTR_TANGENT], tangent);
            // passer à la demi-arête suivante autour de ce sommet
            halfedge = halfedge.getSibling();
        }
        // normaliser le résultat
        vec3.normalize(this.m_Attributes[MeshVertex.ID_ATTR_TANGENT], this.m_Attributes[MeshVertex.ID_ATTR_TANGENT]);
    }


    /**
     * retourne le sommet visé pour une fusion
     * @see MeshModuleRedux
     * @return sommet
     */
    getCollapseTarget()
    {
        return this.m_CollapseTarget;
    }


    /**
     * modifie le sommet visé pour une fusion
     * @see MeshModuleRedux
     * @param vertex : sommet à affecter
     */
    setCollapseTarget(vertex)
    {
        this.m_CollapseTarget = vertex;
    }


    /**
     * retourne le coût d'une fusion vers m_CollapseTarget
     * @see MeshModuleRedux
     * @return coût
     */
    getCollapseCost()
    {
        return this.m_CollapseCost;
    }


    /**
     * modifie le coût d'une fusion vers m_CollapseTarget
     * @see MeshModuleRedux
     * @param cost : coût à affecter
     */
    setCollapseCost(cost)
    {
        this.m_CollapseCost = cost;
    }


    /**
     * retourne la quadrique permettant de calculer le coût d'une réduction
     * @see MeshModuleRedux
     * @return quadrique
     */
    getQuadric()
    {
        return this.m_Quadric;
    }


    /**
     * modifie la quadrique permettant de calculer le coût d'une réduction
     * @see MeshModuleRedux
     * @param quadric : quadrique à affecter
     */
    setQuadric(quadric)
    {
        this.m_Quadric = quadric;
    }


    /**
     * retourne une copie de ce sommet (this)
     * @param suffix : chaine à rajouter au nom de this
     * @return copie du sommet this
     */
    clone(suffix)
    {
        let newvertex = new MeshVertex(this.m_Mesh, this.m_Name+suffix);
        // recopier les attributs du vertex
        for (let i=0; i<this.m_Attributes.length; i++) {
            vec4.copy(newvertex.m_Attributes[i], this.m_Attributes[i]);
        }
        return newvertex;
    }


    /**
     * Cette méthode modifie les attributs du vertex pour qu'ils soient interpolés
     * linéairement entre ceux de v0 et de v1, d'un facteur k
     * @param v0 : premier vertex, (k=0)
     * @param v1 : second vertex (k=1)
     * @param k : valeur entre 0 et 1 pour l'interpolation
     */
    lerp(v0, v1, k)
    {
        // interpoler les attributs
        for (let i=0; i<this.m_Attributes.length; i++) {
            vec4.lerp(this.m_Attributes[i], v0.m_Attributes[i], v1.m_Attributes[i], k);
        }
        // normaliser les vecteurs
        vec3.normalize(this.m_Attributes[MeshVertex.ID_ATTR_NORMAL], this.m_Attributes[MeshVertex.ID_ATTR_NORMAL]);
        vec3.normalize(this.m_Attributes[MeshVertex.ID_ATTR_TANGENT], this.m_Attributes[MeshVertex.ID_ATTR_TANGENT]);
        vec3.normalize(this.m_Attributes[MeshVertex.ID_ATTR_NORMAL1], this.m_Attributes[MeshVertex.ID_ATTR_NORMAL1]);
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
    hermite(v0, t0, v1, t1, k)
    {
        // interpoler les coordonnées par hermite
        vec3.hermite(this.m_Attributes[MeshVertex.ID_ATTR_VERTEX],
            v0.m_Attributes[MeshVertex.ID_ATTR_VERTEX], t0,
            v1.m_Attributes[MeshVertex.ID_ATTR_VERTEX], t1,
            k);
        // interpoler linéairement les autres attributs (il faudrait leurs dérivées)
        for (let i=1; i<this.m_Attributes.length; i++) {
            vec4.lerp(this.m_Attributes[i], v0.m_Attributes[i], v1.m_Attributes[i], k);
        }
        // normaliser les vecteurs
        vec3.normalize(this.m_Attributes[MeshVertex.ID_ATTR_NORMAL], this.m_Attributes[MeshVertex.ID_ATTR_NORMAL]);
        vec3.normalize(this.m_Attributes[MeshVertex.ID_ATTR_TANGENT], this.m_Attributes[MeshVertex.ID_ATTR_TANGENT]);
        vec3.normalize(this.m_Attributes[MeshVertex.ID_ATTR_NORMAL1], this.m_Attributes[MeshVertex.ID_ATTR_NORMAL1]);
    }
}


/**
 * Constantes prédéfinies pour les attributs d'un vertex
 * voir MeshVertex.getAttribut, c'est le code qu'il faut pour idattr
 */
MeshVertex.ID_ATTR_VERTEX     = 0;      // coordonnées 3D du sommet
MeshVertex.ID_ATTR_COLOR      = 1;      // couleur du sommet
MeshVertex.ID_ATTR_NORMAL     = 2;      // vecteur normal
MeshVertex.ID_ATTR_TANGENT    = 3;      // vecteur tangent
MeshVertex.ID_ATTR_TEXCOORD   = 4;      // coordonnées de texture
MeshVertex.ID_ATTR_VERTEX1    = 5;      // coordonnées 3D alternatives (animation par cibles)
MeshVertex.ID_ATTR_NORMAL1    = 6;      // normale alternative (animation par cibles)
MeshVertex.ID_ATTR_IDBONES    = 7;      // numéros des 4 os qui font bouger le sommet
MeshVertex.ID_ATTR_WEIGHTS    = 8;      // poids des 4 os qui font bouger le sommet
