package com.dbooker.livreopengl.mesh;

import java.util.ArrayList;
import java.util.List;

import android.util.Log;

import com.dbooker.livreopengl.glmatrix.vec3;


/**
 * Cette classe représente un maillage 3D : triangles, demi-arêtes et sommets
 */
@SuppressWarnings("unused")
public class Mesh
{
    private static final String TAG = "OpenGL";

    private String m_Name;

    private ArrayList<MeshVertex>   m_VertexList;       // liste des sommets
    private ArrayList<MeshHalfEdge> m_HalfEdgeList;     // liste des demi-arêtes
    private ArrayList<MeshEdge>     m_EdgeList;         // liste des arêtes
    private ArrayList<MeshTriangle> m_TriangleList;     // liste des triangles


    /**
     * constructeur d'un maillage
     * @param name : nom du maillage
     */
    public Mesh(String name)
    {
        init(name);
    }
    public Mesh()
    {
        init("indef");
    }

    private void init(String name)
    {
        // nom
        m_Name = name;
        m_VertexList = new ArrayList<>();
        m_HalfEdgeList = new ArrayList<>();
        m_EdgeList = new ArrayList<>();
        m_TriangleList = new ArrayList<>();
    }


    /**
     * destructeur
     */
    public void destroy()
    {
        // supprimer les triangles
        for (MeshTriangle triangle: m_TriangleList) {
            triangle.destroy(false);
        }
        // supprimer les sommets
        for (MeshVertex vertex: m_VertexList) {
            vertex.destroy(false);
        }
        // supprimer les arêtes qui resteraient
        for (MeshEdge edge: m_EdgeList) {
            edge.destroy(false);
        }
        // supprimer les demi-arêtes qui resteraient
        for (MeshHalfEdge halfedge: m_HalfEdgeList) {
            halfedge.destroy(false);
        }
        // vider les listes définitivement
        m_TriangleList.clear();
        m_VertexList.clear();
        m_EdgeList.clear();
        m_HalfEdgeList.clear();
    }


    /**
     * retourne le nom du maillage
     * @return nom du maillage
     */
    public String getName()
    {
        return m_Name;
    }


    /**
     * retourne la liste des sommets du maillage
     * @return liste des sommets
     */
    public ArrayList<MeshVertex> getVertexList()
    {
        return m_VertexList;
    }


    /**
     * retourne la liste des demi-arêtes du maillage
     * @return liste des demi-arêtes
     */
    public ArrayList<MeshHalfEdge> getHalfEdgeList()
    {
        return m_HalfEdgeList;
    }


    /**
     * retourne la liste des arêtes du maillage
     * @return liste des arêtes
     */
    public ArrayList<MeshEdge> getEdgeList()
    {
        return m_EdgeList;
    }


    /**
     * retourne la liste des triangles du maillage
     * @return liste des triangles
     */
    public ArrayList<MeshTriangle> getTriangleList()
    {
        return m_TriangleList;
    }


    /**
     * retourne le nombre de sommets
     * @return nombre de sommets
     */
    public int getVertexCount()
    {
        return m_VertexList.size();
    }


    /**
     * retourne le nombre de triangles
     * @return nombre de triangles
     */
    public int getTriangleCount()
    {
        return m_TriangleList.size();
    }


    /**
     * retourne le sommet n°i (0..) du maillage, ou null si i n'est pas correct
     * @param i : numéro 0..NV-1 du sommet
     * @return le sommet demandé ou null si i n'est pas dans les bornes
     */
    public MeshVertex getVertex(int i)
    {
        if (i<0 || i>= m_VertexList.size()) return null;
        return m_VertexList.get(i);
    }


    /**
     * Cette fonction retourne le sommet qui porte le nom indiqué
     * NB : cette fonction est très lente dans un grand maillage
     * @param name : nom du vertex à retourner
     * @return vertex ou null
     */
    MeshVertex getVertexByName(String name)
    {
        for (MeshVertex vertex: m_VertexList) {
            if (vertex.getName().equals(name)) return vertex;
        }
        return null;
    }


    /**
     * retourne le triangle n°i (0..) du maillage, ou null si i n'est pas correct
     * @param i : numéro 0..NT-1 du triangle
     * @return le Triangle() demandé ou null si i n'est pas dans les bornes
     */
    public MeshTriangle getTriangle(int i)
    {
        if (i<0 || i>= m_TriangleList.size()) return null;
        return m_TriangleList.get(i);
    }


    /**
     * affiche le nombre de sommets et de triangles sur stdout
     */
    public void info()
    {
        Log.i(TAG, m_Name+": "+m_VertexList.size()+" vertices and "+m_TriangleList.size()+" triangles");
    }


    /**
     * Cette méthode ajoute le sommet fourni à la fin de la liste
     * @param vertex : sommet à rajouter
     */
    public void pushVertex(MeshVertex vertex)
    {
        m_VertexList.add(vertex);
    }


    /**
     * Cette méthode enlève le sommet fourni de la liste
     * NB: la méthode ne le supprime pas (voir son destructeur pour cela)
     * @see #delVertex
     * @param vertex : sommet à enlever
     */
    public void popVertex(MeshVertex vertex)
    {
        m_VertexList.remove(vertex);
    }


    /**
     * Cette méthode ajoute le triangle fourni à la fin de la liste
     * @param triangle à rajouter
     */
    public void pushTriangle(MeshTriangle triangle)
    {
        m_TriangleList.add(triangle);
    }


    /**
     * Cette méthode enlève le triangle fourni de la liste
     * NB: la méthode ne le supprime pas (voir son destructeur pour cela)
     * @see #delTriangle
     * @param triangle à enlever
     */
    public void popTriangle(MeshTriangle triangle)
    {
        m_TriangleList.remove(triangle);
    }


    /**
     * Cette méthode ajoute l'arête fournie à la fin de la liste
     * @param edge : arête à rajouter
     */
    public void pushEdge(MeshEdge edge)
    {
        m_EdgeList.add(edge);
    }


    /**
     * Cette méthode enlève l'arête fournie de la liste
     * NB: la méthode ne le supprime pas (voir son destructeur pour cela)
     * @param edge : arete à enlever
     */
    public void popEdge(MeshEdge edge)
    {
        m_EdgeList.remove(edge);
    }


    /**
     * Cette méthode ajoute la demi-arête fournie à la fin de la liste
     * @param halfedge : demi-arête à rajouter
     */
    public void pushHalfEdge(MeshHalfEdge halfedge)
    {
        m_HalfEdgeList.add(halfedge);
    }


    /**
     * Cette méthode enlève la demi-arête fournie de la liste
     * NB: la méthode ne le supprime pas (voir son destructeur pour cela)
     * @param halfedge : halfedge à enlever
     */
    public void popHalfEdge(MeshHalfEdge halfedge)
    {
        m_HalfEdgeList.remove(halfedge);
    }


    /**
     * Cette méthode crée et rajoute un sommet au maillage
     * @param name : nom du sommet à créer
     * @return le nouveau sommet
     */
    public MeshVertex addVertex(String name)
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
    public MeshTriangle addTriangle(MeshVertex v1, MeshVertex v2, MeshVertex v3) throws Exception
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
    public void addQuad(MeshVertex v1, MeshVertex v2, MeshVertex v3, MeshVertex s4) throws Exception
    {
        addTriangle(v1, v2, s4);
        addTriangle(s4, v2, v3);
    }


    /**
     * Cette méthode crée et rajoute un éventail de triangles au maillage.
     * NB: l'ordre de rotation des sommets est crucial pour le calcul des normales.
     * Il faut tourner dans le sens trigonométrique, comme dans OpenGL.
     * @param vertices : liste des sommets du polygone
     */
    public void addPolygonConvex(List<MeshVertex> vertices) throws Exception
    {
        // créer un éventail autour du premier point fourni
        MeshVertex pivot = vertices.get(0);
        final int count = vertices.size();
        for (int i=0; i<count-2; i++) {
            addTriangle(pivot, vertices.get(i+1), vertices.get(i+2));
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
    public void addPolygon(List<MeshVertex> vertices, vec3 normal) throws Exception
    {
        // vecteurs temporaires
        vec3 AB = vec3.create();
        vec3 BC = vec3.create();
        vec3 CA = vec3.create();
        vec3 N = vec3.create();
        vec3 T = vec3.create();
        vec3 P = vec3.create();

        // tant qu'il reste au moins 3 sommets
        int vertice_count = vertices.size();
        while (vertice_count >= 3) {
            // chercher trois sommets successifs formant une arête convexe
            boolean ok = false;
            for (int i=0; i<vertice_count-2; i++) {
                MeshVertex A = vertices.get((i+0)%vertice_count);
                MeshVertex B = vertices.get((i+1)%vertice_count);
                MeshVertex C = vertices.get((i+2)%vertice_count);
                vec3 cA = A.getCoord();
                vec3 cB = B.getCoord();
                vec3 cC = C.getCoord();
                // est-ce que les trois premiers forment une arête convexe ?
                vec3.subtract(AB, cB, cA);
                vec3.subtract(BC, cC, cB);
                vec3.cross(N, AB, BC);
                // si le produit scalaire entre N et la normale est positif, alors c'est convexe
                if (vec3.dot(N, normal) >= 0.0) {
                    // vérifier qu'aucun des autres points du contour n'est dans ce triangle
                    boolean inside = false;
                    for (int j=0; j<vertice_count; j++) {
                        if (i <= j && j <= i+2) continue;
                        vec3 cP = vertices.get(j).getCoord();
                        // comparaison avec le côté AB
                        vec3.cross(T, AB, N);
                        vec3.subtract(P, cP, cA);
                        boolean lefttoAB = (vec3.dot(P, T) >= 0.0);
                        // comparaison avec le côté BC
                        vec3.cross(T, BC, N);
                        vec3.subtract(P, cP, cB);
                        boolean lefttoBC = (vec3.dot(P, T) >= 0.0);
                        // comparaison avec le côté CA
                        vec3.subtract(CA, cA, cC);
                        vec3.cross(T, CA, N);
                        vec3.subtract(P, cP, cC);
                        boolean lefttoCA = (vec3.dot(P, T) >= 0.0);
                        // dans le futur triangle ?
                        if (lefttoAB && lefttoBC && lefttoCA) {
                            inside = true;
                            break;          // inutile de regarder les autres sommets
                        }
                    }
                    // si aucun point n'est dans le futur triangle, on peut le créer
                    if (! inside) {
                        addTriangle(A, B, C);
                        // enlever B de la liste
                        vertices.remove((i+1)%vertice_count);
                        vertice_count = vertice_count - 1;
                        // on a pu construire au moins un triangle
                        ok = true;
                    }
                }
            }
            // si on n'a pas pu faire un seul triangle, il y a un souci
            if (!ok) throw new Exception("impossible de construire le polygone");
        }
    }


    /**
     * Cette méthode supprime le triangle du maillage en mettant à jour toutes
     * les listes
     * @param triangle : celui qu'il faut supprimer
     */
    public void delTriangle(MeshTriangle triangle)
    {
        triangle.destroy();
    }


    /**
     * Cette méthode supprime le sommet du maillage en mettant à jour toutes
     * les listes : tous les triangles qui le contiennent sont aussi supprimés.
     * @param vertex : celui qu'il faut supprimer
     */
    public void delVertex(MeshVertex vertex)
    {
        vertex.destroy();
    }


    /**
     * Cette méthode recalcule les normales des triangles et sommets.
     * Les normales des triangles sont calculées d'après leurs côtés.
     * Les normales des sommets sont les moyennes des normales des triangles
     * auxquels ils appartiennent.
     */
    public void computeNormals()
    {
        // calculer les normales des triangles
        for (MeshTriangle triangle: m_TriangleList) {
            triangle.computeNormal();
        }

        // calculer les normales des sommets
        short iv = 0;
        for (MeshVertex vertex: m_VertexList) {
            // renuméroter le sommet (numéro dans les VBOs)
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
    public void computeTangents()
    {
        // calculer les tangentes des triangles
        for (MeshTriangle triangle: m_TriangleList) {
            triangle.computeTangent();
        }

        // calculer les tangentes des sommets
        for (MeshVertex vertex: m_VertexList) {
            // recalculer la tangente de ce sommet
            vertex.computeTangent();
        }
    }
}
