package com.dbooker.livreopengl.mesh;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import com.dbooker.livreopengl.glmatrix.vec2;
import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.glmatrix.vec4;
import com.dbooker.livreopengl.mesh.MeshModuleRedux.Quadric;



/**
 * Cette classe représente l'un des sommets d'un maillage
 */
@SuppressWarnings("unused")
public class MeshVertex
{
    private static final String TAG = "OpenGL";

    // attributs de sommet
    private String m_Name;
    private ArrayList<vec4> m_Attributes;

    // appartenance au maillage
    private Mesh m_Mesh;
    private MeshHalfEdge m_HalfEdge;       // demi-arête sortant de ce sommet

    // indice du vertex dans les VBO
    private int m_Number;

    /* ajouts pour le module MeshModuleRedux */
    private Quadric m_Quadric;
    private MeshVertex m_CollapseTarget;
    private float m_CollapseCost;


    /**
     * Constructeur de la classe MeshVertex qui représente un sommet dans
     * le maillage. Initialise un sommet non connecté au maillage, employer
     * Mesh.addVertex pour l'insérer puis new Triangle(...) pour le mettre
     * dans un triangle, setCoord et setColor pour lui donner des coordonnées et
     * des couleurs.
     * @param mesh : maillage d'appartenance de ce sommet
     * @param name : nom du sommet à créer
     */
    public MeshVertex(Mesh mesh, String name)
    {
        // appartenance au maillage
        m_Mesh = mesh;
        if (mesh != null) {
            m_Mesh.pushVertex(this);

            // numéro du vertex dans les VBO
            m_Number = m_Mesh.getVertexCount();
        }

        // nom du sommet
        m_Name = name;

        // attributs du sommet, voir setCoord(), setColor(), setNormal()...
        m_Attributes = new ArrayList<>();
        m_Attributes.add(vec4.create());      // ID_ATTR_VERTEX
        m_Attributes.add(vec4.create());      // ID_ATTR_COLOR
        m_Attributes.add(vec4.create());      // ID_ATTR_NORMAL
        m_Attributes.add(vec4.create());      // ID_ATTR_TANGENT
        m_Attributes.add(vec4.create());      // ID_ATTR_TEXCOORD
        m_Attributes.add(vec4.create());      // ID_ATTR_VERTEX1
        m_Attributes.add(vec4.create());      // ID_ATTR_NORMAL1
        m_Attributes.add(vec4.create());      // ID_ATTR_IDBONES
        m_Attributes.add(vec4.create());      // ID_ATTR_WEIGHTS

        // demi-arête
        m_HalfEdge = null;

        // divers
        m_Quadric = null;
    }


    /** destructeur */
    public void destroy()
    {
        destroy(true);
    }
    public void destroy(boolean remove_from_mesh)
    {
        // supprimer ce sommet de la liste du maillage
        if (m_Mesh != null && remove_from_mesh) m_Mesh.popVertex(this);
        m_Mesh = null;

        // supprimer la quadrique si elle existe
        if (m_Quadric != null) m_Quadric.destroy();

        // enlever tous les triangles autour de ce sommet
        MeshHalfEdge halfedge = m_HalfEdge;
        while (halfedge != null) {
            MeshHalfEdge next = halfedge.getSibling();
            MeshTriangle triangle = halfedge.getTriangle();
            triangle.destroy();
            halfedge = next;
        }
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
     * retourne la première MeshHalfEdge de this
     * @return MeshHalfEdge
     */
    public MeshHalfEdge getHalfEdge()
    {
        return m_HalfEdge;
    }


    /**
     * cette méthode rajoute la demi-arête dans le chaînage m_Sibling (demi-arêtes autour de this)
     * @see #unlinkSibling qui fait l'inverse
     * @param halfedge à rajouter dans la liste m_Sibling de this
     */
    public void linkSibling(MeshHalfEdge halfedge)
    {
        // mettre la demi-arête en tête de la liste
        halfedge.setSibling(m_HalfEdge);
        m_HalfEdge = halfedge;
    }


    /**
     * cette méthode retire cette demi-arête de la liste m_Sibling (demi-arêtes autour de this)
     * @see #linkSibling qui fait l'inverse
     * @param halfedge à enlever de la liste m_Sibling de this
     */
    public void unlinkSibling(MeshHalfEdge halfedge)
    {
        // si le sommet this désigne la demi-arête, alors le faire désigner la suivante
        if (m_HalfEdge == halfedge) {
            // this doit maintenant désigner la demi-arête suivante dans sa liste
            m_HalfEdge = m_HalfEdge.getSibling();
            // si m_HalfEdge==null, alors le sommet this devient libre (supprimable)
        } else {
            // le sommet désigne une autre demi-arête, il faut parcourir le chaînage
            MeshHalfEdge other = m_HalfEdge;
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
    List<MeshTriangle> getTrianglesAround()
    {
        List<MeshTriangle> triangles = new ArrayList<>();
        MeshHalfEdge halfedge = m_HalfEdge;  // demi-arête courante

        while (halfedge != null) {
            MeshTriangle triangle = halfedge.getTriangle();
            triangles.add(triangle);
            halfedge = halfedge.getSibling();
        }
        return triangles;
    }


    /**
     * retourne la liste des triangles autour du sommet this
     * Cette liste est basée sur les liens m_Next et m_Opposite entre triangles
     * NB : la liste est triée par ordre autour du sommet, mais si le maillage n'est
     * pas complet, alors il manquera une partie des triangles
     * @see #getTrianglesAround pour une liste non triée mais complète
     * @return liste des triangles
     */
    List<MeshTriangle> getTrianglesOrderedAround()
    {
        List<MeshTriangle> triangles = new ArrayList<>();
        MeshHalfEdge halfedge = m_HalfEdge;   // demi-arête courante
        MeshHalfEdge first    = m_HalfEdge;   // première demi-arête vue

        while (halfedge != null) {
            MeshTriangle triangle = halfedge.getTriangle();
            triangles.add(triangle);
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
    Set<MeshVertex> getNeighborVertices()
    {
        // collection retournée par la méthode
        Set<MeshVertex> neighbors = new HashSet<>();

        // parcourir les demi-arêtes autour de this
        MeshHalfEdge trianglehalfedge = m_HalfEdge;
        while (trianglehalfedge != null) {
            MeshTriangle triangle = trianglehalfedge.getTriangle();

            // ajouter les trois sommets du triangle sauf s'ils y sont déjà
            MeshHalfEdge vertexhalfedge = triangle.getHalfEdge();
            for (int i=0; i<3; i++) {
                if (vertexhalfedge.getOrigin() != this) {
                    neighbors.add(vertexhalfedge.getOrigin());
                }
                // passer à la demi-arête suivante sur ce triangle
                vertexhalfedge = vertexhalfedge.getNext();
            }

            // passer à la demi-arête suivante autour de this
            trianglehalfedge = trianglehalfedge.getSibling();
        }

        return neighbors;
    }


    /**
     * retourne la demi-arête allant de this à vertex
     * @param vertex : destination de la demi-arête recherchée
     * @return demi-arête allant de this à vertex, ou null si absente
     */
    public MeshHalfEdge getHalfEdgeTo(MeshVertex vertex)
    {
        MeshHalfEdge halfedge = m_HalfEdge;
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
    MeshTriangle getTriangleRightTo(MeshVertex vertex)
    {
        // chercher la demi-arête entre this et vertex
        MeshHalfEdge halfedge = getHalfEdgeTo(vertex);
        if (halfedge == null || halfedge.getOpposite() == null) return null;
        return halfedge.getOpposite().getTriangle();
    }


    /**
     * retourne le triangle situé à gauche de l'arête allant de this à vertex
     * @param vertex : second sommet
     * @return triangle ou null si aucun
     */
    MeshTriangle getTriangleLeftTo(MeshVertex vertex)
    {
        // chercher la demi-arête entre this et vertex
        MeshHalfEdge halfedge = getHalfEdgeTo(vertex);
        if (halfedge == null) return null;
        return halfedge.getTriangle();
    }


    /**
     * retourne le nom du sommet
     * @return le nom du sommet
     */
    public String toString()
    {
        return m_Name;
    }


    /**
     * retourne une chaîne représentant ce sommet de manière détaillée
     * @return description du sommet
     */
    public String toFullString()
    {
        String result = toString();
        MeshHalfEdge halfedge = m_HalfEdge;
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
    public String getName()
    {
        return m_Name;
    }


    /**
     * modifie le numéro du vertex
     * @param number : numero du sommet
     */
    public void setNumber(int number)
    {
        m_Number = number;
    }


    /**
     * retourne le numéro du vertex
     * @return numéro du sommet
     */
    public int getNumber()
    {
        return m_Number;
    }


    /**
     * définit les coordonnées du sommet
     * @param xyz coordonnées
     * @return this pour pouvoir chaîner les affectations
     */
    public MeshVertex setCoord(vec3 xyz)
    {
        vec3.copy(vec3.cast(m_Attributes.get(ID_ATTR_VERTEX)), xyz);
        m_Attributes.get(MeshVertex.ID_ATTR_VERTEX).set(3, 1.0f);
        return this;
    }


    /**
     * retourne les coordonnées du sommet
     * @return coordonnées 3D du sommet
     */
    public vec3 getCoord()
    {
        return vec3.cast(m_Attributes.get(ID_ATTR_VERTEX));
    }


    /**
     * définit la couleur du sommet
     * @param rgba couleur (r,g,b,a)
     * @return this pour pouvoir chaîner les affectations
     */
    public MeshVertex setColor(vec4 rgba)
    {
        vec4.copy(m_Attributes.get(ID_ATTR_COLOR), rgba);
        return this;
    }
    /**
     * définit la couleur du sommet
     * @param rgb couleur (r,g,b)
     * @return this pour pouvoir chaîner les affectations
     */
    public MeshVertex setColor(vec3 rgb)
    {
        vec4.set(m_Attributes.get(ID_ATTR_COLOR), rgb.get(0), rgb.get(1), rgb.get(2), 1.0f);
        return this;
    }


    /**
     * retourne la couleur du sommet
     * @return couleur (r,g,b,a)
     */
    public vec4 getColor()
    {
        return m_Attributes.get(ID_ATTR_COLOR);
    }


    /**
     * définit les coordonnées de la normale du sommet
     * @param normal : normale à affecter
     * @return this pour pouvoir chaîner les affectations
     */
    public MeshVertex setNormal(vec3 normal)
    {
        vec3.copy(vec3.cast(m_Attributes.get(ID_ATTR_NORMAL)), normal);
        return this;
    }


    /**
     * retourne la normale du sommet
     * @return normale
     */
    public vec3 getNormal()
    {
        return vec3.cast(m_Attributes.get(ID_ATTR_NORMAL));
    }


    /**
     * retourne la tangente du sommet
     * @return tangente
     */
    public vec3 getTangent()
    {
        return vec3.cast(m_Attributes.get(ID_ATTR_TANGENT));
    }


    /**
     * définit les coordonnées de texture du sommet
     * @param uv coordonnées de texture
     * @return this pour pouvoir chaîner les affectations
     */
    public MeshVertex setTexCoord(vec2 uv)
    {
        vec2.copy(vec2.cast(m_Attributes.get(ID_ATTR_TEXCOORD)), uv);
        return this;
    }


    /**
     * retourne les coordonnées de texture du sommet
     * @return coordonnées de texture
     */
    public vec2 getTexCoord()
    {
        return vec2.cast(m_Attributes.get(ID_ATTR_TEXCOORD));
    }


    /**
     * retourne l'attribut dont on fournit l'identifiant
     * @param idattr : numéro de l'attribut, ex: MeshVertex.ID_ATTR_VERTEX
     * @return vec4 contenant l'attribut
     */
    public vec4 getAttribute(int idattr)
    {
        return m_Attributes.get(idattr);
    }


    /**
     * affecte ou définit l'attribut dont on fournit l'identifiant
     * @param idattr : numéro de l'attribut, ex: MeshVertex.ID_ATTR_VERTEX
     * @param value : valeur à affecter à l'attribut
     * @return this pour pouvoir chaîner les affectations
     */
    public MeshVertex setAttribute(int idattr, vec4 value)
    {
        vec4.copy(m_Attributes.get(idattr), value);
        return this;
    }
    public MeshVertex setAttribute(int idattr, vec3 value)
    {
        vec3.copy(vec3.cast(m_Attributes.get(idattr)), value);
        return this;
    }
    public MeshVertex setAttribute(int idattr, vec2 value)
    {
        vec2.copy(vec2.cast(m_Attributes.get(idattr)), value);
        return this;
    }


    /**
     * Cette méthode calcule la normale du sommet = moyenne des normales des
     * triangles contenant ce sommet.
     */
    void computeNormal()
    {
        // calculer la moyenne des normales des triangles contenant ce sommet
        vec3.zero(vec3.cast(m_Attributes.get(ID_ATTR_NORMAL)));
        MeshHalfEdge halfedge = m_HalfEdge;
        while (halfedge != null) {
            MeshTriangle triangle = halfedge.getTriangle();
            // normale du triangle courant
            vec3 normal = vec3.clone(triangle.getNormal());
            // prendre en compte la surface du triangle
            vec3.scale(normal, normal, triangle.getSurface());
            // cumuler les normales pondérées
            vec3.add(vec3.cast(m_Attributes.get(ID_ATTR_NORMAL)), vec3.cast(m_Attributes.get(ID_ATTR_NORMAL)), normal);
            // passer à la demi-arête suivante autour de ce sommet
            halfedge = halfedge.getSibling();
       }
        // normaliser le résultat
        vec3.normalize(vec3.cast(m_Attributes.get(ID_ATTR_NORMAL)), vec3.cast(m_Attributes.get(ID_ATTR_NORMAL)));
    }


    /**
     * Cette méthode calcule la tangente du sommet = moyenne des tangentes des
     * triangles contenant ce sommet.
     */
    void computeTangent()
    {
        // calculer la moyenne des tangentes des triangles contenant ce sommet
        vec3.zero(vec3.cast(m_Attributes.get(ID_ATTR_TANGENT)));
        MeshHalfEdge halfedge = m_HalfEdge;
        while (halfedge != null) {
            MeshTriangle triangle = halfedge.getTriangle();
            // tangente du triangle courant
            vec3 tangent = vec3.clone(triangle.getTangent());
            // prendre en compte la surface du triangle
            vec3.scale(tangent, tangent, triangle.getSurface());
            // cumuler les tangentes pondérées
            vec3.add(vec3.cast(m_Attributes.get(ID_ATTR_TANGENT)), vec3.cast(m_Attributes.get(ID_ATTR_TANGENT)), tangent);
            // passer à la demi-arête suivante autour de ce sommet
            halfedge = halfedge.getSibling();
        }
        // normaliser le résultat
        vec3.normalize(vec3.cast(m_Attributes.get(ID_ATTR_TANGENT)), vec3.cast(m_Attributes.get(ID_ATTR_TANGENT)));
    }


    /**
     * retourne le sommet visé pour une fusion
     * @see MeshModuleRedux
     * @return sommet
     */
    public MeshVertex getCollapseTarget()
    {
        return m_CollapseTarget;
    }


    /**
     * modifie le sommet visé pour une fusion
     * @see MeshModuleRedux
     * @param vertex : sommet à affecter
     */
    public void setCollapseTarget(MeshVertex vertex)
    {
        m_CollapseTarget = vertex;
    }


    /**
     * retourne le coût d'une fusion vers m_CollapseTarget
     * @see MeshModuleRedux
     * @return coût
     */
    public float getCollapseCost()
    {
        return m_CollapseCost;
    }


    /**
     * modifie le coût d'une fusion vers m_CollapseTarget
     * @see MeshModuleRedux
     * @param cost : coût à affecter
     */
    public void setCollapseCost(float cost)
    {
        m_CollapseCost = cost;
    }


    /**
     * retourne la quadrique permettant de calculer le coût d'une réduction
     * @see MeshModuleRedux
     * @return quadrique
     */
    public Quadric getQuadric()
    {
        return m_Quadric;
    }


    /**
     * modifie la quadrique permettant de calculer le coût d'une réduction
     * @see MeshModuleRedux
     * @param quadric : quadrique à affecter
     */
    public void setQuadric(Quadric quadric)
    {
        m_Quadric = quadric;
    }


    /**
     * retourne une copie de ce sommet (this)
     * @param suffix : chaine à rajouter au nom de this
     * @return copie du sommet this
     */
    public MeshVertex clone(String suffix)
    {
        MeshVertex copie = new MeshVertex(m_Mesh, m_Name+suffix);
        // recopier les attributs du vertex
        for (int i=0; i<m_Attributes.size(); i++) {
            vec4.copy(copie.m_Attributes.get(i), m_Attributes.get(i));
        }
        return copie;
    }


    /**
     * Cette méthode modifie les attributs du vertex pour qu'ils soient interpolés
     * linéairement entre ceux de v0 et de v1, d'un facteur k
     * @param v0 : premier vertex, (k=0)
     * @param v1 : second vertex (k=1)
     * @param k : valeur entre 0 et 1 pour l'interpolation
     */
    void lerp(MeshVertex v0, MeshVertex v1, float k)
    {
        // interpoler les attributs
        for (int i=0; i<m_Attributes.size(); i++) {
            vec4.lerp(m_Attributes.get(i), v0.m_Attributes.get(i), v1.m_Attributes.get(i), k);
        }
        // normaliser les vecteurs
        vec3.normalize(vec3.cast(m_Attributes.get(ID_ATTR_NORMAL)),  vec3.cast(m_Attributes.get(ID_ATTR_NORMAL)));
        vec3.normalize(vec3.cast(m_Attributes.get(ID_ATTR_TANGENT)), vec3.cast(m_Attributes.get(ID_ATTR_TANGENT)));
        vec3.normalize(vec3.cast(m_Attributes.get(ID_ATTR_NORMAL1)), vec3.cast(m_Attributes.get(ID_ATTR_NORMAL1)));
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
    void hermite(MeshVertex v0, vec3 t0, MeshVertex v1, vec3 t1, float k)
    {
        // interpoler les coordonnées par hermite
        vec3.hermite(vec3.cast(m_Attributes.get(ID_ATTR_VERTEX)),
            vec3.cast(v0.m_Attributes.get(ID_ATTR_VERTEX)), t0,
            vec3.cast(v1.m_Attributes.get(ID_ATTR_VERTEX)), t1,
            k);
        // interpoler linéairement les autres attributs (il faudrait leurs dérivées)
        for (int i=1; i<m_Attributes.size(); i++) {
            vec4.lerp(m_Attributes.get(i), v0.m_Attributes.get(i), v1.m_Attributes.get(i), k);
        }
        // normaliser les vecteurs
        vec3.normalize(vec3.cast(m_Attributes.get(ID_ATTR_NORMAL)), vec3.cast(m_Attributes.get(ID_ATTR_NORMAL)));
        vec3.normalize(vec3.cast(m_Attributes.get(ID_ATTR_TANGENT)), vec3.cast(m_Attributes.get(ID_ATTR_TANGENT)));
        vec3.normalize(vec3.cast(m_Attributes.get(ID_ATTR_NORMAL1)), vec3.cast(m_Attributes.get(ID_ATTR_NORMAL1)));
    }


    /**
     * Constantes prédéfinies pour les 4 attributs de base d'un vertex
     * voir MeshVertex.getAttribut, c'est le code qu'il faut pour idattr
     */
    public static final int ID_ATTR_VERTEX     = 0;
    public static final int ID_ATTR_COLOR      = 1;
    public static final int ID_ATTR_NORMAL     = 2;
    public static final int ID_ATTR_TANGENT    = 3;
    public static final int ID_ATTR_TEXCOORD   = 4;
    public static final int ID_ATTR_VERTEX1    = 5;
    public static final int ID_ATTR_NORMAL1    = 6;
    public static final int ID_ATTR_IDBONES    = 7;
    public static final int ID_ATTR_WEIGHTS    = 8;
}
