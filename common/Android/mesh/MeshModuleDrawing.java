package com.dbooker.livreopengl.mesh;

import static android.opengl.GLES20.*;

import java.util.ArrayList;
import java.util.Collections;

import android.util.Log;

import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.material.VBOset;
import com.dbooker.livreopengl.material.Material;



/**
 * Ce module rajoute des fonctions pour créer les VBO pour dessiner un maillage
 */
@SuppressWarnings("unused")
public class MeshModuleDrawing extends MeshModule
{
    /**
     * initialise le module sans maillage
     */
    public MeshModuleDrawing()
    {
        super();
    }


    /**
     * initialise le module sur le maillage fourni
     * @param mesh : maillage concerné
     */
    public MeshModuleDrawing(Mesh mesh)
    {
        super(mesh);
    }


    /** destructeur */
    public void destroy()
    {
        super.destroy();
    }


    /**
     * Cette méthode crée tous les VBO pour dessiner le maillage à l'aide de triangles
     * @param material : celui qu'il faut employer pour dessiner les triangles
     * @param interleaved : true s'il faut entrelacer les données
     * @throws Exception
     */
    public VBOset createVBOset(Material material, boolean interleaved) throws Exception
    {
        // créer les VBO demandés par le matériau
        VBOset vboset = material.createVBOset();
        vboset.createAttributesVBO(m_Mesh, interleaved);

        // rassembler les indices des triangles
        int[] indexlist = new int[m_Mesh.getTriangleCount() * 3];
        int it = 0;
        for (MeshTriangle triangle: m_Mesh.getTriangleList()) {
            for (int i=0; i<3; i++) {
                indexlist[it++] = (short) triangle.getVertex(i).getNumber();
            }
        }

        // créer le VBO des indices
        int size = vboset.createIndexedPrimitiveVBO(GL_TRIANGLES, indexlist);
        Log.d(TAG, m_Mesh.getName()+": "+size+" indices with triangles");

        return vboset;
    }
    public VBOset createVBOset(Material material) throws Exception
    {
        return createVBOset(material, true);
    }


    /**
     * Cette méthode crée tous les VBO pour dessiner le maillage à l'aide de triangle_strips
     * @param material : celui qu'il faut employer pour dessiner les triangles du ruban
     * @param interleaved : true s'il faut entrelacer les données
     */
    public VBOset createStripVBOset(Material material, boolean interleaved)
    {
        // créer les VBO demandés par le matériau
        VBOset vboset = material.createVBOset();
        vboset.createAttributesVBO(m_Mesh, interleaved);

        // créer une liste de rubans
        ArrayList<TriangleStrip> striplist = new ArrayList<>();

        // parcourir tous les triangles du maillage
        for (MeshTriangle triangle: m_Mesh.getTriangleList()) {
            // tenter d'insérer le triangle dans l'un des rubans
            boolean ok = false;
            final int stripnumber = striplist.size();
            for (int ir=0; ir<stripnumber; ir++) {
                TriangleStrip strip = striplist.get(ir);
                // le triangle peut-il être mis dans ce ruban ?
                if (strip.appendTriangle(triangle)) {
                    // voir si l'un des rubans suivants peut être concaténé au courant
                    for (int irs=ir+1; irs<stripnumber; irs++) {
                        TriangleStrip nextstrip = striplist.get(irs);
                        if (strip.concat(nextstrip)) {
                            // supprimer le ruban suivant
                            striplist.remove(nextstrip);
                            // ne pas continuer avec les autres rubans
                            break;
                        }
                    }
                    // c'est ok, on arrête le parcours des rubans
                    ok = true;
                    break;
                }
            }
            // si le triangle n'a pas pu être ajouté à un ruban, alors créer un autre ruban et le mettre en tête
            if (! ok) {
                striplist.add(0, new TriangleStrip(triangle));
            }
        }

        // rassembler les indices des sommets des rubans, avec des codes de redémarrage entre rubans
        ArrayList<Integer> indexlist = new ArrayList<>();
        TriangleStrip precstrip = null;
        for (TriangleStrip strip: striplist) {
            // rajouter un "primitive restart"
            if (precstrip != null) {
                int prec = precstrip.getVertex(-1).getNumber();
                if ((indexlist.size() % 2) == 1) indexlist.add(prec);
                indexlist.add(prec);
                int svt = strip.getVertex(0).getNumber();
                indexlist.add(svt);
            }
            // rajouter les sommets du ruban
            for (MeshVertex vertex: strip.getVertexList()) {
                indexlist.add(vertex.getNumber());
            }
            precstrip = strip;
        }

        // créer le VBO des indices
        int size = vboset.createIndexedPrimitiveVBO(GL_TRIANGLE_STRIP, indexlist);

        // message d'information
        final int triangle_count = m_Mesh.getTriangleList().size();
        Log.d(TAG, m_Mesh.getName()+": "+size+" indices with "+striplist.size()+" strips, instead of "+(triangle_count*3));

        return vboset;
    }
    public VBOset createStripVBOset(Material material) throws Exception
    {
        return createStripVBOset(material, true);
    }


    /**
     * Cette méthode crée tous les VBO pour dessiner les arêtes du maillage
     * @param material : celui qu'il faut employer pour dessiner les arêtes
     */
    public VBOset createEdgesVBOset(Material material)
    {
        // créer les VBO demandés par le matériau
        VBOset vboset = material.createVBOset();
        vboset.createAttributesVBO(m_Mesh, true);

        // test sur le nombre d'indices
        final int edge_count = m_Mesh.getEdgeList().size();
        //if (edge_count > 32767)
        //    throw new Exception("Too many edges to draw");

        // rassembler les indices des arêtes
        int[] indexlist = new int[edge_count * 2];
        int ia = 0;
        for (MeshEdge edge: m_Mesh.getEdgeList()) {
            // rajouter les sommets de l'arête
            indexlist[ia++] = edge.getVertex1().getNumber();
            indexlist[ia++] = edge.getVertex2().getNumber();
        }

        // créer le VBO des indices
        vboset.createIndexedPrimitiveVBO(GL_LINES, indexlist);

        return vboset;
    }


    /**
     * Cette méthode crée tous les VBO pour dessiner les normales des facettes du maillage
     * @param material : celui qu'il faut employer pour dessiner les normales
     * @param length : nombre donnant la longueur des normales à afficher
     */
    public VBOset createFacesNormalsVBOset(Material material, float length)
    {
        // créer les VBO demandés par le matériau
        VBOset vboset = material.createVBOset();

        // remplir les VBO avec les données demandées
        ArrayList<Float> data = new ArrayList<>();

        // sommet et coordonnées temporaires
        MeshVertex vertextmp = new MeshVertex(null, "tmp");
        vec3 coords_center = vec3.create();
        vec3 coords_endpoint = vec3.create();

        // traiter chaque triangle
        for (MeshTriangle triangle: m_Mesh.getTriangleList()) {

            // sommets du triangle courant
            MeshVertex v0 = triangle.getVertex0();
            MeshVertex v1 = triangle.getVertex1();
            MeshVertex v2 = triangle.getVertex2();

            // normale du sommet temporaire
            vertextmp.setNormal(triangle.getNormal());

            // centre du triangle : ajouter ses données dans le VBO
            vec3.zero(coords_center);
            vec3.add(coords_center, v0.getCoord(), v1.getCoord());
            vec3.add(coords_center, coords_center, v2.getCoord());
            vec3.scale(coords_center, coords_center, 1.0f/3.0f);
            vertextmp.setCoord(coords_center);

            // ajouter les valeurs des variables attributs (VBOvar)
            vboset.appendVertexComponents(vertextmp, data);

            // extremité = centre + normale * longueur
            vec3.zero(coords_endpoint);
            vec3.scale(coords_endpoint, triangle.getNormal(), length);
            vec3.add(coords_endpoint, coords_center, coords_endpoint);
            vertextmp.setCoord(coords_endpoint);

            // ajouter les valeurs des variables attributs (VBOvar)
            vboset.appendVertexComponents(vertextmp, data);
        }
        vertextmp.destroy();

        // créer le VBO entrelacé
        vboset.createInterleavedDataAttributesVBO(data);

        // pas de VBO d'indices car seulement des lignes à dessiner
        final int triangle_count = m_Mesh.getTriangleCount();
        vboset.createDirectPrimitiveVBO(GL_LINES, triangle_count * 2);

        return vboset;
    }


    /**
     * Cette méthode crée tous les VBO pour dessiner les normales des sommets du maillage
     * @param material : celui qu'il faut employer pour dessiner les normales
     * @param length : nombre donnant la longueur des normales à afficher
     */
    public VBOset createVertexNormalsVBOset(Material material, float length)
    {
        // créer les VBO demandés par le matériau
        VBOset vboset = material.createVBOset();

        // remplir les VBO avec les données demandées
        ArrayList<Float> data = new ArrayList<>();

        // sommet et coordonnées temporaires
        MeshVertex vertextmp = new MeshVertex(null, "tmp");
        vec3 coords_endpoint = vec3.create();

        // traiter chaque vertex
        for (MeshVertex vertex: m_Mesh.getVertexList()) {

            // ajouter les valeurs des variables attributs (VBOvar)
            vboset.appendVertexComponents(vertex, data);

            // extremité = sommet + normale * longueur
            vec3.zero(coords_endpoint);
            vec3.scale(coords_endpoint, vertex.getNormal(), length);
            vec3.add(coords_endpoint, vertex.getCoord(), coords_endpoint);
            vertextmp.setCoord(coords_endpoint);

            // ajouter les valeurs des variables attributs (VBOvar)
            vboset.appendVertexComponents(vertextmp, data);
        }
        vertextmp.destroy();

        // créer le VBO entrelacé
        vboset.createInterleavedDataAttributesVBO(data);

        // pas de VBO d'indices car seulement des lignes à dessiner
        final int vertex_count = m_Mesh.getVertexCount();
        vboset.createDirectPrimitiveVBO(GL_LINES, vertex_count * 2);

        return vboset;
    }


    /**
     * Cette classe représente un triangle strip
     */
    private class TriangleStrip
    {
        private ArrayList<MeshVertex> m_VertexList;

        /**
         * Cette classe représente un triangle strip
         * NB: l'ordre des sommets du triangle est important pour commencer un ruban !
         * @param triangle : le premier triangle d'un ruban
         */
        TriangleStrip(MeshTriangle triangle)
        {
            // allouer la liste des sommets du ruban
            m_VertexList = new ArrayList<>();

            // copie de la liste des sommets du triangle
            m_VertexList.add(triangle.getVertex0());
            m_VertexList.add(triangle.getVertex1());
            m_VertexList.add(triangle.getVertex2());
        }


        /**
         * retourne la longueur du ruban, son nombre de sommets
         * @return nombre de sommets du ruban
         */
        public int length()
        {
            return m_VertexList.size();
        }


        /**
         * retourne la liste des sommets du ruban
         * @return liste des sommets
         */
        public ArrayList<MeshVertex> getVertexList()
        {
            return m_VertexList;
        }


        /**
         * retourne l'un des sommets du ruban, désigné par son numéro
         * @param i = le numéro du sommet, si <0 alors par rapport à la fin du tableau
         * @return le ie sommet du ruban
         */
        MeshVertex getVertex(int i)
        {
            if (i < 0) {
                return m_VertexList.get(m_VertexList.size()+i);
            } else {
                return m_VertexList.get(i);
            }
        }


        /**
         * Cette méthode tente d'ajouter le triangle au ruban
         * @param triangle à rajouter
         * @return true si le triangle a pu être mis dans le ruban, false sinon
         */
        boolean appendTriangle(MeshTriangle triangle)
        {
            // nombre de sommets du ruban
            int vertex_number = m_VertexList.size();

            // cas particulier : le ruban ne compte qu'un seul triangle
            if (vertex_number == 3) {
                // sommets du ruban
                MeshVertex v0 = m_VertexList.get(0);
                MeshVertex v1 = m_VertexList.get(1);
                MeshVertex v2 = m_VertexList.get(2);
                // NB: les tests sont dans l'ordre le plus favorable si les triangles ont été créés par addNappeRectangulaire
                // le triangle contient-il l'arête C-B ?
                if (triangle.containsEdge(v2, v1)) {
                    // le triangle se colle à l'arête B-C du ruban
                    m_VertexList.clear();
                    m_VertexList.add(v0);
                    m_VertexList.add(v1);
                    m_VertexList.add(v2);
                    m_VertexList.add(triangle.getVertex(2));
                    return true;
                }
                // le triangle contient-il l'arête B-A ?
                if (triangle.containsEdge(v1, v0)) {
                    // le triangle se colle à l'arête A-B du ruban
                    m_VertexList.clear();
                    m_VertexList.add(v2);
                    m_VertexList.add(v0);
                    m_VertexList.add(v1);
                    m_VertexList.add(triangle.getVertex(2));
                    return true;
                }
                // le triangle contient-il l'arête A-C ?
                if (triangle.containsEdge(v0, v2)) {
                    // le triangle se colle à l'arête C-A du ruban
                    m_VertexList.clear();
                    m_VertexList.add(v1);
                    m_VertexList.add(v2);
                    m_VertexList.add(v0);
                    m_VertexList.add(triangle.getVertex(2));
                    return true;
                }
                // aucun des trois côtés ne convient
                return false;
            }

            // si le nombre de sommets du ruban est pair, on peut le retourner si besoin
            if ((vertex_number % 2) == 0) {
                // test d'insertion en fin de ruban
                if (triangle.containsEdge(m_VertexList.get(vertex_number-2), m_VertexList.get(vertex_number-1))) {
                    m_VertexList.add(triangle.getVertex(2));
                    return true;
                }
                // test d'insertion en debut de ruban
                if (triangle.containsEdge(m_VertexList.get(1), m_VertexList.get(0))) {
                    // retourner le ruban
                    Collections.reverse(m_VertexList);
                    m_VertexList.add(triangle.getVertex(2));
                    return true;
                }
            } else {
                // le nombre de sommets du ruban est impair, on ne peut insérer qu'en fin
                if (triangle.containsEdge(m_VertexList.get(vertex_number-1), m_VertexList.get(vertex_number-2))) {
                    m_VertexList.add(triangle.getVertex(2));
                    return true;
                }
            }

            // aucune solution n'est satisfaisante, le triangle ne peut pas être ajouté
            return false;
        }


        /**
         * Cette méthode tente d'ajouter un ruban de 3 sommets au ruban
         * @param other : autre ruban à rajouter
         * @return true si l'autre ruban a pu être mis dans le ruban, false sinon
         */
        private boolean appendStrip3(TriangleStrip other)
        {
            // nombre de sommets du ruban
            int vertex_number = m_VertexList.size();

            // cas particulier : le ruban ne compte qu'un seul triangle
            if (vertex_number == 3) {
                // sommets du ruban
                MeshVertex v0 = m_VertexList.get(0);
                MeshVertex v1 = m_VertexList.get(1);
                MeshVertex v2 = m_VertexList.get(2);
                // NB: les tests sont dans l'ordre le plus favorable si les triangles ont été créés par addNappeRectangulaire
                // l'autre ruban contient-il l'arête C-B ?
                if (other.containsEdge(v2, v1)) {
                    // l'autre ruban se colle à l'arête B-C du ruban
                    m_VertexList.clear();
                    m_VertexList.add(v0);
                    m_VertexList.add(v1);
                    m_VertexList.add(v2);
                    m_VertexList.add(other.m_VertexList.get(2));
                    return true;
                }
                // l'autre ruban contient-il l'arête B-A ?
                if (other.containsEdge(v1, v0)) {
                    // l'autre ruban se colle à l'arête A-B du ruban
                    m_VertexList.clear();
                    m_VertexList.add(v2);
                    m_VertexList.add(v0);
                    m_VertexList.add(v1);
                    m_VertexList.add(other.m_VertexList.get(2));
                    return true;
                }
                // l'autre ruban contient-il l'arête A-C ?
                if (other.containsEdge(v0, v2)) {
                    // l'autre ruban se colle à l'arête C-A du ruban
                    m_VertexList.clear();
                    m_VertexList.add(v1);
                    m_VertexList.add(v2);
                    m_VertexList.add(v0);
                    m_VertexList.add(other.m_VertexList.get(2));
                    return true;
                }
                // aucun des trois côtés ne convient
                return false;
            }

            // si le nombre de sommets du ruban est pair, on peut le retourner si besoin
            if ((vertex_number % 2) == 0) {
                // test d'insertion en fin de ruban
                if (other.containsEdge(m_VertexList.get(vertex_number-2), m_VertexList.get(vertex_number-1))) {
                    m_VertexList.add(other.m_VertexList.get(2));
                    return true;
                }
                // test d'insertion en debut de ruban
                if (other.containsEdge(m_VertexList.get(1), m_VertexList.get(0))) {
                    // retourner le ruban
                    Collections.reverse(m_VertexList);
                    m_VertexList.add(other.m_VertexList.get(2));
                    return true;
                }
            } else {
                // le nombre de sommets du ruban est impair, on ne peut insérer qu'en fin
                if (other.containsEdge(m_VertexList.get(vertex_number-1), m_VertexList.get(vertex_number-2))) {
                    m_VertexList.add(other.m_VertexList.get(2));
                    return true;
                }
            }

            // aucune solution n'est satisfaisante, le triangle ne peut pas être ajouté
            return false;
        }


        /**
         * retourne true si le ruban contient 3 sommets, dont v1 suivi de v2
         * Si true, alors au retour, v1 et v2 sont dans les deux premières places du ruban
         * @param vertex0 : premier sommet de l'arête (elle est orientée de 1 à 2)
         * @param vertex1 : second sommet
         * @return true si oui, false si l'arête n'est pas dans le triangle
         */
        boolean containsEdge(MeshVertex vertex0, MeshVertex vertex1)
        {
            if (m_VertexList.size() != 3) return false;
            MeshVertex v0 = m_VertexList.get(0);
            MeshVertex v1 = m_VertexList.get(1);
            MeshVertex v2 = m_VertexList.get(2);

            if (v0 == vertex0 && v1 == vertex1) return true;

            if (v1 == vertex0 && v2 == vertex1) {
                // mettre v2 en premier
                m_VertexList.clear();
                m_VertexList.add(v1);
                m_VertexList.add(v2);
                m_VertexList.add(v0);
                return true;
            }

            if (v2 == vertex0 && v0 == vertex1) {
                // mettre v2 en premier
                m_VertexList.clear();
                m_VertexList.add(v2);
                m_VertexList.add(v0);
                m_VertexList.add(v1);
                return true;
            }

            return false;
        }


        /**
         * Cette méthode tente d'ajouter le ruban fourni au ruban this
         * @param strip : ruban à rajouter
         * @return true si le ruban a pu être concaténé à this, false sinon
         */
        boolean concat(TriangleStrip strip)
        {
            // nombre de sommets des rubans
            int thisvertexnumber = m_VertexList.size();
            int stripvertexnumber = strip.m_VertexList.size();

            // cas particulier d'un triangle, appeler l'autre méthode
            if (stripvertexnumber == 3) return appendStrip3(strip);

            // si this a un nombre de sommets pair, alors tenter
            if ((thisvertexnumber % 2) == 0) {
                // il faut que la dernière arête de this = la première de ruban
                if (m_VertexList.get(thisvertexnumber-2) == strip.m_VertexList.get(0) &&
                        m_VertexList.get(thisvertexnumber-1) == strip.m_VertexList.get(1)) {
                    m_VertexList.addAll(strip.m_VertexList.subList(2, stripvertexnumber));
                    return true;
                }
                // si l'autre ruban a un nombre pair de sommets, on peut le retourner
                if ((stripvertexnumber % 2) == 0) {
                    // tenter de mettre l'autre ruban à l'envers en fin de this
                    if (m_VertexList.get(thisvertexnumber-2) == strip.m_VertexList.get(stripvertexnumber-1) &&
                            m_VertexList.get(thisvertexnumber-1) == strip.m_VertexList.get(stripvertexnumber-2)) {
                        Collections.reverse(strip.m_VertexList);
                        m_VertexList.addAll(strip.m_VertexList.subList(2, stripvertexnumber));
                        return true;
                    }
                    // tenter de metre l'autre ruban au début de this
                    if (m_VertexList.get(0) == strip.m_VertexList.get(stripvertexnumber-2) &&
                            m_VertexList.get(1) == strip.m_VertexList.get(stripvertexnumber-1)) {
                        @SuppressWarnings("unchecked")
                        ArrayList<MeshVertex> tmp = (ArrayList<MeshVertex>) strip.m_VertexList.clone();
                        tmp.addAll(m_VertexList.subList(2, thisvertexnumber));
                        m_VertexList = tmp;
                        return true;
                    }
                }
            }

            // si ruban a un nombre de sommets pair
            if ((stripvertexnumber % 2) == 0) {
                // tenter de mettre this à la fin de ruban
                if (m_VertexList.get(0) == strip.m_VertexList.get(stripvertexnumber-2) &&
                        m_VertexList.get(1) == strip.m_VertexList.get(stripvertexnumber-1)) {
                    @SuppressWarnings("unchecked")
                    ArrayList<MeshVertex> tmp = (ArrayList<MeshVertex>) strip.m_VertexList.clone();
                    tmp.addAll(m_VertexList.subList(2, thisvertexnumber));
                    m_VertexList = tmp;
                    return true;
                }
                // tenter de mettre this en début de ruban (en inversant ruban)
                if (m_VertexList.get(0) == strip.m_VertexList.get(1) &&
                        m_VertexList.get(1) == strip.m_VertexList.get(0)) {
                    Collections.reverse(strip.m_VertexList);
                    @SuppressWarnings("unchecked")
                    ArrayList<MeshVertex> tmp = (ArrayList<MeshVertex>) strip.m_VertexList.clone();
                    tmp.addAll(m_VertexList.subList(2, thisvertexnumber));
                    m_VertexList = tmp;
                    return true;
                }
            }

            // aucune solution n'est satisfaisante, le ruban ne peut pas être ajouté
            return false;
        }
    }
}
