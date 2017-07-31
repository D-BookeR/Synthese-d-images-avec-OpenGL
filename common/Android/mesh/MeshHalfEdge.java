/**
 * Cette classe représente le bord d'un triangle quand on le parcourt dans le sens trigonométrique,
 * C'est le couple orienté de deux sommets appartenant au même triangle
 */

package com.dbooker.livreopengl.mesh;

@SuppressWarnings("unused")
public class MeshHalfEdge
{
    // appartenance au maillage
    private Mesh m_Mesh;

    // relations avec les autres demi-arêtes
    private MeshHalfEdge m_Sibling;        // demi-arête suivante autour du même sommet d'origine
    private MeshHalfEdge m_Next;           // demi-arête suivante dans le même triangle, son origine = la target de this
    private MeshHalfEdge m_Opposite;       // demi-arête inverse, appartient au triangle de droite

    // relations avec les éléments du maillage
    private MeshVertex m_Origin;           // sommet situé au début de la demi-arête
    private MeshTriangle m_Triangle;       // triangle à gauche de la demi-arête
    private MeshEdge m_Edge;               // arête contenant cette demi-arête


    /**
     * crée une demi-arête partant d'un sommet
     * @param mesh : maillage d'appartenance de cette demi-arête
     * @param origin : sommet de départ de la demi-arête
     * @param target : sommet de fin de la demi-arête
     * @param triangle : celui situé à gauche de cette demi-arête
     * @throws Exception
     */
    MeshHalfEdge(Mesh mesh, MeshVertex origin, MeshVertex target, MeshTriangle triangle) throws Exception
    {
        // appartenance au maillage
        m_Mesh = mesh;
        if (mesh != null) m_Mesh.pushHalfEdge(this);

        // sommet cible de la demi-arête
        m_Origin   = origin;
        m_Triangle = triangle;
        m_Edge     = null;
        m_Next     = null;
        m_Sibling  = null;

        // vérifier que le sommet d'origine n'a pas déjà une demi-arête allant vers target
        if (origin.getHalfEdgeTo(target) != null) {
            if (mesh == null) {
                throw new Exception("Already a half-edge between " + origin.toString() + " and " + target.toString());
            } else {
                throw new Exception("In " + mesh.getName() + ", already a half-edge between " + origin.toString() + " and " + target.toString());
            }
        }

        // est-ce que le sommet cible contient une demi-arête qui va vers origin ?
        m_Opposite = null;
        MeshHalfEdge other = target.getHalfEdgeTo(origin);
        if (other != null) {
            // vérifier que cette autre demi-arête n'a pas d'opposée (sinon maillage pas manifold)
            if (other.m_Opposite != null) {
                //Log.e(TAG, "other="+other.toString()+" sa cible="<<other.getTarget().toString());
                if (mesh == null) {
                    throw new Exception("Opposite half-edge already reserved between "+origin.toString()+" and "+target.toString());
                } else {
                    throw new Exception("In " + mesh.getName() + ", opposite half-edge already reserved between "+origin.toString()+" and "+target.toString());
                }
            }
            // cette autre arête devient l'opposée de this et réciproquement
            m_Opposite = other;
            other.m_Opposite = this;
            m_Edge = m_Opposite.m_Edge;
        }

        // s'il n'y a pas d'arête, la créer
        if (m_Edge == null) {
            m_Edge = new MeshEdge(mesh, origin, target);
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
    public void destroy()
    {
        destroy(true);
    }
    public void destroy(boolean remove_from_mesh)
    {
        // si cette demi-arête a une opposée, les séparer
        if (m_Opposite != null) {
            m_Opposite.m_Opposite = null;
            m_Opposite = null;
        } else {
            // cette demi-arête était isolée, alors supprimer l'arête
            m_Edge.destroy();
        }
        m_Edge = null;
        m_Next = null;

        // retirer cette demi-arête du chaînage de son origine
        m_Origin.unlinkSibling(this);

        // supprimer cette demi-arête du maillage
        if (m_Mesh != null && remove_from_mesh) m_Mesh.popHalfEdge(this);
        m_Mesh = null;

        // oublier le lien vers le triangle
        m_Triangle = null;
    }


    /**
     * retourne une chaîne représentant cette demi-arête
     * @return nom de la demi-arête
     */
    public String toString()
    {
        if (m_Origin == null) return "MeshHalfEdge(?,?)";
        if (m_Opposite == null || m_Opposite.m_Origin == null) return "MeshHalfEdge("+ m_Origin.getName()+",?)";
        return "MeshHalfEdge("+m_Origin.getName()+","+m_Opposite.m_Origin.getName()+")";
    }


    /**
     * retourne une chaîne représentant cette demi-arête de manière détaillée
     * @return description de la demi-arête
     */
    public String toFullString()
    {
        String result = toString();

        result += ", orig="+((m_Origin != null)?m_Origin.toString():"null");
        result += ", tri="+((m_Triangle != null)?m_Triangle.toString():"null");
        result += ", edge="+((m_Edge != null)?m_Edge.toString():"null");

        result += ", sib="+((m_Sibling != null)?m_Sibling.toString():"null");
        result += ", next="+((m_Next != null)?m_Next.toString():"null");
        result += ", oppo="+((m_Opposite != null)?m_Opposite.toString():"null");

        return result;
    }


    /**
     * retourne le maillage contenant this
     * @return maillage
     */
    public Mesh getMesh()
    {
        return m_Mesh;
    }


    /**
     * modifie le maillage associé à this
     * @param mesh : maillage
     */
    public void setMesh(Mesh mesh)
    {
        m_Mesh = mesh;
    }


    /**
     * retourne le sommet qui est au départ de la demi-arête
     * @return sommet d'origine
     */
    public MeshVertex getOrigin()
    {
        return m_Origin;
    }


    /**
     * retourne le sommet qui est à l'extrémité de la demi-arête
     * @return sommet d'extrémité
     */
    public MeshVertex getTarget()
    {
        if (m_Next == null) return null;
        return m_Next.m_Origin;
    }


    /**
     * retourne la demi-arête opposée (null si aucune)
     * @return demi-arête opposée
     */
    public MeshHalfEdge getOpposite()
    {
        return m_Opposite;
    }


    /**
     * retourne le triangle d'appartenance de cette demi-arête
     * @return triangle
     */
    public MeshTriangle getTriangle()
    {
        return m_Triangle;
    }


    /**
     * retourne l'arête correspondant à cette demi-arête. Cette arête est partagée
     * avec la demi-arête opposée si elle est présente.
     * @return arete
     */
    public MeshEdge getEdge()
    {
        return m_Edge;
    }


    /**
     * retourne la demi-arête suivante dans le triangle
     * @return demi-arête suivante
     */
    public MeshHalfEdge getNext()
    {
        return m_Next;
    }


    /**
     * modifie la demi-arête suivante de this
     * Cette méthode est appelée uniquement par le triangle qui crée cette demi-arête.
     * @param next demi-arête suivante pour tourner autour du triangle
     */
    void setNext(MeshHalfEdge next)
    {
        m_Next = next;
    }


    /**
     * retourne la demi-arête suivante autour du sommet d'origine
     * @return demi-arête suivante
     */
    public MeshHalfEdge getSibling()
    {
        return m_Sibling;
    }


    /**
     * modifie la demi-arête suivante de this autour du sommet d'origine
     * @param sibling demi-arête suivante pour tourner autour du triangle
     */
    public void setSibling(MeshHalfEdge sibling)
    {
        m_Sibling = sibling;
    }


    /**
     * Cette méthode effectue la suppression du triangle associé à cette demi-arête
     * Elle reconstruit une arête, s'il faut, reliant les anciennes deux autres demi-arêtes
     * @param halfedge : demi-arête à supprimer
     */
    private static void collapseTriangle(MeshHalfEdge halfedge)
    {
        // copie des variables membre qui vont disparaître
        Mesh mesh = halfedge.m_Mesh;
        MeshVertex origin = halfedge.m_Origin;

        // sommet opposé à cette demi-arête
        MeshVertex vertex = halfedge.m_Next.m_Next.m_Origin;

        // demi-arête opposées côté "droit" et "gauche"
        MeshHalfEdge oppNN = halfedge.m_Next.m_Next.m_Opposite;
        MeshHalfEdge oppN = halfedge.m_Next.m_Opposite;

        // supprimer les arêtes de ces deux demi-arêtes
        if (oppN != null) oppN.m_Edge.destroy();
        if (oppNN != null) oppNN.m_Edge.destroy();

        // supprimer le triangle contenant cette demi-arête => suppression de ses 3 demi-arêtes
        halfedge.m_Triangle.destroy();

        // reconstruire les demi-arêtes opposées et l'arête commune
        if (oppN != null || oppNN != null) {
            MeshEdge edge = new MeshEdge(mesh, origin, vertex);
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
    public static void collapse(MeshHalfEdge halfedge)
    {
        // copie des variables membre qui vont disparaître
        MeshVertex origin = halfedge.m_Origin;

        // vertex cible qui va être fusionné sur origin
        MeshVertex target = halfedge.getTarget();

        // disparition du triangle sur l'arête opposée, avec échange des demi-arêtes opposées
        if (halfedge.m_Opposite != null) MeshHalfEdge.collapseTriangle(halfedge.m_Opposite);

        // disparition du triangle contenant cette arête avec échange des demi-arêtes opposées
        MeshHalfEdge.collapseTriangle(halfedge);

        // modifier l'origine de toutes les demi-arêtes sortantes entourant target
        MeshHalfEdge other = target.getHalfEdge();
        while (other != null) {
            MeshHalfEdge next_other = other.m_Sibling;
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
    public void flip()
    {
        // demi-arête opposée
        MeshHalfEdge opposite = m_Opposite;
        if (opposite == null) return;

        // autres demi-arêtes des deux triangles
        MeshHalfEdge halfedgenext = m_Next;
        MeshHalfEdge halfedgenextnext = halfedgenext.m_Next;
        MeshHalfEdge oppositenext = opposite.m_Next;
        MeshHalfEdge oppositenextnext = oppositenext.m_Next;

        // modifier le chaînage de la demi-arête this
        m_Next = halfedgenextnext;
        halfedgenextnext.m_Next = oppositenext;
        oppositenext.m_Next = this;

        // modifier le chaînage de la demi-arête opposée
        opposite.m_Next = oppositenextnext;
        oppositenextnext.m_Next = halfedgenext;
        halfedgenext.m_Next = opposite;

        // enlever le chaînage m_Sibling des anciens sommets
        m_Origin.unlinkSibling(this);
        opposite.m_Origin.unlinkSibling(opposite);

        // modifier l'origine de this et son opposée
        m_Origin = oppositenextnext.m_Origin;
        opposite.m_Origin = halfedgenextnext.m_Origin;

        // remettre le chaînage m_Sibling sur les nouveaux sommets
        m_Origin.linkSibling(this);
        opposite.m_Origin.linkSibling(opposite);

        // refaire l'arête
        m_Edge.destroy();
        m_Edge = new MeshEdge(m_Mesh, m_Origin, opposite.m_Origin);
    }
}
