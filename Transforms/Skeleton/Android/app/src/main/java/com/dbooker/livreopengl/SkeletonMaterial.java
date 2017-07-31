package com.dbooker.livreopengl;

import static android.opengl.GLES20.*;

import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.glmatrix.vec4;
import com.dbooker.livreopengl.mesh.Mesh;
import com.dbooker.livreopengl.material.VBOset;
import com.dbooker.livreopengl.mesh.MeshVertex;
import com.dbooker.livreopengl.material.Material;
import com.dbooker.livreopengl.utils.Utils;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;


@SuppressWarnings("unused")
public class SkeletonMaterial extends Material
{
    private static final String TAG = "LivreOpenGL";

    /// paramètres de construction des shaders
    protected vec3 m_Ks;
    protected float m_Ns;

    /// jointures de ce matériau
    protected List<Joint> m_Joints;

    protected int m_MatsGlobalJointsLoc;

    /*** MODE MRT SIMULÉ ***/
    protected int[] m_NoMRTJointsGlobalMatrixLoc;
    /*** MODE MRT SIMULÉ ***/


    /**
     * Constructeur de la classe SkeletonMaterial. Ce matériau gère une hiérarchie
     * de Joint représentant un squelette animable
     * @param Ks : vec3
     * @param Ns : poli du matériau
     * @throws Exception
     */
    @SuppressWarnings("unchecked")
    public SkeletonMaterial(vec3 Ks, float Ns) throws Exception
    {
        super("SkeletonMaterial");

        // caractéristiques du matériau
        m_Ks = Ks;
        m_Ns = Ns;

        // liste des jointures, celles qui n'ont pas de parent sont des racines
        m_Joints = new LinkedList<>();

        /*** MODE MRT SIMULÉ ***/
        if (GBufferSize > 0) {
            m_NoMRTJointsGlobalMatrixLoc = new int[GBufferSize];
        }
        /*** MODE MRT SIMULÉ ***/

        // compiler le shader
        compileShader();
    }


    /**
     * crée une nouvelle jointure pour ce matériau
     * @param joint : jointure à ajouter
     * @return jointure à ajouter
     */
    public Joint addJoint(Joint joint) throws Exception
    {
        // l'ajouter à la liste
        m_Joints.add(joint);

        // recompiler le shader car le nombre de jointures a changé
        compileShader();

        return joint;
    }


    /**
     * calcule les poids des sommets du maillage
     * @param mesh : maillage concerné
     */
    public void computeWeights(Mesh mesh) throws Exception
    {
        if (m_Joints.size() == 0) throw new Exception("Il n'y a aucune joint");
        // traiter tous les sommets
        for (MeshVertex vertex : mesh.getVertexList()) {
            // initialiser les poids à 0
            for (Joint joint : m_Joints) {
                joint.setWeight(0.0f);
            }
            // calculer le poids du sommet compte tenu de toutes les jointures (les parents sont calculés avant les enfants)
            vec3 coords = vertex.getCoord();
            for (Joint joint: m_Joints) {
                joint.computeWeights(coords);
            }
            // préparer les attributs poids et identifiants du sommet
            computeWeightsVertex(vertex);
        }
    }


    /**
     * affecte les attributs ID_ATTR_IDBONES et ID_ATTR_WEIGHTS d'un sommet
     * @param vertex : sommet dont il faut affecter les poids et numéros de jointures
     */
    protected void computeWeightsVertex(MeshVertex vertex)
    {
        // tableau des numéros de jointures et des poids du sommet
        ArrayList<Float> JointsIndex = new ArrayList<>();
        ArrayList<Float> JointsWeight = new ArrayList<>();
        // passer les jointures en revue, garder celles dont les poids ne sont pas nuls
        float sum = 0.0f;
        int num = 0;
        for (Joint joint : m_Joints) {
            float weight = joint.getWeight();
            if (weight > 0.0f) {
                // ajouter le poids et le numéro à la fin
                JointsIndex.add((float) num);
                JointsWeight.add(weight);
                sum += weight;
            }
            num++;
        }
        // modifier les poids si leur somme n'est pas 1.0
        if (sum == 0.0) {
            // forcer la dépendance totale à la première jointure
            JointsIndex.add(0.0f);
            JointsWeight.add(1.0f);
        } else if (sum != 1.0) {
            for (int i = 0; i < JointsWeight.size(); i++) {
                JointsWeight.set(i, JointsWeight.get(i) / sum);
            }
        }
        // compléter les listes à 4 éléments
        for (int i = JointsIndex.size(); i < 4; i++) {
            JointsIndex.add(0.0f);
            JointsWeight.add(0.0f);
        }
        // enregistrer ces informations dans le sommet
        vertex.setAttribute(MeshVertex.ID_ATTR_IDBONES, vec4.fromValues(JointsIndex.get(0), JointsIndex.get(1), JointsIndex.get(2), JointsIndex.get(3)));
        vertex.setAttribute(MeshVertex.ID_ATTR_WEIGHTS, vec4.fromValues(JointsWeight.get(0), JointsWeight.get(1), JointsWeight.get(2), JointsWeight.get(3)));
    }



    /**
     * recopie le poids d'une des jointures dans l'attribut ID_ATTR_COLOR
     * @param mesh : maillage contenant les sommets à colorer
     * @param joint : jointure concernée
     */
    public void debugWeights(Mesh mesh, Joint joint)
    {
        for (MeshVertex vertex: mesh.getVertexList()) {
            // recalculer le poids de ce sommet pour la jointure
            vec3 coords = vertex.getCoord();
            for (Joint j: m_Joints) {
                j.computeWeights(coords);
            }
            // transformer le poids en couleur
            float weight = joint.getWeight();
            vec4 color = vec4.create();
            vec4.lerp(color, vec4.fromValues(0,0,1,1), vec4.fromValues(1,0,0,1), weight);
            vertex.setColor(color);
        }
    }



    /**
     * calcule les matrices de transformation des jointures
     */
    public void computeGlobalMatrices()
    {
        // calculer les matrices absolues des jointures
        for (Joint joint: m_Joints) {
            joint.computeGlobalMatrix();
        }
    }


    /** destructeur */
    public void destroy()
    {
        super.destroy();
    }


    /**
     * retourne le source du Vertex Shader
     */
    @Override
    protected String getVertexShader()
    {
        String srcVertexShader =
            "#version 310 es\n" +
            "\n" +
            "// paramètres uniform\n" +
            "uniform mat4 mat4ModelView;\n" +
            "uniform mat4 mat4Projection;\n" +
            "uniform mat3 mat3Normal;\n" +
            "\n" +
            "// attributs de sommets\n" +
            "in vec3 glVertex;\n" +
            "in vec3 glNormal;\n" +
            "in vec4 glColor;\n" +
            "in vec4 JointsIndex;    // numéro des jointures de ce sommet\n" +
            "in vec4 JointsWeight;      // poids des jointures pour ce sommet\n" +
            "\n" +
            "// interpolation vers les fragments\n" +
            "out vec4 frgPosition;\n" +
            "out vec3 frgNormal;\n" +
            "out vec4 frgColor;\n" +
            "\n" +
            "// définition des os du squelette\n" +
            "const int N = "+Math.max(1, m_Joints.size())+";\n" +
            "uniform mat4 JointsGlobalMatrix[N];\n" +
            "\n" +
            "void main()\n" +
            "{\n" +
            "    // coordonnées et normale initiale\n" +
            "    vec4 vertex_orig = vec4(glVertex, 1.0);\n" +
            "    vec4 normal_orig = vec4(glNormal, 0.0);\n" +
            "\n" +
            "    // calculer la transformation par sa première jointure\n" +
            "    int num = int(JointsIndex.x);\n" +
            "    float weight = JointsWeight.x;\n" +
            "    mat4 matrix = JointsGlobalMatrix[num];\n" +
            "    vec4 vertex = matrix * vertex_orig * weight;\n" +
            "    vec4 normal = matrix * normal_orig * weight;\n" +
            "\n" +
            "    // ajouter la transformation par sa deuxième jointure\n" +
            "    num = int(JointsIndex.y);\n" +
            "    weight = JointsWeight.y;\n" +
            "    matrix = JointsGlobalMatrix[num];\n" +
            "    vertex += matrix * vertex_orig * weight;\n" +
            "    normal += matrix * normal_orig * weight;\n" +
            "\n" +
            "    // ajouter la transformation par sa troisième jointure\n" +
            "    num = int(JointsIndex.z);\n" +
            "    weight = JointsWeight.z;\n" +
            "    matrix = JointsGlobalMatrix[num];\n" +
            "    vertex += matrix * vertex_orig * weight;\n" +
            "    normal += matrix * normal_orig * weight;\n" +
            "\n" +
            "    // ajouter la transformation par sa quatrième jointure\n" +
            "    num = int(JointsIndex.w);\n" +
            "    weight = JointsWeight.w;\n" +
            "    matrix = JointsGlobalMatrix[num];\n" +
            "    vertex += matrix * vertex_orig * weight;\n" +
            "    normal += matrix * normal_orig * weight;\n" +
            "\n" +
            "    // normaliser les coordonnées (perte de précision)\n" +
            "    vertex /= vertex.w;\n" +
            "\n" +
            "    // transformation en repère caméra et écran\n" +
            "    frgPosition = mat4ModelView * vertex;\n" +
            "    gl_Position = mat4Projection * frgPosition;\n" +
            "    frgNormal = mat3Normal * normal.xyz;\n" +
            "\n" +
            "    // couleur\n" +
            "    frgColor = glColor;\n" +
            "}";
        return srcVertexShader;
    }


    /**
     * retourne le source du Fragment Shader
     * @param num sert pour simuler le mode MRT sur les machines où il est absent
     */
    @Override
    protected String getFragmentShader(int num)
    {
        String srcFragmentShader = new String();
        srcFragmentShader += "#version 310 es\n";
        srcFragmentShader += "precision mediump float;\n";
        srcFragmentShader += "\n";
        srcFragmentShader += "// coordonnées et normale du fragment\n";
        srcFragmentShader += "in vec4 frgPosition;\n";
        srcFragmentShader += "in vec3 frgNormal;\n";
        srcFragmentShader += "in vec4 frgColor;\n";
        srcFragmentShader += "\n";
        srcFragmentShader += "// caractéristiques du matériau\n";
        srcFragmentShader += "const vec3 Ks = " + vec3.str(m_Ks) + ";\n";
        srcFragmentShader += "const float Ns = " + m_Ns + ";\n";
        srcFragmentShader += "\n";
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
        /*** MODE MRT SIMULÉ ***/
        switch (num) {
            case 0:
                srcFragmentShader += "    glFragColor = frgColor;\n";
                break;
            case 1:
                srcFragmentShader += "    glFragColor = vec4(Ks, Ns);\n";
                break;
            case 2:
                srcFragmentShader += "    glFragColor = frgPosition;\n";
                break;
            case 3:
                srcFragmentShader += "    glFragColor = vec4(normalize(frgNormal), 0.0);\n";
                break;
            default:
        /*** MODE MRT SIMULÉ ***/
                srcFragmentShader += "    glFragData[0] = frgColor;\n";
                srcFragmentShader += "    glFragData[1] = vec4(Ks, Ns);\n";
                srcFragmentShader += "    glFragData[2] = vec4(frgPosition.xyz, 1.0);\n";
                srcFragmentShader += "    glFragData[3] = vec4(normalize(frgNormal), 0.0);\n";
        /*** MODE MRT SIMULÉ ***/
                break;
        }
        /*** MODE MRT SIMULÉ ***/
        srcFragmentShader += "}\n";
        return srcFragmentShader;
    }


    /**
     * recompile le shader du matériau
     * @throws Exception
     */
    @Override
    protected void compileShader() throws Exception
    {
        // appeler la méthode de la superclasse
        super.compileShader();

        /*** MODE MRT SIMULÉ ***/
        if (GBufferSize > 0) {
            for (int i = 0; i < GBufferSize; i++) {
                // déterminer où sont les variables uniform
                m_NoMRTJointsGlobalMatrixLoc[i] = glGetUniformLocation(m_NoMRTShaderId[i], "JointsGlobalMatrix");
            }
        } else {
        /*** MODE MRT SIMULÉ ***/

            // déterminer où sont les variables uniform spécifiques
            m_MatsGlobalJointsLoc = glGetUniformLocation(m_ShaderId, "JointsGlobalMatrix");

        /*** MODE MRT SIMULÉ ***/
        }
        /*** MODE MRT SIMULÉ ***/
    }


    /**
     * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
     * @return le VBOset du matériau
     */
    @Override
    public VBOset createVBOset()
    {
        // créer le VBOset et spécifier les noms des attribute nécessaires à ce matériau
        VBOset vboset = super.createVBOset();
        vboset.addAttribute(MeshVertex.ID_ATTR_NORMAL,  Utils.VEC3, "glNormal");
        vboset.addAttribute(MeshVertex.ID_ATTR_COLOR,   Utils.VEC4, "glColor");
        vboset.addAttribute(MeshVertex.ID_ATTR_IDBONES, Utils.VEC4, "JointsIndex");
        vboset.addAttribute(MeshVertex.ID_ATTR_WEIGHTS, Utils.VEC4, "JointsWeight");
        return vboset;
    }


    /**
     * Cette méthode active le matériau : met en place son shader,
     * fournit les variables uniform qu'il demande
     * @param mat4Projection : mat4 contenant la projection
     * @param mat4ModelView : mat4 contenant la transformation vers la caméra
     */
    @Override
    public void enable(mat4 mat4Projection, mat4 mat4ModelView)
    {
        // appeler la méthode de la superclasse
        super.enable(mat4Projection, mat4ModelView);

        /*** MODE MRT SIMULÉ ***/
        if (GBufferSize>0 && c_NumBuffer>=0 && c_NumBuffer<GBufferSize) {

            // fournir les matrices des os
            for (int i=0; i<m_Joints.size(); i++) {
                mat4.glUniformMatrix(m_NoMRTJointsGlobalMatrixLoc[c_NumBuffer]+i, m_Joints.get(i).getGlobalMatrix());
            }

        } else {
        /*** MODE MRT SIMULÉ ***/

            // fournir les matrices des os
            for (int i=0; i<m_Joints.size(); i++) {
                mat4.glUniformMatrix(m_MatsGlobalJointsLoc+i, m_Joints.get(i).getGlobalMatrix());
            }

        /*** MODE MRT SIMULÉ ***/
        }
        /*** MODE MRT SIMULÉ ***/
    }
}
