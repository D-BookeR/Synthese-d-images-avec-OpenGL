/**
 * Définition de la classe Material, une base pour construire des matériaux
 */

package com.dbooker.livreopengl.material;

import static android.opengl.GLES20.*;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.HashMap;
import java.util.Locale;
import java.util.Map;

import android.content.Context;
import android.util.Log;

import com.dbooker.livreopengl.glmatrix.mat3;
import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.glmatrix.vec4;
import com.dbooker.livreopengl.material.VBOset;
import com.dbooker.livreopengl.mesh.MeshVertex;
import com.dbooker.livreopengl.utils.ApplicationContext;
import com.dbooker.livreopengl.utils.Utils;

// Définition de la classe abstraite Material


@SuppressWarnings("unused")
public abstract class Material
{
    private static final String TAG = "LivreOpenGL";

    /** nom du matériau **/
    protected String m_Name;

    /** identifiants liés au shader */
    protected int m_ShaderId;
    protected int m_MatPloc;
    protected int m_MatMVloc;
    protected int m_MatNloc;
    protected int m_ClipPlaneLoc;
    // NB: les attributs glVertex et autres sont dans le VBOset

    /** matrice normale */
    protected mat3 m_Mat3Normal;

    /** plan de coupe */
    protected boolean m_ClipPlaneOn;
    protected vec4 m_ClipPlane;


    /*** MODE MRT SIMULÉ ***/
    /** mode MRT émulé sur Android (rustine en attendant des GPU complets) **/
    /** GBufferSize vaut 0 si le GPU est capable de MRT, sinon c'est le nombre de buffers à dessiner un par un, l'un après l'autre */
    public static int GBufferSize = 0;
    /** numéro du buffer courant, -1 si MRT ok, sinon 0..GBufferSize-1 si dessins successifs */
    protected static int c_NumBuffer = -1;
    /* identifiants des variables des shaders séparés dans le cas où le GPU n'est pas capable de MRT */
    protected int[] m_NoMRTShaderId;
    protected int[] m_NoMRTMatPloc;
    protected int[] m_NoMRTMatMVloc;
    protected int[] m_NoMRTMatNloc;
    protected int[] m_NoMRTClipPlaneLoc;


    /**
     * spécifie le nombre de buffers dans le cas d'un rendu différé en mode non MRT
     * @param num nombre de buffers
     */
    public static void setGBufferSize(int num)
    {
        GBufferSize = num;
    }
    /**
     * spécifie le buffer courant dans le cas d'un rendu différé en mode non MRT
     * @param num numéro du buffer 0..3 à activer, ou -1 pour les activer tous
     */
    public static void setMRTBuffer(int num)
    {
        if (num >= GBufferSize) {
            c_NumBuffer = -1;
        } else {
            c_NumBuffer = num;
        }
    }
    /*** MODE MRT SIMULÉ ***/


    /**
     * constructeur
     * NB: chaque sous-classe doit appeler elle-même compileShader() pour créer le shader
     * @param nom : nom du matériau
     */
    protected Material(String nom)
    {
        // nom du matériau
        m_Name = nom;

        // identifiant OpenGL du shader
        m_ShaderId = 0;

        // emplacement des matrices de changement de repère
        m_MatPloc = -1;
        m_MatMVloc = -1;
        m_MatNloc  = -1;

        // matrice normale
        m_Mat3Normal = mat3.create();

        // plan de coupe par défaut à l'infini
        m_ClipPlaneOn = false;
        m_ClipPlane = vec4.fromValues(0,0,1,1e38);
        m_ClipPlaneLoc = -1;


        /*** MODE MRT SIMULÉ ***/
        // rustine pour faire du MRT sans qu'il y en ait
        if (GBufferSize > 0) {
            m_NoMRTShaderId = new int[GBufferSize];
            m_NoMRTMatPloc  = new int[GBufferSize];
            m_NoMRTMatMVloc = new int[GBufferSize];
            m_NoMRTMatNloc  = new int[GBufferSize];
            m_NoMRTClipPlaneLoc = new int[GBufferSize];
            for (int i = 0; i < GBufferSize; i++) {
                m_NoMRTShaderId[i] = 0;
                m_NoMRTMatPloc[i] = -1;
                m_NoMRTMatMVloc[i] = -1;
                m_NoMRTMatNloc[i] = -1;
                m_NoMRTClipPlaneLoc[i] = -1;
            }
        }
        /*** MODE MRT SIMULÉ ***/

        // note : ce constructeur ne compile pas le shader, c'est aux sous-classes finales de le faire
    }


    /** destructeur */
    public void destroy()
    {
        // supprimer le shader
        if (m_ShaderId > 0) Utils.deleteShaderProgram(m_ShaderId);


        /*** MODE MRT SIMULÉ ***/
        // rustine pour faire du MRT sans qu'il y en ait
        for (int i = 0; i < GBufferSize; i++) {
            if (m_NoMRTShaderId[i] > 0) Utils.deleteShaderProgram(m_NoMRTShaderId[i]);
        }
        /*** MODE MRT SIMULÉ ***/
    }


    /**
     * retourne le nom du matériau
     * @return nom du matériau
     */
    public String getName()
    {
        return m_Name;
    }


    /**
     * retourne l'identifiant du shader de ce matériau
     * @return identifiant du shader
     */
    public int getShaderId()
    {
        /*** MODE MRT SIMULÉ ***/
        if (GBufferSize>0 && c_NumBuffer>=0 && c_NumBuffer<GBufferSize) {
            return m_NoMRTShaderId[c_NumBuffer];
        } else {
        /*** MODE MRT SIMULÉ ***/

            return m_ShaderId;

        /*** MODE MRT SIMULÉ ***/
        }
        /*** MODE MRT SIMULÉ ***/
    }


    /**
     * retourne le source du Vertex Shader
     */
    protected String getVertexShader()
    {
        String srcVertexShader =
            "#version 310 es\n"+
            "in vec3 glVertex;\n"+
            "uniform mat4 mat4Projection;\n"+
            "uniform mat4 mat4ModelView;\n"+
            "out vec4 frgPosition;\n"+
            "void main()\n"+
            "{\n"+
            "    frgPosition = mat4ModelView * vec4(glVertex, 1.0);\n"+
            "    gl_Position = mat4Projection * frgPosition;\n"+
            "}";
        return srcVertexShader;
    }


    /**
     * retourne le source du Fragment Shader
     * @param num sert pour simuler le mode MRT sur les machines où il est absent
     */
    protected String getFragmentShader(int num)
    {
        String srcFragmentShader = "";
        srcFragmentShader += "#version 310 es\n";
        srcFragmentShader += "precision mediump float;\n";
        if (this.m_ClipPlaneOn) {
            srcFragmentShader += "\n";
            srcFragmentShader += "// plan de coupe\n";
            srcFragmentShader += "uniform vec4 ClipPlane;\n";
        }
        srcFragmentShader += "\n";
        srcFragmentShader += "in vec4 frgPosition;\n";
        /*** MODE MRT SIMULÉ ***/
        if (num>=0) {
            srcFragmentShader += "out vec4 glFragColor;\n";
        } else {
        /*** MODE MRT SIMULÉ ***/
        srcFragmentShader += "out vec4 glFragData[4];\n";
        /*** MODE MRT SIMULÉ ***/
        }
        /*** MODE MRT SIMULÉ ***/
        srcFragmentShader += "\n";
        srcFragmentShader += "void main()\n";
        srcFragmentShader += "{\n";
        if (this.m_ClipPlaneOn) {
            srcFragmentShader += "    // plan de coupe\n";
            srcFragmentShader += "    if (dot(frgPosition, ClipPlane) < 0.0) discard;\n";
            srcFragmentShader += "\n";
        }
        /*** MODE MRT SIMULÉ ***/
        switch (num) {
            case 0:
                srcFragmentShader += "    glFragColor = vec4(1.0, 0.0, 1.0, 1.0);\n";
                break;
            case 1:
                srcFragmentShader += "    glFragColor = vec4(0.0);\n";
                break;
            case 2:
                srcFragmentShader += "    glFragColor = frgPosition;\n";
                break;
            case 3:
                srcFragmentShader += "    glFragColor = vec4(1.0, 1.0, 1.0, 1.0);\n";
                break;
            default:
        /*** MODE MRT SIMULÉ ***/
                srcFragmentShader += "    glFragData[0] = vec4(1.0, 0.0, 1.0, 1.0);\n";
                srcFragmentShader += "    glFragData[1] = vec4(0.0);\n";
                srcFragmentShader += "    glFragData[2] = vec4(frgPosition.xyz, 1.0);\n";
                srcFragmentShader += "    glFragData[3] = vec4(1.0, 1.0, 1.0, 1.0);\n";
        /*** MODE MRT SIMULÉ ***/
                break;
        }
        /*** MODE MRT SIMULÉ ***/
        srcFragmentShader += "}";
        return srcFragmentShader;
    }
    protected String getFragmentShader()
    {
        return getFragmentShader(-1);
    }


    /**
     * recompile le shader du matériau
     * @throws Exception
     */
    protected void compileShader() throws Exception
    {
        /*** MODE MRT SIMULÉ ***/
        if (GBufferSize > 0) {
            for (int i = 0; i < GBufferSize; i++) {
                // supprimer l'ancien shader s'il y en avait un
                if (m_NoMRTShaderId[i] != 0) Utils.deleteShaderProgram(m_NoMRTShaderId[i]);

                // construire le vertex shader
                String srcVertexShader = getVertexShader();

                // construire le fragment shader
                String srcFragmentShader = getFragmentShader(i);

                // compiler et lier les shaders
                m_NoMRTShaderId[i] = Utils.makeShaderProgram(srcVertexShader, srcFragmentShader, m_Name);

                // déterminer où sont les variables uniform
                m_NoMRTMatPloc[i]      = glGetUniformLocation(m_NoMRTShaderId[i], "mat4Projection");
                m_NoMRTMatMVloc[i]     = glGetUniformLocation(m_NoMRTShaderId[i], "mat4ModelView");
                m_NoMRTMatNloc[i]      = glGetUniformLocation(m_NoMRTShaderId[i], "mat3Normal");
                m_NoMRTClipPlaneLoc[i] = glGetUniformLocation(m_NoMRTShaderId[i], "ClipPlane");
            }

        } else {
        /*** MODE MRT SIMULÉ ***/

            // supprimer l'ancien shader s'il y en avait un
            if (m_ShaderId != 0) Utils.deleteShaderProgram(m_ShaderId);

            // construire le vertex shader
            String srcVertexShader = getVertexShader();

            // construire le fragment shader
            String srcFragmentShader = getFragmentShader();

            // compiler et lier les shaders
            m_ShaderId = Utils.makeShaderProgram(srcVertexShader, srcFragmentShader, m_Name);

            // déterminer où sont les variables uniform
            m_MatPloc      = glGetUniformLocation(m_ShaderId, "mat4Projection");
            m_MatMVloc     = glGetUniformLocation(m_ShaderId, "mat4ModelView");
            m_MatNloc      = glGetUniformLocation(m_ShaderId, "mat3Normal");
            m_ClipPlaneLoc = glGetUniformLocation(m_ShaderId, "ClipPlane");
        /*** MODE MRT SIMULÉ ***/
        }
        /*** MODE MRT SIMULÉ ***/
    }


    /**
     * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
     * @return le VBOset du matériau
     */
    public VBOset createVBOset()
    {
        // créer le VBOset et spécifier les noms des attribute nécessaires à ce matériau
        VBOset vboset = new VBOset(this);
        vboset.addAttribute(MeshVertex.ID_ATTR_VERTEX,   Utils.VEC3, "glVertex");
        return vboset;
    }


    /**
     * Cette méthode active le matériau : met en place son shader,
     * fournit les variables uniform qu'il demande
     * @param mat4Projection : mat4 contenant la projection
     * @param mat4ModelView : mat4 contenant la transformation vers la caméra
     */
    public void enable(mat4 mat4Projection, mat4 mat4ModelView)
    {
        // activer le shader
        /*** MODE MRT SIMULÉ ***/
        if (GBufferSize>0 && c_NumBuffer>=0 && c_NumBuffer<GBufferSize) {
            glUseProgram(m_NoMRTShaderId[c_NumBuffer]);

            // fournir les matrices MV et P
            mat4.glUniformMatrix(m_NoMRTMatPloc[c_NumBuffer], mat4Projection);
            mat4.glUniformMatrix(m_NoMRTMatMVloc[c_NumBuffer], mat4ModelView);

            // calculer et fournir la matrice normale
            if (m_NoMRTMatNloc[c_NumBuffer] >= 0) {
                mat3.fromMat4(m_Mat3Normal, mat4ModelView);
                mat3.transpose(m_Mat3Normal, m_Mat3Normal);
                mat3.invert(m_Mat3Normal, m_Mat3Normal);
                mat3.glUniformMatrix(m_NoMRTMatNloc[c_NumBuffer], m_Mat3Normal);
            }

            // si le plan de coupe est actif, alors le fournir
            if (m_ClipPlaneOn && m_NoMRTClipPlaneLoc[c_NumBuffer] >= 0) {
                vec4.glUniform(m_NoMRTClipPlaneLoc[c_NumBuffer], m_ClipPlane);
            }

        } else {
        /*** MODE MRT SIMULÉ ***/

            glUseProgram(m_ShaderId);

            // fournir les matrices MV et P
            mat4.glUniformMatrix(m_MatPloc, mat4Projection);
            mat4.glUniformMatrix(m_MatMVloc, mat4ModelView);

            // calculer et fournir la matrice normale au shader
            if (m_MatNloc >= 0) {
                mat3.fromMat4(m_Mat3Normal, mat4ModelView);
                mat3.transpose(m_Mat3Normal, m_Mat3Normal);
                mat3.invert(m_Mat3Normal, m_Mat3Normal);
                mat3.glUniformMatrix(m_MatNloc, m_Mat3Normal);
            }

            // si le plan de coupe est actif, alors le fournir
            if (m_ClipPlaneOn && m_ClipPlaneLoc >= 0) {
                vec4.glUniform(m_ClipPlaneLoc, m_ClipPlane);
            }

        /*** MODE MRT SIMULÉ ***/
        }
        /*** MODE MRT SIMULÉ ***/
    }


    /**
     * Cette méthode désactive le matériau
     * NB : le shader doit encore être activé
     */
    public void disable()
    {
        // désactiver le shader
        glUseProgram(0);
    }


    /**
     * définit un plan de coupe pour les fragments. Ce plan est en coordonnées caméra
     * @param active : true s'il faut compiler un shader gérant le plan de coupe
     * @param plane vec4 en coordonnées caméra ou null pour supprimer le plan de coupe
     * @throws Exception
     */
    public void setClipPlane(boolean active, vec4 plane) throws Exception
    {
        // il faut recompiler s'il y a un changement d'état
        boolean recompile = (active != m_ClipPlaneOn);

        m_ClipPlaneOn = active;
        m_ClipPlane = plane;

        // recompiler le shader
        if (recompile) compileShader();
    }


    /**
     * enlève le plan de coupe mais sans recompiler le shader (le plan est mis à l'infini)
     * @param active : true s'il faut compiler un shader gérant le plan de coupe
     * @throws Exception
     */
    public void setClipPlane(boolean active) throws Exception
    {
        // il faut recompiler s'il y a un changement d'état
        boolean recompile = (active != m_ClipPlaneOn);

        // nouvel état et plan à l'infini
        m_ClipPlaneOn = active;
        m_ClipPlane = vec4.fromValues(0,0,1,1e38);

        // recompiler le shader
        if (recompile) compileShader();
    }


    /**
     * désactive le plan de coupe
     * @throws Exception
     */
    public void resetClipPlane() throws Exception
    {
        boolean recompile = m_ClipPlaneOn;
        m_ClipPlaneOn = false;
        m_ClipPlane = vec4.fromValues(0,0,1,1e38);
        if (recompile) compileShader();
    }


    /**
     * Cette méthode crée une collection de DeferredShadingMaterial lue d'un fichier MTL pour un fichier OBJ.
     * NB : le chargement du fichier est fait en asynchrone, par une requête HTTP sur le serveur
     * @param folder : contient le fichier mtl et les textures
     * @param mtlfilename : nom seul du fichier au format MTL (ne pas mettre de chemin)
     * @throws Exception
     */
    public static Map<String, Material> loadMTL(String folder, String mtlfilename) throws Exception
    {
        // dictionnaire des matériaux du fichier => dans le modèle
        Map<String, MatDescription> mat_descriptions = new HashMap<>();

        // matériau courant
        MatDescription mat_description = null;

        // ouverture du fichier
        String fullpathname = folder + "/" + mtlfilename;
        Context context = ApplicationContext.getContext();
        try {
            InputStream data = context.getAssets().open(fullpathname);
            BufferedReader br = new BufferedReader(new InputStreamReader(data));

            // parcourir le fichier mtl ligne par ligne
            String line;
            while ((line = br.readLine()) != null) {

                // transformer les tabulations en espaces
                line = line.replaceAll("\\s\\s", " ");
                line = line.trim();

                // extraire le premier mot de la ligne
                String[] words = line.split(" ");
                if (words.length < 1) continue;
                String word = words[0];

                // mettre le mot en minuscules
                word = word.toLowerCase(Locale.getDefault());

                if (word.equals("newmtl")) {
                    // créer une spécif de matériau et l'ajouter aux spécifs
                    mat_description = new MatDescription();
                    mat_descriptions.put(words[1], mat_description);

                } else if (mat_description != null) {
                    float r,g,b;
                    switch (word) {
                    case"map_kd":
                        // construire le nom de la texture
                        mat_description.map_Kd = folder + "/" + words[1];
                        break;
                    case "kd":
                        r = Float.parseFloat(words[1]);
                        g = Float.parseFloat(words[2]);
                        b = Float.parseFloat(words[3]);
                        mat_description.Kd = vec4.fromValues(r, g, b, 1.0);
                        break;
                    case "ks":
                        r = Float.parseFloat(words[1]);
                        g = Float.parseFloat(words[2]);
                        b = Float.parseFloat(words[3]);
                        mat_description.Ks = vec3.fromValues(r, g, b);
                        break;
                    case "ns":
                        mat_description.Ns = Float.parseFloat(words[1]);
                        break;
                    case "d":
                        float a = Float.parseFloat(words[1]);
                        vec4.set(mat_description.Kd, 3, a);
                        break;
                    default:
                    }
                }
            }
        } catch (Exception e) {
            Log.e(TAG, "Le fichier \""+fullpathname+"\" ne peut pas être ouvert");
        }

        // repasser sur tous les matériaux et les créer effectivement
        Map<String, Material> materials = new HashMap<>();
        for (String mat_name: mat_descriptions.keySet()) {
            mat_description = mat_descriptions.get(mat_name);
            if (mat_description.map_Kd.isEmpty()) {
                materials.put(mat_name, new DeferredShadingMaterial(mat_description.Kd, mat_description.Ks, mat_description.Ns));
            } else {
                materials.put(mat_name, new DeferredShadingMaterial(mat_description.map_Kd, mat_description.Ks, mat_description.Ns));
            }
        }

        // retourner la map des matériaux
        return materials;
    }

    // description d'un matériau, avant sa création
    private static class MatDescription {
        String map_Kd;
        vec4 Kd;
        vec3 Ks;
        float Ns;
        float d;
        MatDescription() {
            map_Kd = "";
            Kd = vec4.create();
            Ks = vec3.create();
            Ns = 1.0f;
        }
    }
}
