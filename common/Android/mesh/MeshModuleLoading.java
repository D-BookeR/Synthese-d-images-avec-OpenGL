package com.dbooker.livreopengl.mesh;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Collection;
import java.util.LinkedList;

import android.content.Context;
import android.util.SparseArray;

import com.dbooker.livreopengl.glmatrix.vec2;
import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.utils.ApplicationContext;


/**
 * Ce module lit un fichier OBJ et en fait un maillage
 */


@SuppressWarnings("unused")
public class MeshModuleLoading extends MeshModule
{
    /**
     * initialise le module sans maillage
     */
    public MeshModuleLoading()
    {
        super();
    }


    /**
     * initialise le module sur le maillage fourni
     * @param mesh : maillage concerné
     */
    public MeshModuleLoading(Mesh mesh)
    {
        super(mesh);
    }


    /** destructeur */
    public void destroy()
    {
        super.destroy();
    }


    /**
     * Cette fonction reçoit un tableau de chaînes ["nv", "nt", "nn"]
     * Elle retourne la conversion l'une de ces chaînes en entier.
     * La chaîne est indiquée par l'indice.
     * Si la valeur lue est négatif, alors la fonction retourne max+indice
     * @param strings : tableau contenant ["nv", "nt", "nn"]
     * @param index : numéro de la chaîne à convertir
     * @param max : nombre de valeurs présentes dans les tableaux de coordonnées...
     * @return -1 si pb, nv ou nt ou nn selon l'index
     */
    private static int splitNumbers(String[] strings, int index, int max)
    {
        if (index >= strings.length) return -1;
        String word = strings[index];
        if (word.isEmpty()) return -1;
        int value = Integer.parseInt(word);
        if (value > max) return -1;
        if (value < 0) return max + value;
        return value - 1;
    }


    /**
     * Cette méthode recherche dans sommets celui qui possède exactement les
     * mêmes coordonnées : 3D, de texture et normale qu'indiqué
     * @param nvntnn : mot contenant les numéros nn/nt/nn
     * @param vertexlist : tableau indexé par iv de sommets au même endroit
     * @param coordlist : tableau de vec3 donnant les coordonnées des sommets
     * @param texcoordlist : tableau de vec2 donnant les coordonnées de texture
     * @param normallist : tableau de vec3 donnant les normales
     * @return le sommet correspondant au triplet (nv,nt,nn)
     */
    private MeshVertex findOrCreateVertex(String nvntnn, SparseArray<Collection<MeshVertex>> vertexlist, ArrayList<vec3> coordlist, ArrayList<vec2> texcoordlist, ArrayList<vec3> normallist)
    {
        // indices des coordonnées 3D, des coordonnées de texture et de la normale
        String[] strings = nvntnn.split("/");
        int nv = splitNumbers(strings, 0, coordlist.size());
        int nt = splitNumbers(strings, 1, texcoordlist.size());
        int nn = splitNumbers(strings, 2, normallist.size());
        if (nv < 0) return null;

        // nom du sommet courant, c'est son identifiant
        String name = "v("+nv+","+nt+","+nn+")";

        // parcourir les sommets de la liste nv et voir s'il y a le même
        Collection<MeshVertex> siblings = vertexlist.get(nv);
        if (siblings != null) {
            for (MeshVertex other: siblings) {
                // si les noms sont égaux, c'est le même sommet
                if (name.equals(other.getName())) return other;
            }
        } else {
            // initialiser la liste des sommets ayant l'indice nv
            siblings = new LinkedList<>();
            vertexlist.put(nv, siblings);
        }

        // il faut créer un nouveau sommet car soit nouveau, soit un peu différent des autres
        MeshVertex vertex = m_Mesh.addVertex(name);
        vertex.setCoord(coordlist.get(nv));
        if (nt >= 0) vertex.setTexCoord(texcoordlist.get(nt));
        if (nn >= 0) vertex.setNormal(normallist.get(nn));

        // on ajoute ce sommet dans la liste de ceux qui ont le même numéro nv
        siblings.add(vertex);
        return vertex;
    }


    /**
     * Charge les données du fichier OBJ indiqué et applique une homothétie aux sommets.
     * @param objfilename : nom complet du fichier à charger
     * @param materialname : nom du matériau à lire, les autres sont ignorés, s'il est null, tous sont acceptés
     * @param scale : rapport d'agrandissement à appliquer
     * @throws Exception
     */
    public void loadObjFile(String objfilename, String materialname, float scale) throws Exception
    {
        // table des sommets qu'on va extraire du fichier obj, ils sont groupés par indice nv
        SparseArray<Collection<MeshVertex>> vertexlist = new SparseArray<>();

        // tableau des coordonnées, des textures et des normales
        ArrayList<vec3> coordlist    = new ArrayList<>();
        ArrayList<vec2> texcoordlist = new ArrayList<>();
        ArrayList<vec3> normallist   = new ArrayList<>();
        boolean computeNormals = true;

        // matériau à prendre en compte (tous si materialname est null)
        String usemtl = null;

        // ouverture du fichier
        Context context = ApplicationContext.getContext();
        InputStream data = context.getAssets().open(objfilename);
        BufferedReader br = new BufferedReader(new InputStreamReader(data));

        // parcourir le fichier obj ligne par ligne
        String line;
        while ((line = br.readLine()) != null) {
            // transformer les tabulations en espaces
            line = line.trim();
            line = line.replaceAll("\\s\\s", " ");
            // extraire le premier mot de la ligne
            String[] words = line.split(" ");
            if (words[0].equals("f") && words.length > 3) {
                // le matériau est-il celui qu'on veut ?
                if (materialname != null && ! materialname.equals(usemtl)) continue;
                // lire les numéros du premier point
                MeshVertex v1 = findOrCreateVertex(words[1], vertexlist, coordlist, texcoordlist, normallist);
                // lire les numéros du deuxième point
                MeshVertex v2 = findOrCreateVertex(words[2], vertexlist, coordlist, texcoordlist, normallist);
                // lire et traiter les points suivants
                for (int i=3; i<words.length; i++) {
                    MeshVertex v3 = findOrCreateVertex(words[i], vertexlist, coordlist, texcoordlist, normallist);
                    // ajouter un triangle v1,v2,v3
                    m_Mesh.addTriangle(v1,v2,v3);
                    // préparer le passage au triangle suivant
                    v2 = v3;
                }
            } else
            if (words[0].equals("v")) {
                // coordonnées 3D d'un sommet
                float x = Float.parseFloat(words[1]);
                float y = Float.parseFloat(words[2]);
                float z = Float.parseFloat(words[3]);
                vec3 coords = vec3.fromValues(x,y,z);
                vec3.scale(coords, coords, scale);
                coordlist.add(coords);
            } else
            if (words[0].equals("vt")) {
                // coordonnées de texture
                float u = Float.parseFloat(words[1]);
                float v = Float.parseFloat(words[2]);
                texcoordlist.add(vec2.fromValues(u,v));
            } else
            if (words[0].equals("vn")) {
                // normale
                float nx = Float.parseFloat(words[1]);
                float ny = Float.parseFloat(words[2]);
                float nz = Float.parseFloat(words[3]);
                normallist.add(vec3.fromValues(nx,ny,nz));
                computeNormals = false;
            } else
            if (words[0].equals("usemtl")) {
                usemtl = words[1];
            }
        }
        if (computeNormals) m_Mesh.computeNormals();
    }
}
