package com.dbooker.livreopengl.mesh;

import android.util.Log;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;

import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.vec3;


/**
 * Ce module rajoute des fonctions de transformations géométriques
 * NB: penser à appeler creerVBO() avant et après chaque transformation
 */
@SuppressWarnings("unused")
public class MeshModuleProcessing extends MeshModuleUtils
{
    /**
     * initialise le module sans maillage
     */
    public MeshModuleProcessing()
    {
        super();
    }


    /**
     * initialise le module sur le maillage fourni
     * @param mesh : maillage concerné
     */
    public MeshModuleProcessing(Mesh mesh)
    {
        super(mesh);
    }


    /** destructeur */
    public void destroy()
    {
        super.destroy();
    }


    /**
     * Cette méthode fait une homothétie du triangle autour de son centre
     * @param triangle auquel on applique la transformation
     * @param scale : rapport d'agrandissement (si > 1.0) ou de réduction (si < 1.0)
     * @return triangle : le triangle auquel on applique cette méthode
     */
    public static MeshTriangle homothety(MeshTriangle triangle, float scale)
    {
        // traiter tous les sommets
        for (int i=0; i<3; i++) {
            MeshVertex vertex = triangle.getVertex(i);
            vec3 coords = vertex.getCoord();
            vec3.subtract(coords, coords, triangle.getCenter());
            vec3.scale(   coords, coords, scale);
            vec3.add(     coords, coords, triangle.getCenter());
        }
        return triangle;
    }


    /**
     * Cette méthode casse l'angle au niveau du sommet indiqué. Elle
     * enfonce le sommet de la distance selon la direction fournie en paramètre.
     * @param vertex : sommet qu'il faut tronquer
     * @param distance : distance où vont aller les points intermédiaires
     * @param direction : vec3 indiquant l'axe d'enfoncement des points
     * @return liste des sommets du contour résultant de la découpe
     */
    public ArrayList<MeshVertex> bevelVertex(MeshVertex vertex, float distance, vec3 direction) throws  Exception
    {
        // normaliser la direction
        vec3.normalize(direction, direction);

        // liste des sommets formant le contour
        ArrayList<MeshVertex> border = new ArrayList<>();

        // couper en deux chaque triangle entourant le sommet
        for (MeshTriangle triangle: vertex.getTrianglesOrderedAround()) {
            // faire tourner la liste des sommets du triangle pour mettre this en premier
            if (!triangle.cycleVertexFirst(vertex)) {
                Log.e(TAG, "bug dans le mesh");
                continue;
            }

            // découper de chaque côté
            ArrayList<MeshVertex> quad = new ArrayList<>();
            quad.add(triangle.getVertex1());
            quad.add(triangle.getVertex2());
            for (int is=1; is<=2; is++) {
                MeshVertex other = triangle.getVertex(is);
                String midname = getMidName(vertex, other);
                MeshVertex mid = m_Mesh.getVertexByName(midname);
                if (mid == null) {
                    mid = m_Mesh.addVertex(midname);
                    // calculer la distance relative
                    vec3 edge = vec3.create();
                    vec3.subtract(edge, vertex.getCoord(), other.getCoord());
                    float k = distance / vec3.dot(direction, edge);
                    // positionner le point intermédiaire à cette distance
                    mid.lerp(vertex, other, k);
                    // ajouter ce point au contour
                    border.add(mid);
                }
                // mémoriser le point intermédiaire
                quad.add(mid);
            }

            // supprimer le triangle
            triangle.destroy();

            // créer un quad entre s1, s2, m1 et m2 pour remplacer le triangle
            m_Mesh.addQuad(quad.get(0), quad.get(1), quad.get(3), quad.get(2));

            // créer un triangle entre le sommet, m1 et m2
            this.m_Mesh.addTriangle(vertex, quad.get(2), quad.get(3));
        }

        /* deux choix possibles : garder l'ancien sommet et ses anciens triangles, ou refaire un polygone */

        /* choix 1 : on garde l'ancien sommet mais on le met dans le plan du contour */
        /*

        // déplacer le sommet dans le plan de ceux du contour
        vec3 translation = vec3.clone(direction);
        vec3.scale(translation, translation, -distance);
        vec3.add(vertex.getCoord(), vertex.getCoord(), translation);

        */
        /* choix 2 : on supprime l'ancien sommet et on recrée un polygone dans le contour */

        // supprimer le sommet
        vertex.destroy();

        // créer un polygone pour remplir le trou
        m_Mesh.addPolygon(border, direction);

        // recalculer les normales
        m_Mesh.computeNormals();

        return border;
    }


    /**
     * Cette méthode clone les sommets spécifiés dans le contour afin
     * de la rendre dure (séparer deux groupes de lissage de part et d'autre
     * NB: ce sont les sommets des triangles "de gauche" qui sont clonés
     * @param border : liste de sommets (l'ordre est important), il en faut au moins 2
     */
    public void splitBorder(List<MeshVertex> border) throws  Exception
    {
        // si la liste est trop courte, sortir sans rien faire
        final int vertex_count = border.size();
        if (vertex_count < 2) return;

        // liste des triangles dont les sommets vont être clonés
        List<MeshHalfEdge> halfedges = getHalfEdgesAlongBorder(border);
        List<MeshTriangle> triangles = getTrianglesInsideBorder(halfedges);

        // liste des clones, il y en a autant que de sommets dans l'arête
        MeshVertex[] clones = new MeshVertex[vertex_count];

        // remplacer les sommets de l'arête dans tous les triangles
        for (MeshTriangle triangle: triangles) {
            // remplacer tous les sommets qui sont dans l'arête par leurs clones
            for (int is=0; is<vertex_count; is++) {
                MeshVertex vertex = border.get(is);
                MeshVertex clone = clones[is];
                // le triangle contient-il ce sommet ?
                if (triangle.containsVertex(vertex)) {
                    // cloner le sommet, sauf s'il l'est déjà
                    if (clones[is] == null) {
                        clone = vertex.clone("clone");
                        clones[is] = clone;
                    }
                    // remplacer le sommet par son clone
                    triangle.replaceVertex(vertex, clone);
                }
            }
        }
    }


    /**
     * Cette méthode fait une extrusion des triangles contenus dans le contour
     * et de la distance indiquée
     * @param border : liste de sommets, délimitent les triangles concernés qui sont à gauche du contour
     * @param distance : longueur de l'extrusion
     * @return liste des clones des sommets de tous les triangles inclus dans le contour
     */
    public ArrayList<MeshVertex> extrudePolygon(ArrayList<MeshVertex> border, float distance) throws  Exception
    {
        // liste des triangles dont les sommets vont être clonés
        List<MeshHalfEdge> halfedges = getHalfEdgesAlongBorder(border);
        List<MeshTriangle> triangles = getTrianglesInsideBorder(halfedges);
        if (triangles.size() == 0) return null;
        final int vertex_count = border.size();

        // direction de l'extrusion : moyenne normalisée des normales des triangles
        vec3 direction = averageNormals(triangles);

        // définir le décalage à partir de la direction et la distance
        vec3 offset = vec3.create();
        vec3.scale(offset, direction, distance);

        // liste des clones, il y en a autant que de sommets dans le contour
        ArrayList<MeshVertex> clones = new ArrayList<>(vertex_count);
        for (MeshVertex vertex: border) {
            MeshVertex clone = vertex.clone("clone");
            clones.add(clone);
        }

        // remplacer les sommets par leurs clones dans tous les triangles
        for (MeshTriangle triangle: triangles) {
            // remplacer tous les sommets par leurs clones
            for (int iv=0; iv<vertex_count; iv++) {
                // sommet courant et son clone
                MeshVertex vertex = border.get(iv);
                MeshVertex clone = clones.get(iv);
                // remplacer le sommet par son clone (si sommet est dans ce triangle)
                triangle.replaceVertex(vertex, clone);
            }
        }

        // construire la liste des sommets contenus dans ces triangles
        Set<MeshVertex> vertex_list = getVerticesFromTriangles(triangles);

        // décaler les sommets
        for (MeshVertex vertex: vertex_list) {
            // décaler le sommet
            vec3.add(vertex.getCoord(), vertex.getCoord(), offset);
        }

        // construire des quads avec chaque couple de sommets du contour
        for (int ic=0; ic<vertex_count; ic++) {
            // sommet courant et son clone
            MeshVertex A = border.get(ic);
            MeshVertex A1 = clones.get(ic);
            // sommet suivant et son clone
            MeshVertex B = border.get((ic+1)%vertex_count);
            MeshVertex B1 = clones.get((ic+1)%vertex_count);
            // construire un quad sur ces 4 sommets
            m_Mesh.addQuad(A, B, B1, A1);
        }

        return clones;
    }


    /**
     * Cette méthode fait une extrusion du triangle indiqué, le long de sa normale
     * et de la distance indiquée
     * @param triangle : celui qu'il faut extruder
     * @param distance : longueur de l'extrusion
     * @return nouveau triangle résultat de l'extrusion
     */
    public MeshTriangle extrudeTriangle(MeshTriangle triangle, float distance) throws Exception
    {
        // sommets du triangle existant
        MeshVertex A = triangle.getVertex0();
        MeshVertex B = triangle.getVertex1();
        MeshVertex C = triangle.getVertex2();

        // définir le décalage à partir de la normale et la distance
        vec3 offset = vec3.create();
        vec3.scale(offset, triangle.getNormal(), distance);

        // décaler les sommets dans la direction de la normale
        MeshVertex A1 = A.clone("clone");
        vec3.add(A1.getCoord(), A.getCoord(), offset);
        MeshVertex B1 = B.clone("clone");
        vec3.add(B1.getCoord(), B.getCoord(), offset);
        MeshVertex C1 = C.clone("clone");
        vec3.add(C1.getCoord(), C.getCoord(), offset);

        // remplacer les sommets par leurs clones dans le triangle (A,B,C)
        triangle.replaceVertex(A, A1);
        triangle.replaceVertex(B, B1);
        triangle.replaceVertex(C, C1);

        // créer un quad sur chacun des trois flancs
        m_Mesh.addQuad(A, B, B1, A1);
        m_Mesh.addQuad(B, C, C1, B1);
        m_Mesh.addQuad(C, A, A1, C1);

        // retourner le triangle modifié
        return triangle;
    }


    /**
     * Cette méthode subdivise le triangle en sous-triangles
     * @param triangle auquel on applique la transformation
     * @param steps : fournir un entier 1..raisonnable
     * @param smooth : déplace ou non les points midpoints vers l'intérieur ou l'extérieur
     * @return liste de triangles qui doivent remplacer celui-ci
     */
    private List<MeshTriangle> subdivide(MeshTriangle triangle, int steps, float smooth) throws Exception
    {
        ArrayList<MeshTriangle> result = new ArrayList<>();

        // fin de la récursivité ?
        if (steps <= 0) {
            // ajouter le triangle au résultat et le retourner
            result.add(triangle);
            return result;
        }
        steps -= 1;

        // noter les trois sommets du triangle
        MeshVertex vertex_list[] = new MeshVertex[] {
                triangle.getVertex0(),
                triangle.getVertex1(),
                triangle.getVertex2()
        };

        // sommets intermédiaires (dans l'ordre des sommets)
        ArrayList<MeshVertex> midpoints = new ArrayList<>();

        // traiter chaque arête
        for (int is=0; is<3; is++) {
            // extrémités de l'arête
            MeshVertex s0 = triangle.getVertex(is);
            MeshVertex s1 = triangle.getVertex((is+1)%3);
            // milieu entre s0 et s1
            String name01 = getMidName(s0, s1);
            MeshVertex m01 = m_Mesh.getVertexByName(name01);
            if (m01 == null) {
                m01 = new MeshVertex(m_Mesh, name01);
                // lissage ?
                if (smooth > 0.0) {
                    // vecteur s0s1
                    vec3 s0s1 = vec3.create();
                    vec3.subtract(s0s1, s1.getCoord(), s0.getCoord());
                    vec3.scale(s0s1, s0s1, smooth);
                    // normale, binormale et tangente en s0
                    vec3 n0 = s0.getNormal();
                    vec3 b0 = vec3.create();
                    vec3.cross(b0, s0s1, n0);
                    vec3 t0 = vec3.create();
                    vec3.cross(t0, n0, b0);
                    // normale, binormale et tangente en s1
                    vec3 n1 = s1.getNormal();
                    vec3 b1 = vec3.create();
                    vec3.cross(b1, s0s1, n1);
                    vec3 t1 = vec3.create();
                    vec3.cross(t1, n1, b1);
                    // interpolation cubique spline de Hermite
                    m01.hermite(s0, t0, s1, t1, 0.5f);
                } else {
                    // simple interpolation linéaire
                    m01.lerp(s0, s1, 0.5f);
                }
            }
            midpoints.add(m01);
        }

        // supprimer le triangle
        triangle.destroy();

        // créer 3 triangles à l'intérieur de this (sens trigo)
        for (int is=0; is<3; is++) {
            MeshVertex s0 = vertex_list[is];
            MeshVertex m01 = midpoints.get(is);
            MeshVertex m20 = midpoints.get((is+2)%3);
            MeshTriangle triangle_tmp = new MeshTriangle(m_Mesh, s0, m01, m20);
            result.addAll(subdivide(triangle_tmp, steps, smooth));
        }

        // triangle central
        MeshTriangle triangle_tmp = new MeshTriangle(m_Mesh, midpoints.get(0), midpoints.get(1), midpoints.get(2));
        result.addAll(subdivide(triangle_tmp, steps, smooth));

        return result;
    }


    /**
     * Cette méthode subdivise les triangles indiqués
     * @param triangles : liste des triangles concernés par la subdivision
     * @param steps : fournir un entier 1..raisonnable
     * @param smooth : déplace ou non les points milieux vers l'intérieur ou l'extérieur
     * @return liste de triangles qui doivent remplacer celui-ci
     */
    public List<MeshTriangle> subdivideAll(List<MeshTriangle> triangles, int steps, float smooth) throws Exception
    {
        // mémoriser les triangles qui sont subdivisés
        List<MeshTriangle> result = new ArrayList<>();

        // liste des triangles dont les sommets vont être clonés
        final int nbtriangles = triangles.size();
        if (nbtriangles == 0) return result;

        // faut-il faire quelque chose ?
        if (steps <= 0) return triangles;

        // si la liste qui est fournie est celle des triangles de this, alors la cloner parce qu'on va la modifier
        if (triangles == m_Mesh.getTriangleList()) {
            triangles = new ArrayList<>(m_Mesh.getTriangleList());
        }

        // subdiviser tous les triangles
        for (MeshTriangle triangle: triangles) {
            result.addAll(subdivide(triangle, steps, smooth));
        }
        return result;
    }


    /**
     * Cette méthode applique une matrice à tous les sommets du maillage
     * @param matrix : la transformation qu'il faut appliquer sur les sommets
     */
    public void transform(mat4 matrix)
    {
        for (MeshVertex vertex: m_Mesh.getVertexList()) {
            vec3.transformMat4(vertex.getCoord(), vertex.getCoord(), matrix);
        }
    }
}
