/**
 * Cette classe représente le bord d'un triangle quand on le parcourt dans le sens trigonométrique,
 * C'est le couple orienté de deux sommets appartenant au même triangle
 */

class MeshHalfEdge
{
    /** variables membres
    // appartenance au maillage
    private Mesh m_Mesh;

    // relations avec les autres demi-arêtes
    private MeshHalfEdge m_Sibling;         // demi-arête suivante autour du même sommet d'origine
    private MeshHalfEdge m_Next;            // demi-arête suivante dans le même triangle, son origine = la cible de this
    private MeshHalfEdge m_Opposite;        // demi-arête inverse, appartient au triangle de droite

    // relations avec les éléments du maillage
    private MeshVertex m_Origin;            // sommet situé au début de la demi-arête
    private Triangle m_Triangle;            // triangle à gauche de la demi-arête
    private Edge m_Edge;                    // arête contenant cette demi-arête
    **/


    /**
     * crée une demi-arête partant d'un sommet
     * @param mesh : maillage d'appartenance de cette demi-arête
     * @param origin : sommet de départ de la demi-arête
     * @param target : sommet de fin de la demi-arête
     * @param triangle : celui situé à gauche de cette demi-arête
     */
    constructor(mesh, origin, target, triangle)
    {
        // appartenance au maillage
        this.m_Mesh = mesh;
        if (mesh != null) this.m_Mesh.pushHalfEdge(this);

        // sommet cible de la demi-arête
        this.m_Origin   = origin;
        this.m_Triangle = triangle;
        this.m_Edge     = null;
        this.m_Next     = null;
        this.m_Sibling  = null;

        // vérifier que le sommet d'origine n'a pas déjà une demi-arête allant vers target
        if (origin.getHalfEdgeTo(target) != null) {
            if (mesh == null) {
                throw "Already a half-edge between "+origin.toString()+" and "+target.toString();
            } else {
                throw "In "+mesh.getName()+", already a half-edge between "+origin.toString()+" and "+target.toString();
            }
        }

        // est-ce que le sommet cible contient une demi-arête qui va vers origin ?
        this.m_Opposite = null;
        let other = target.getHalfEdgeTo(origin);
        if (other != null) {
            // vérifier que cette autre demi-arête n'a pas d'opposée (sinon maillage pas manifold)
            if (other.getOpposite() != null) {
                //console.error(TAG, "other="+other.toString()+" its target="<<other.getTarget().toString());
                if (mesh == null) {
                    throw "Opposite half-edge already reserved between "+origin.getName()+" and "+target.getName();
                } else {
                    throw "In "+mesh.getName()+", opposite half-edge already reserved between "+origin.getName()+" and "+target.getName();
                }
            }
            // cette autre arête devient l'opposée de this et réciproquement
            this.m_Opposite = other;
            other.m_Opposite = this;
            this.m_Edge = this.m_Opposite.m_Edge;
        }

        // s'il n'y a pas d'arête, la créer
        if (this.m_Edge == null) {
            this.m_Edge = new MeshEdge(mesh, origin, target);
        }

        // chaînage de la demi-arête autour du sommet origin
        origin.linkSibling(this);
    }


    /**
     * destructeur
     * Il est appelé par celui d'un triangle
     * NB : il ne détruit ni le triangle contenant cette demi-arête, ni les sommets s'ils deviennent isolés.
     * Par contre, il peut détruire l'arête correspondante si elle n'a plus aucune demi-arête.
     */
    destroy()
    {
        // si cette demi-arête a une opposée, les séparer
        if (this.m_Opposite != null) {
            this.m_Opposite.m_Opposite = null;
            this.m_Opposite = null;
        } else {
            // cette demi-arête était isolée, alors supprimer l'arête
            this.m_Edge.destroy();
        }
        this.m_Edge = null;
        this.m_Next = null;

        // retirer cette demi-arête du chaînage de son origine
        this.m_Origin.unlinkSibling(this);

        // supprimer cette demi-arête du maillage
        if (this.m_Mesh != null) this.m_Mesh.popHalfEdge(this);
        this.m_Mesh = null;

        // oublier le lien vers le triangle
        this.m_Triangle = null;
    }


    /**
     * retourne une chaîne représentant cette demi-arête
     * @return nom de la demi-arête
     */
    toString()
    {
        if (this.m_Origin == null) return "MeshHalfEdge(?,?)";
        if (this.m_Opposite == null || this.m_Opposite.m_Origin == null) return "MeshHalfEdge("+ this.m_Origin.getName()+",?)";
        return "MeshHalfEdge("+this.m_Origin.getName()+","+this.m_Opposite.m_Origin.getName()+")";
    }


    /**
     * retourne une chaîne représentant cette demi-arête de manière détaillée
     * @return description de la demi-arête
     */
    toFullString()
    {
        let result = this.toString();

        result += ", orig="+((this.m_Origin != null)?this.m_Origin.toString():"null");
        result += ", tri="+((this.m_Triangle != null)?this.m_Triangle.toString():"null");
        result += ", edge="+((this.m_Edge != null)?this.m_Edge.toString():"null");

        result += ", sib="+((this.m_Sibling != null)?this.m_Sibling.toString():"null");
        result += ", next="+((this.m_Next != null)?this.m_Next.toString():"null");
        result += ", oppo="+((this.m_Opposite != null)?this.m_Opposite.toString():"null");

        return result;
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
     * modifie le maillage associé à this
     * @param mesh : maillage
     */
    setMesh(mesh)
    {
        m_Mesh = mesh;
    }


    /**
     * retourne le sommet qui est au départ de la demi-arête
     * @return sommet d'origine
     */
    getOrigin()
    {
        return this.m_Origin;
    }


    /**
     * retourne le sommet qui est à l'extrémité de la demi-arête
     * @return sommet d'extrémité
     */
    getTarget()
    {
        if (this.m_Next == null) return null;
        return this.m_Next.m_Origin;
    }


    /**
     * retourne la demi-arête opposée (null si aucune)
     * @return demi-arête opposée
     */
    getOpposite()
    {
        return this.m_Opposite;
    }


    /**
     * retourne le triangle d'appartenance de cette demi-arête
     * @return triangle
     */
    getTriangle()
    {
        return this.m_Triangle;
    }


    /**
     * retourne l'arête correspondant à cette demi-arête. Cette arête est partagée
     * avec la demi-arête opposée si elle est présente.
     * @return arete
     */
    getEdge()
    {
        return this.m_Edge;
    }


    /**
     * retourne la demi-arête suivante dans le triangle
     * @return demi-arête suivante
     */
    getNext()
    {
        return this.m_Next;
    }


    /**
     * modifie la demi-arête suivante de this
     * Cette méthode est appelée uniquement par le triangle qui crée cette demi-arête.
     * @param next demi-arête suivante pour tourner autour du triangle
     */
    setNext(next)
    {
        this.m_Next = next;
    }


    /**
     * retourne la demi-arête suivante autour du sommet d'origine
     * @return demi-arête suivante
     */
    getSibling()
    {
        return this.m_Sibling;
    }


    /**
     * modifie la demi-arête suivante de this autour du sommet d'origine
     * @param sibling demi-arête suivante pour tourner autour du triangle
     */
    setSibling(sibling)
    {
        this.m_Sibling = sibling;
    }


    /**
     * Cette méthode effectue la suppression du triangle associé à cette demi-arête
     * Elle reconstruit une arête, s'il faut, reliant les anciennes deux autres demi-arêtes
     * @param halfedge : demi-arête à supprimer
     */
    static collapseTriangle(halfedge)
    {
        // copie des variables membre qui vont disparaître
        let mesh = halfedge.m_Mesh;
        let origin = halfedge.m_Origin;

        // sommet opposé à cette demi-arête
        let vertex = halfedge.m_Next.m_Next.m_Origin;

        // demi-arête opposées côté "droit" et "gauche"
        let oppNN = halfedge.m_Next.m_Next.m_Opposite;
        let oppN = halfedge.m_Next.m_Opposite;

        // supprimer les arêtes de ces deux demi-arêtes
        if (oppN != null) oppN.m_Edge.destroy();
        if (oppNN != null) oppNN.m_Edge.destroy();

        // supprimer le triangle contenant cette demi-arête => suppression de ses 3 demi-arêtes
        halfedge.m_Triangle.destroy();

        // reconstruire les demi-arêtes opposées et l'arête commune
        if (oppN != null || oppNN != null) {
            let edge = new MeshEdge(mesh, origin, vertex);
            if (oppN != null) {
                oppN.m_Opposite = oppNN;
                oppN.m_Edge = edge;
            }
            if (oppNN != null) {
                oppNN.m_Opposite = oppN;
                oppNN.m_Edge = edge;
            }
        }
    }


    /**
     * modifie le maillage pour faire disparaître cette demi-arête :
     * son sommet cible est fusionné sur le sommet origine, faisant disparaître le triangle de gauche et
     * celui de droite s'il existe. L'autre demi-arête disparaît aussi.
     * NB: la méthode est statique car la demi-arête disparaît au cours de l'opération
     * @param halfedge : demi-arête à supprimer
     */
    static collapse(halfedge)
    {
        // copie des variables membre qui vont disparaître
        let origin = halfedge.m_Origin;

        // vertex cible qui va être fusionné sur origin
        let target = halfedge.getTarget();

        // disparition du triangle sur l'arête opposée, avec échange des demi-arêtes opposées
        if (halfedge.m_Opposite != null) MeshHalfEdge.collapseTriangle(halfedge.m_Opposite);

        // disparition du triangle contenant cette arête avec échange des demi-arêtes opposées
        MeshHalfEdge.collapseTriangle(halfedge);

        // modifier l'origine de toutes les demi-arêtes sortantes entourant la cible
        let other = target.m_HalfEdge;
        while (other != null) {
            let next_other = other.m_Sibling;
            // enlever cette arête de la liste de la cible
            target.unlinkSibling(other);
            // changer l'origine de cette arete
            origin.linkSibling(other);
            // changer l'origine de cette demi-arête
            other.m_Origin = origin;
            // modifier l'arête
            other.m_Edge.replaceVertex(target, origin);
            // passer à la demi-arête sortante suivante
            other = next_other;
        }
    }


    /**
     * Cette méthode renverse l'arête diagonale this entre deux triangles jointifs,
     * c'est à dire que les triangles sont reconstruits à partir des 4 sommets,
     * avec l'arête commune placée sur l'autre diagonale.
     * Soient (A,B,C) et (C,D,A) deux triangles. Ils sont jointifs sur l'arête (A,C).
     * Le renversement change les triangles en (A,B,D) et (C,D,B).
     */
    flip()
    {
        // demi-arête opposée
        let opposite = this.m_Opposite;
        if (opposite == null) return;

        // autres demi-arêtes des deux triangles
        let halfedgenext = this.m_Next;
        let halfedgenextnext = halfedgenext.m_Next;
        let oppositenext = opposite.m_Next;
        let oppositenextnext = oppositenext.m_Next;

        // modifier le chaînage de la demi-arête this
        this.m_Next = halfedgenextnext;
        halfedgenextnext.m_Next = oppositenext;
        oppositenext.m_Next = this;

        // modifier le chaînage de la demi-arête opposée
        opposite.m_Next = oppositenextnext;
        oppositenextnext.m_Next = halfedgenext;
        halfedgenext.m_Next = opposite;

        // enlever le chaînage m_Sibling des anciens sommets
        this.m_Origin.unlinkSibling(this);
        opposite.m_Origin.unlinkSibling(opposite);

        // modifier l'origine de this et son opposée
        this.m_Origin = oppositenextnext.m_Origin;
        opposite.m_Origin = halfedgenextnext.m_Origin;

        // remettre le chaînage m_Sibling sur les nouveaux sommets
        this.m_Origin.linkSibling(this);
        opposite.m_Origin.linkSibling(opposite);

        // refaire l'arête
        this.m_Edge.destroy();
        this.m_Edge = new Edge(this.m_Mesh, this.m_Origin, opposite.m_Origin);
    }
}
