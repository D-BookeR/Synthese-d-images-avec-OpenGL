package com.dbooker.livreopengl.mesh;

import com.dbooker.livreopengl.glmatrix.vec3;

import java.util.Iterator;


@SuppressWarnings("unused")
public class MeshTriangle
{
    // appartenance au maillage
    private Mesh m_Mesh;
    private MeshHalfEdge m_HalfEdge;

    // attributs de triangle
    private vec3 m_Normal;
    private vec3 m_Tangent;
    private vec3 m_Center;
    private float m_Surface;
    private float m_W;

    /**
     * Constructeur de la classe Triangle à partir de trois sommets
     * NB: l'ordre de rotation des sommets est crucial pour le calcul des normales.
     * Il faut tourner dans le sens trigonométrique, comme dans OpenGL.
     * @param mesh : maillage dans lequel on rajoute ce triangle
     * @param v1 : l'un des coins du triangle, null si création manuelle
     * @param v2 : l'un des coins du triangle, null si création manuelle
     * @param v3 : l'un des coins du triangle, null si création manuelle
     */
    public MeshTriangle(Mesh mesh, MeshVertex v1, MeshVertex v2, MeshVertex v3) throws Exception
    {
        // appartenance au maillage
        m_Mesh = mesh;
        if (mesh != null) m_Mesh.pushTriangle(this);

        // créer trois demi-arêtes sauf si les sommets sont nuls
        if (v1 != null && v1 != null && v3 != null) {
            MeshHalfEdge he12 = new MeshHalfEdge(mesh, v1, v2, this);
            MeshHalfEdge he23 = new MeshHalfEdge(mesh, v2, v3, this);
            MeshHalfEdge he31 = new MeshHalfEdge(mesh, v3, v1, this);

            // chaîner ces demi-arêtes
            he12.setNext(he23);
            he23.setNext(he31);
            he31.setNext(he12);

            // le triangle est constitué de cette chaîne
            m_HalfEdge = he12;
        } else {
            m_HalfEdge = null;
        }

        // seront initialisées dans calcNormale()
        m_Normal  = vec3.create();
        m_Tangent = vec3.create();
        m_Center  = vec3.create();
        m_Surface = 0.0f;
        m_W       = 0.0f;
    }

    public MeshTriangle(Mesh mesh) throws Exception
    {
        // appartenance au maillage
        m_Mesh = mesh;
        if (mesh != null) m_Mesh.pushTriangle(this);

        // pas de demi-arête (création manuelle)
        m_HalfEdge = null;

        // seront initialisées dans calcNormale()
        m_Normal  = vec3.create();
        m_Tangent = vec3.create();
        m_Center  = vec3.create();
        m_Surface = 0.0f;
        m_W       = 0.0f;
    }


    /**
     * Cette méthode supprime ce triangle du maillage en mettant à jour toutes
     * les listes. Cela peut supprimer des arêtes et rendre des sommets isolés.
     */
    public void destroy()
    {
        destroy(true);
    }
    public void destroy(boolean remove_from_mesh)
    {
        // supprimer ce triangle de la liste du maillage
        if (m_Mesh != null && remove_from_mesh) m_Mesh.popTriangle(this);
        m_Mesh = null;

        // supprimer les trois demi-arêtes de ce triangle
        if (m_HalfEdge != null) {
            m_HalfEdge.getNext().getNext().destroy();
            m_HalfEdge.getNext().destroy();
            m_HalfEdge.destroy();
        }
        m_HalfEdge = null;
    }


    /**
     * retourne une chaîne qui décrit le triangle
     * @return nom du triangle
     */
    public String toString()
    {
        // récupérer les trois sommets
        MeshVertex A = getVertex0();
        String nA = (A == null) ? "?" : A.getName();
        MeshVertex B = getVertex1();
        String nB = (B == null) ? "?" : B.getName();
        MeshVertex C = getVertex2();
        String nC = (C == null) ? "?" : C.getName();

        // chaîne mentionnant les noms des sommets
        return "Triangle("+nA+","+nB+","+nC+")";
    }


    /**
     * retourne une chaîne qui décrit le triangle de manière détaillée
     * @return description du triangle
     */
    public String toFullString()
    {
        if (m_HalfEdge != null) {
            return toString()+" -> "+m_HalfEdge.toString();
        } else {
            return toString();
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
     * retourne les sommets du triangle sous forme d'un itérable
     * @return itérable sur les trois sommets du triangle
     */
    Iterable<MeshVertex> getVertexList()
    {
        return new Iterable<MeshVertex>() {
            @Override
            public Iterator<MeshVertex> iterator() {
                return new Iterator<MeshVertex>() {

                    private int i = 3;                           // nombre de sommets restant à retourner
                    private MeshHalfEdge halfedge = m_HalfEdge;  // demi-arête courante

                    @Override
                    public boolean hasNext() {
                        return i>0;
                    }

                    @Override
                    public MeshVertex next() {
                        halfedge = halfedge.getNext();
                        i--;
                        return halfedge.getOrigin();
                    }

                    @Override
                    public void remove() {
                        throw new UnsupportedOperationException();
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
    MeshVertex getVertex(int n)
    {
        if (n<0 || n>2) return null;

        MeshHalfEdge halfedge = m_HalfEdge;
        for (int i=0; i<n; i++) {
            halfedge = halfedge.getNext();
        }
        return halfedge.getOrigin();
    }


    /**
     * retourne la demi-arête du triangle
     * @return demi-arête
     */
    public MeshHalfEdge getHalfEdge()
    {
        return m_HalfEdge;
    }


    /**
     * retourne le premier sommet du triangle
     * @return premier sommet
     */
    public MeshVertex getVertex0()
    {
        if (m_HalfEdge == null) return null;
        return m_HalfEdge.getOrigin();
    }


    /**
     * retourne le deuxième sommet du triangle
     * @return deuxième sommet
     */
    public MeshVertex getVertex1()
    {
        if (m_HalfEdge == null) return null;
        if (m_HalfEdge.getNext() == null) return null;
        return m_HalfEdge.getNext().getOrigin();
    }


    /**
     * retourne le troisième sommet du triangle
     * @return troisième sommet
     */
    public MeshVertex getVertex2()
    {
        if (m_HalfEdge == null) return null;
        if (m_HalfEdge.getNext() == null) return null;
        if (m_HalfEdge.getNext().getNext() == null) return null;
        return m_HalfEdge.getNext().getNext().getOrigin();
    }


    /**
     * retourne la valeur de la normale
     * @see #computeNormal pour la calculer auparavant
     * @return normale du triangle
     */
    public vec3 getNormal()
    {
        return m_Normal;
    }


    /**
     * retourne la valeur de la surface
     * @see #computeNormal pour la calculer auparavant
     * @return surface du triangle
     */
    public float getSurface()
    {
        return m_Surface;
    }


    /**
     * retourne le centre du triangle
     * @see #computeNormal pour le calculer auparavant
     * @return centre du triangle
     */
    public vec3 getCenter()
    {
        return m_Center;
    }


    /**
     * retourne la valeur w du triangle
     * @return w
     */
    public float getW()
    {
        return m_W;
    }


    /**
     * affecte la variable w du triangle
     * @param w variable
     */
    public void setW(float w)
    {
        m_W = w;
    }


    /**
     * recalcule les informations géométriques du triangle : centre, normale, surface...
     */
    void computeNormal()
    {
        // les trois sommets
        MeshVertex A = m_HalfEdge.getOrigin();
        MeshVertex B = m_HalfEdge.getNext().getOrigin();
        MeshVertex C = m_HalfEdge.getNext().getNext().getOrigin();

        // les coordonnées des trois sommets
        vec3 cA = A.getCoord();
        vec3 cB = B.getCoord();
        vec3 cC = C.getCoord();

        // vecteurs AB et AC
        vec3 cAB = vec3.create();
        vec3.subtract(cAB, cB, cA);
        vec3 cAC = vec3.create();
        vec3.subtract(cAC, cC, cA);

        // calculer les coordonnées du centre du triangle
        vec3.zero(m_Center);
        vec3.add(m_Center, cA, cB);
        vec3.add(m_Center, m_Center, cC);
        vec3.scale(m_Center, m_Center, 1.0f/3.0f);

        // calculer le vecteur normal
        vec3.cross(m_Normal, cAB, cAC);

        // surface et coefficient W
        m_Surface = 0.5f * vec3.length(m_Normal);
        vec3.normalize(m_Normal, m_Normal);
        m_W = -vec3.dot(m_Normal, cA);
    }


    /**
     * retourne la valeur de la tangente
     * @see #computeTangent pour la calculer auparavant
     * @return tangente du triangle
     */
    public vec3 getTangent()
    {
        return m_Tangent;
    }


    /**
     * recalcule la tangente du triangle à l'aide de la normale et des coordonnées de texture
     */
    void computeTangent()
    {
        // les trois sommets
        MeshVertex A = m_HalfEdge.getOrigin();
        MeshVertex B = m_HalfEdge.getNext().getOrigin();
        MeshVertex C = m_HalfEdge.getNext().getNext().getOrigin();

        // les coordonnées des trois sommets
        vec3 cA = A.getCoord();
        vec3 cB = B.getCoord();
        vec3 cC = C.getCoord();

        // vecteurs AB et AC
        vec3 cAB = vec3.create();
        vec3.subtract(cAB, cB, cA);
        vec3 cAC = vec3.create();
        vec3.subtract(cAC, cC, cA);

        // récupération de leur 2e coordonnée de texture
        float tA = A.getTexCoord().get(1);
        float tB = B.getTexCoord().get(1);
        float tC = C.getTexCoord().get(1);

        // vecteurs dans l'espace (s,t), et uniquement la coordonnée t
        float tAB = tB - tA;
        float tAC = tC - tA;

        // TODO s'il n'y a pas de coordonnées de texture, alors tAB et tAC sont nuls, les remplacer par AB et AC

        // calcul de la tangente
        vec3.scale(cAB, cAB, tAC);
        vec3.scale(cAC, cAC, tAB);
        vec3.subtract(m_Tangent, cAB, cAC);

        // normalisation
        vec3.normalize(m_Tangent, m_Tangent);
    }


    /**
     * Cette méthode fait tourner les sommets du triangle afin que
     * celui qui est passé en paramètre se trouve en premier
     * @param vertex : sommet à mettre en tête de liste (sauf s'il en est absent)
     * @return true si ok, false si le sommet est absent du triangle
     */
    boolean cycleVertexFirst(MeshVertex vertex)
    {
        // est-ce que sommet = origine de la première demi-arête ?
        if (m_HalfEdge.getOrigin() == vertex) return true;

        // est-ce que sommet = origine de la deuxième demi-arête ?
        m_HalfEdge = m_HalfEdge.getNext();
        if (m_HalfEdge.getOrigin() == vertex) return true;

        // est-ce que sommet = origine de la troisième demi-arête ?
        m_HalfEdge = m_HalfEdge.getNext();
        if (m_HalfEdge.getOrigin() == vertex) return true;

        // échec, le sommet n'est origine d'aucune demi-arête
        return false;
    }


    /**
     * Cette méthode indique si le triangle this contient le sommet indiqué
     * @param vertex : sommet dont il faut vérifier l'appartenance à this
     * @return true si ok, false si le sommet est absent du triangle
     */
    boolean containsVertex(MeshVertex vertex)
    {
        if (m_HalfEdge.getOrigin() == vertex) return true;
        if (m_HalfEdge.getNext().getOrigin() == vertex) return true;
        if (m_HalfEdge.getNext().getNext().getOrigin() == vertex) return true;
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
    boolean containsEdge(MeshVertex vertex1, MeshVertex vertex2)
    {
        if (! cycleVertexFirst(vertex1)) return false;
        return m_HalfEdge.getNext().getOrigin() == vertex2;
    }


    /**
     * remplace un sommet par un autre dans ce triangle
     * @param A sommet à remplacer
     * @param A1 sommet à mettre à la place
     */
    public void replaceVertex(MeshVertex A, MeshVertex A1) throws Exception
    {
        // mettre A en premier
        if (! cycleVertexFirst(A)) return;

        // les deux autres sommets actuels du triangle
        MeshVertex B = getVertex1();
        MeshVertex C = getVertex2();

        // demi-arêtes du triangle
        MeshHalfEdge daAB = m_HalfEdge;
        MeshHalfEdge daBC = daAB.getNext();
        MeshHalfEdge daCA = daBC.getNext();

        // supprimer les deux demi-arêtes AB et CA
        daAB.destroy();
        daCA.destroy();
        daBC.setNext(null);

        // créer deux demi-arêtes allant de et vers A1
        daAB = new MeshHalfEdge(m_Mesh, A1, B, this);
        daCA = new MeshHalfEdge(m_Mesh, C, A1, this);

        // chaîner ces demi-arêtes
        daAB.setNext(daBC);
        daBC.setNext(daCA);
        daCA.setNext(daAB);

        // le triangle est constitué de cette chaîne
        m_HalfEdge = daAB;
    }
}
