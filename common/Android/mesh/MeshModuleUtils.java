package com.dbooker.livreopengl.mesh;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Set;

import android.util.Log;

import com.dbooker.livreopengl.glmatrix.vec3;


/**
 * Ce module rajoute des fonctions diverses pour les maillages
 *
 */


@SuppressWarnings("unused")
public class MeshModuleUtils extends MeshModule
{
    /**
     * initialise le module sans maillage
     */
    public MeshModuleUtils()
    {
        super();
    }


    /**
     * initialise le module sur le maillage fourni
     * @param mesh : maillage concerné
     */
    public MeshModuleUtils(Mesh mesh)
    {
        super(mesh);
    }


    /** destructeur */
    public void destroy()
    {
        super.destroy();
    }


    /**
     * Cette méthode produit le nom du milieu entre p1 et p2, c'est la concaténation
     * du nom de p1 et de p2 mais dans l'ordre lexicographique afin que
     * nomMilieu(p1, p2) == nomMilieu(p2, p1) quels que soient p1 et p2
     * @param p1 : vertex
     * @param p2 : vertex
     * @return "m" + nom du plus petit sommet + nom du plus grand
     */
    public static String getMidName(MeshVertex p1, MeshVertex p2)
    {
        // définir le nom du milieu à partir des noms de p1 et p2
        if (p1.getName().compareTo(p2.getName()) < 0) {
            // p1 a le plus petit nom dans l'ordre lexicographique
            return "m"+p1.getName()+p2.getName();
        } else {
            // p2 a le plus petit nom dans l'ordre lexicographique
            return "m"+p2.getName()+p1.getName();
        }
    }


    /**
     * calcule la moyenne des normales des triangles fournis
     * @param triangles : liste des triangles à considérer
     * @return moyenne des normales
     */
    public static vec3 averageNormals(Collection<MeshTriangle> triangles)
    {
        // accumuler les normales puis normaliser
        vec3 sum = vec3.create();
        for (MeshTriangle triangle: triangles) {
            // normale du triangle courant
            vec3 n = vec3.clone(triangle.getNormal());
            // prendre en compte la surface du triangle
            vec3.scale(n, n, triangle.getSurface());
            // cumuler les normales pondérées
            vec3.add(sum, sum, n);
        }
        // normaliser le résultat
        vec3.normalize(sum, sum);
        return sum;
    }


    /**
     * calcule la moyenne des tangentes des triangles fournis
     * @param triangles : liste des triangles à considérer
     * @return moyenne des tangentes
     */
    public static vec3 averageTangents(Collection<MeshTriangle> triangles)
    {
        // accumuler les tangentes puis normaliser
        vec3 sum = vec3.create();
        for (MeshTriangle triangle: triangles) {
            // tangente du triangle courant
            vec3 t = vec3.clone(triangle.getTangent());
            // prendre en compte la surface du triangle
            vec3.scale(t, t, triangle.getSurface());
            // cumuler les tangentes pondérées
            vec3.add(sum, sum, t);
        }
        // normaliser le résultat
        vec3.normalize(sum, sum);
        return sum;
    }


    /**
     * Cette fonction retourne la liste des sommets qui sont contenus dans les triangles fournis.
     * @param triangles : liste de triangles
     * @return tableau des sommets
     */
    public static Set<MeshVertex> getVerticesFromTriangles(List<MeshTriangle> triangles)
    {
        Set<MeshVertex> result = new HashSet<>();
        for (MeshTriangle triangle: triangles) {
            for (MeshVertex vertex: triangle.getVertexList()) {
                result.add(vertex);
            }
        }
        return result;
    }


    /**
     * Détermine les (deux) triangles qui sont à gauche de la demi-arête mais sans sortir
     * de la zone délimitée par le contour. Rajoute aussi tous les triangles qui sont en contact
     * avec les triangles déterminés au premier coup (appels récursifs).
     * En fait, c'est un algo de remplissage de tache appliqué à un maillage de triangles.
     * @param trianglelist : liste à laquelle sont rajoutés les triangles
     * @param halfedge : demi-arête de départ
     * @param border : liste de demi-arêtes qui délimitent les triangles intéressants
     */
    protected void appendNeighborTriangles(List<MeshTriangle> trianglelist, MeshHalfEdge halfedge, List<MeshHalfEdge> border)
    {
        // triangle à gauche de cette demi-arête
        MeshTriangle triangle = halfedge.getTriangle();
        if (triangle == null) return;

        // ajouter ce triangle à la liste sauf s'il y est déjà
        if (trianglelist.indexOf(triangle) < 0) {
            trianglelist.add(triangle);

            // demi-arête suivante et son opposée
            halfedge = halfedge.getNext();
            MeshHalfEdge opposite = halfedge.getOpposite();

            // si elle n'est pas dans le contour, on va voir à sa droite
            if (border.indexOf(halfedge) < 0 && opposite != null) {
                appendNeighborTriangles(trianglelist, opposite, border);
            }

            // demi-arête suivante et son opposée
            halfedge = halfedge.getNext();
            opposite = halfedge.getOpposite();

            // si elle n'est pas dans le contour, on va voir à sa droite
            if (border.indexOf(halfedge) < 0 && opposite != null) {
                appendNeighborTriangles(trianglelist, opposite, border);
            }
        }
    }


    /**
     * retourne la liste des triangles qui sont délimités par une succession de demi-arêtes
     * Le contour doit être refermé sur lui-même ou constituer une frontière d'un bord
     * à l'autre du maillage, sinon tous les triangles seront sélectionnés.
     * Attention à l'ordre de spécification des sommets du contour : ce sont les triangles
     * à gauche qui sont spécifiés (tourner dans le sens trigo pour entourer une zone).
     * @param border : liste des demi-arêtes. Les triangles sont à gauche d'elles
     * @return liste des triangles compris dans le contour
     */
    public List<MeshTriangle> getTrianglesInsideBorder(List<MeshHalfEdge> border)
    {
        // résultat, initialisé à vide
        List<MeshTriangle> trianglelist = new LinkedList<>();

        // si la liste est trop courte, sortir sans rien faire
        if (border.size() < 3) return trianglelist;

        // partir de la première arête
        MeshHalfEdge halfedge = border.get(0);

        // appels récursifs pour rajouter les triangles qui sont en contact avec cette arête
        appendNeighborTriangles(trianglelist, halfedge, border);

        return trianglelist;
    }


    /**
     * retourne la liste des demi-arêtes correspondant à une liste de vertex formant un contour fermé
     * @param border : liste de sommets classés dans l'ordre, le dernier doit être lié au premier
     * @return liste des demi-arêtes
     * @throws Exception
     */
    public List<MeshHalfEdge> getHalfEdgesAlongBorder(List<MeshVertex> border) throws Exception
    {
        List<MeshHalfEdge> halfedgeslist = new LinkedList<>();

        MeshVertex prec = null;
        for (MeshVertex v: border) {
            if (prec != null) {
                MeshHalfEdge halfedge = prec.getHalfEdgeTo(v);
                if (halfedge == null) throw new Exception("Vertex"+prec.toString()+" has no half-edge to "+v.toString());
                halfedgeslist.add(halfedge);
            }
            prec = v;
        }
        // dernier sommet à lier avec le premier
        if (prec != null) {
            MeshVertex v = border.get(0);
            MeshHalfEdge halfedge = prec.getHalfEdgeTo(v);
            if (halfedge == null)
                throw new Exception("Vertex " + prec.toString() + " has no half-edge to " + v.toString());
            halfedgeslist.add(halfedge);
        }
        return halfedgeslist;
    }


    /**
     * Cette méthode calcule la valeur minimum des coordonnées
     * @return vec3 contenant min(x),min(y),min(z) des coordonnées des sommets
     */
    public vec3 computeMinAABB()
    {
        // déterminer les bornes min du maillage
        vec3 vmin = vec3.clone(m_Mesh.getVertexList().get(0).getCoord());
        for (MeshVertex vertex: m_Mesh.getVertexList()) {
            vec3.min(vmin, vmin, vertex.getCoord());
        }
        return vmin;
    }


    /**
     * Cette méthode calcule la valeur maximale des coordonnées
     * @return vec3 contenant max(x),max(y),max(z) des coordonnées des sommets
     */
    public vec3 computeMaxAABB()
    {
        // déterminer les bornes max du maillage
        vec3 vmax = vec3.clone(m_Mesh.getVertexList().get(0).getCoord());
        for (MeshVertex vertex: m_Mesh.getVertexList()) {
            vec3.max(vmax, vmax, vertex.getCoord());
        }
        return vmax;
    }


    /**
     * Cette méthode construit la boîte englobante du maillage this
     * @return maillage en forme de parallépipède rectangle
     */
    public Mesh createAABB() throws Exception
    {
        // déterminer les bornes du maillage
        vec3 vmin = vec3.clone(m_Mesh.getVertexList().get(0).getCoord());
        vec3 vmax = vec3.clone(m_Mesh.getVertexList().get(0).getCoord());
        for (MeshVertex vertex: m_Mesh.getVertexList()) {
            vec3.min(vmin, vmin, vertex.getCoord());
            vec3.max(vmax, vmax, vertex.getCoord());
        }

        // maillage en retour
        Mesh aabb = new Mesh();

        // sommets de ce maillage
        MeshVertex xyz = aabb.addVertex("xyz");
        MeshVertex Xyz = aabb.addVertex("Xyz");
        MeshVertex xYz = aabb.addVertex("xYz");
        MeshVertex XYz = aabb.addVertex("XYz");
        MeshVertex xyZ = aabb.addVertex("xyZ");
        MeshVertex XyZ = aabb.addVertex("XyZ");
        MeshVertex xYZ = aabb.addVertex("xYZ");
        MeshVertex XYZ = aabb.addVertex("XYZ");

        // coordonnées des coins
        xyz.setCoord(vec3.fromValues(vmin.get(0), vmin.get(1), vmin.get(2)));
        Xyz.setCoord(vec3.fromValues(vmax.get(0), vmin.get(1), vmin.get(2)));
        xYz.setCoord(vec3.fromValues(vmin.get(0), vmax.get(1), vmin.get(2)));
        XYz.setCoord(vec3.fromValues(vmax.get(0), vmax.get(1), vmin.get(2)));
        xyZ.setCoord(vec3.fromValues(vmin.get(0), vmin.get(1), vmax.get(2)));
        XyZ.setCoord(vec3.fromValues(vmax.get(0), vmin.get(1), vmax.get(2)));
        xYZ.setCoord(vec3.fromValues(vmin.get(0), vmax.get(1), vmax.get(2)));
        XYZ.setCoord(vec3.fromValues(vmax.get(0), vmax.get(1), vmax.get(2)));

        // construire les quads de la boîte
        aabb.addQuad(XyZ,Xyz,XYz,XYZ);
        aabb.addQuad(Xyz,xyz,xYz,XYz);
        aabb.addQuad(xyz,xyZ,xYZ,xYz);
        aabb.addQuad(xyZ,XyZ,XYZ,xYZ);
        aabb.addQuad(XYZ,XYz,xYz,xYZ);
        aabb.addQuad(Xyz,XyZ,xyZ,xyz);

        // finaliser cette boîte
        aabb.computeNormals();

        return aabb;
    }


    /**
     * Cette méthode vérifie la cohérence des informations du maillage.
     * La référence est la liste des sommets et la liste des triangles
     * NB : cette fonction ne teste pas toutes les erreurs possibles, mais
     * seulement quelques unes des plus graves.
     * @param repair : true s'il faut tenter de réparer les erreurs, ex: vertex non utilisé
     * @return true si le maillage semble correct, false s'il y a une grave erreur
     */
    public boolean checkMesh(boolean repair)
    {
        // vérifier les triangles
        int triangle_count = m_Mesh.getTriangleCount();
        for (int it=0; it<triangle_count; it++) {
            MeshTriangle triangle = m_Mesh.getTriangleList().get(it);
            // le triangle doit être du maillage
            if (triangle.getMesh() != m_Mesh) {
                Log.e(TAG, "Triangle #"+it+" does not belong to the mesh");
                return false;
            }
            // la demi-arête doit être dans le maillage
            if (m_Mesh.getHalfEdgeList().indexOf(triangle.getHalfEdge()) < 0) {
                Log.e(TAG, "Half-edge of Triangle #"+it+" does not belong to the mesh");
                return false;
            }
            // sa demi-arête doit le désigner
            if (triangle.getHalfEdge().getTriangle() != triangle) {
                Log.e(TAG, "Half-edge of Triangle #"+it+" does not point to it");
                return false;
            }
            // vérifier que les points sont tous différents
            if (triangle.getVertex0() == triangle.getVertex1() ||
                triangle.getVertex0() == triangle.getVertex2() ||
                triangle.getVertex1() == triangle.getVertex2()) {
                Log.e(TAG, "Vertices of triangle #"+it+" are not all distincts");
                return false;
            }
            // vérifier les points
            for (int i=0; i<3; i++) {
                MeshVertex vertex = triangle.getVertex(i);
                // ce sommet doit être dans la liste des sommets du maillage
                if (m_Mesh.getVertexList().indexOf(vertex) < 0) {
                    Log.e(TAG, "Vertex #"+i+"="+vertex.getName()+" of triangle #"+it+" does not belong to the mesh");
                    return false;
                }
            }
        }

        // liste des sommets à supprimer
        ArrayList<MeshVertex> suppr = new ArrayList<>();

        // vérifier les sommets
        int vertex_count = m_Mesh.getVertexCount();
        for (int iv=0; iv<vertex_count; iv++) {
            MeshVertex vertex = m_Mesh.getVertexList().get(iv);
            // le sommet doit être du maillage
            if (vertex.getMesh() != m_Mesh) {
                Log.e(TAG, "Vertex #"+iv+"="+vertex.getName()+" does not belong to the mesh");
                return false;
            }
            // la demi-arête doit être null ou dans le maillage
            if (vertex.getHalfEdge() != null && m_Mesh.getHalfEdgeList().indexOf(vertex.getHalfEdge()) < 0) {
                Log.e(TAG, "Half-edge of vertex #"+iv+"="+vertex.getName()+" does not belong to the mesh");
                return false;
            }
            // sa demi-arête doit le désigner
            if (vertex.getHalfEdge() != null && vertex.getHalfEdge().getOrigin() != vertex) {
                Log.e(TAG, "Half-edge of vertex #"+iv+"="+vertex.getName()+" does not point to it");
                return false;
            }
            // ce sommet doit être dans l'un des triangles du maillage
            boolean present = false;
            for (int it=0; it<triangle_count && !present; it++) {
                MeshTriangle triangle = m_Mesh.getTriangleList().get(it);
                for (int i=0; i<3; i++) {
                    if (triangle.getVertex(i) == vertex) {
                        present = true;
                        break;
                    }
                }
            }
            if (! present) {
                if (repair) {
                    suppr.add(vertex);
                } else {
                    Log.e(TAG, "Vertex "+vertex+" does not belong to any triangle of mesh");
                    //return false;
                }
            }
            // vérifier le chaînage m_Sibling
            MeshHalfEdge halfedge = vertex.getHalfEdge();
            while (halfedge != null) {
                // l'origine de cette demi-arête doit être le sommet
                if (halfedge.getOrigin() != vertex) {
                    Log.e(TAG, "Half-edge "+halfedge+" is chained to vertex "+vertex+" but does not point to it");
                    return false;
                }
                // il ne faut pas qu'il y ait un cycle dans le chaînage
                MeshHalfEdge other = vertex.getHalfEdge();
                while (other != null && other != halfedge) {
                    if (halfedge.getSibling() == other) {
                        Log.e(TAG, "Half-edge "+halfedge+" belongs to a cycle in siblings of vertex "+vertex);
                    }
                    other = other.getSibling();
                }
                halfedge = halfedge.getSibling();
            }
        }

        // supprimer les sommets qui ne sont dans aucun triangle (si l'option repair a été activée)
        for (MeshVertex vertex: suppr) {
            vertex.destroy();
        }

        // vérifier les demi-arêtes
        int halfedge_count = m_Mesh.getHalfEdgeList().size();
        for (int id=0; id<halfedge_count; id++) {
            MeshHalfEdge halfedge = m_Mesh.getHalfEdgeList().get(id);
            // la demi-arête doit être du maillage
            if (halfedge.getMesh() != m_Mesh) {
                Log.e(TAG, "Half-Edge #"+id+" does not belong to the mesh");
                return false;
            }
            // son origine doit être un des sommets du maillage
            if (m_Mesh.getVertexList().indexOf(halfedge.getOrigin()) < 0) {
                Log.e(TAG, "Origin of Opposite Half-edge of #"+id+" does not belong to the mesh");
                return false;
            }
            // vérifier l'opposée de la demi-arête
            if (halfedge.getOpposite() != null) {
                // son opposée doit être dans le maillage
                if (m_Mesh.getHalfEdgeList().indexOf(halfedge.getOpposite()) < 0) {
                    Log.e(TAG, "Opposite Half-edge of #"+id+" does not belong to the mesh");
                    return false;
                }
                // l'opposée de son opposée doit être elle-même
                if (halfedge.getOpposite().getOpposite() != halfedge) {
                    Log.e(TAG, "Opposite of opposite Half-edge of #"+id+" is not itself");
                    return false;
                }
            }
            // vérifier le chaînage m_Next
            if (halfedge.getNext() == null) {
                Log.e(TAG, "Next half-edge of #"+id+" is null");
                return false;
            }
            if (m_Mesh.getHalfEdgeList().indexOf(halfedge.getNext()) < 0) {
                Log.e(TAG, "Next half-edge of #"+id+" does not belong to the mesh");
                return false;
            }
            // vérifier le chaînage m_Next.m_Next
            if (halfedge.getNext().getNext() == null) {
                Log.e(TAG, "Second next of half-edge #"+id+" is null");
                return false;
            }
            if (m_Mesh.getHalfEdgeList().indexOf(halfedge.getNext().getNext()) < 0) {
                Log.e(TAG, "Second next of half-edge #"+id+" does not belong to the mesh");
                return false;
            }
            // vérifier le chaînage m_Next.m_Next.m_Next
            if (halfedge.getNext().getNext().getNext() == null) {
                Log.e(TAG, "Third next of half-edge #"+id+" is null");
                return false;
            }
            if (halfedge.getNext().getNext().getNext() != halfedge) {
                Log.e(TAG, "Third next of half-edge #"+id+" n'est pas elle-même");
                return false;
            }
        }

        // vérifier les arêtes
        int edge_count = m_Mesh.getEdgeList().size();
        for (int ia=0; ia<edge_count; ia++) {
            MeshEdge edge = m_Mesh.getEdgeList().get(ia);
            // l'arête doit être du maillage
            if (edge.getMesh() != m_Mesh) {
                Log.e(TAG, "Edge #"+ia+" does not belong to the mesh");
                return false;
            }
        }

        // bilan positif
        Log.i(TAG, "Mesh checked OK");

        // succès, mais attention, ça ne valide que ce qui a été testé
        return true;
    }


    /**
     * Cette méthode affiche la totalité des structures de données du maillage
     */
    public void dump()
    {
        // sommets
        for (MeshVertex vertex: m_Mesh.getVertexList())
            System.out.println(vertex.toFullString());
        // arêtes
        //for (MeshEdge edge: m_Mesh.getEdgeList())
        //    System.out.println(edge.toFullString());
        // triangles
        for (MeshTriangle triangle: m_Mesh.getTriangleList())
            System.out.println(triangle.toFullString());
        // demi-arêtes
        for (MeshHalfEdge halfedge: m_Mesh.getHalfEdgeList())
            System.out.println(halfedge.toFullString());
        System.out.println();
    }
}
