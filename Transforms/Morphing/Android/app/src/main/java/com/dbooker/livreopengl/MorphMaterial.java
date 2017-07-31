package com.dbooker.livreopengl;

import static android.opengl.GLES20.*;

import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.material.VBOset;
import com.dbooker.livreopengl.mesh.MeshVertex;
import com.dbooker.livreopengl.material.Material;
import com.dbooker.livreopengl.material.Texture2D;
import com.dbooker.livreopengl.utils.Utils;

@SuppressWarnings("unused")
public class MorphMaterial extends Material
{
    private static final String TAG = "LivreOpenGL";

    /// paramètres de construction des shaders
    protected vec3 m_Ks;
    protected float m_Ns;

    /** texture utilisée par le matériau **/
    protected Texture2D m_TxDiffuse;
    protected int m_TxDiffuseLoc;

    /** coefficient de morphing */
    protected float m_MorphCoef;
    protected int m_MorphCoefLoc;

    /*** MODE MRT SIMULÉ ***/
    /** mode MRT émulé sur Android (rustine en attendant des GPU complets) **/
    protected int[] m_NoMRTTxDiffuseLoc;
    protected int[] m_NoMRTMorphCoefLoc;
    /*** MODE MRT SIMULÉ ***/


    /**
     * constructeur
     * @param diffuse : nom d'une texture
     * @param Ks : vec3
     * @param Ns : poli du matériau
     * @throws Exception
     */
    public MorphMaterial(String diffuse, vec3 Ks, float Ns) throws Exception
    {
        super("MorphMaterial");

        /*** MODE MRT SIMULÉ ***/
        // rustine pour faire du MRT sans qu'il y en ait
        if (GBufferSize > 0) {
            m_NoMRTTxDiffuseLoc  = new int[GBufferSize];
            m_NoMRTMorphCoefLoc = new int[GBufferSize];
            for (int i = 0; i < GBufferSize; i++) {
                m_NoMRTTxDiffuseLoc[i] = -1;
                m_NoMRTMorphCoefLoc[i] = -1;
            }
        }
        /*** MODE MRT SIMULÉ ***/

            // texture diffuse
        m_TxDiffuse = new Texture2D(diffuse, GL_LINEAR, GL_REPEAT);

        // couleur spéculaire
        m_Ks = Ks;
        m_Ns = Ns;

        // coefficient de morphing
        m_MorphCoef = 0.0f;

        // compiler le shader
        compileShader();
    }


    /** destructeur */
    public void destroy()
    {
        // libérer les textures qui ont été chargées
        if (m_TxDiffuse != null) m_TxDiffuse.destroy();
        super.destroy();
    }


    /**
     * retourne le source du Vertex Shader
     */
    @Override
    protected String getVertexShader()
    {
        // début du vertex shader
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
            "in vec2 glTexCoord;\n" +
            "\n" +
            "// morph target\n" +
            "uniform float morphcoef;\n" +
            "in vec3 glVertex1;\n" +
            "in vec3 glNormal1;\n" +
            "\n" +
            "// interpolation vers les fragments\n" +
            "out vec4 frgPosition;\n" +
            "out vec3 frgNormal;\n" +
            "out vec2 frgTexCoord;\n" +
            "\n" +
            "void main()\n" +
            "{\n" +
            "    // interpolation entre référence et morph target\n" +
            "    vec3 vertex = mix(glVertex, glVertex1, morphcoef);\n" +
            "    vec3 normal = mix(glNormal, glNormal1, morphcoef);\n" +
            "\n" +
            "    // transformations pour l'affichage\n" +
            "    frgPosition = mat4ModelView * vec4(vertex, 1.0);\n" +
            "    gl_Position = mat4Projection * frgPosition;\n" +
            "    frgNormal = mat3Normal * normal;\n" +
            "    frgTexCoord = glTexCoord;\n" +
            "}\n";
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
        srcFragmentShader += "in vec2 frgTexCoord;\n";
        srcFragmentShader += "\n";
        srcFragmentShader += "// caractéristiques du matériau\n";
        srcFragmentShader += "uniform sampler2D txDiffuse;\n";
        srcFragmentShader += "const vec3 Ks = "+vec3.str(m_Ks)+";\n";
        srcFragmentShader += "const float Ns = "+m_Ns+";\n";
        // plan de coupe
        if (m_ClipPlaneOn) {
            srcFragmentShader += "\n";
            srcFragmentShader += "// plan de coupe\n";
            srcFragmentShader += "uniform vec4 ClipPlane;\n";
        }
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
        // plan de coupe
        if (m_ClipPlaneOn) {
            srcFragmentShader += "    if (dot(frgPosition, ClipPlane) < 0.0) discard;\n";
        }
        srcFragmentShader += "    vec4 Kd = texture(txDiffuse, frgTexCoord);\n";
        srcFragmentShader += "    // remplir les buffers avec les informations\n";
        /*** MODE MRT SIMULÉ ***/
        switch (num) {
            case 0:
                srcFragmentShader += "    glFragColor = Kd;\n";
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
                srcFragmentShader += "    glFragData[0] = Kd;\n";
                srcFragmentShader += "    glFragData[1] = vec4(Ks, Ns);\n";
                srcFragmentShader += "    glFragData[2] = vec4(frgPosition.xyz, 1.0);\n";
                srcFragmentShader += "    glFragData[3] = vec4(normalize(frgNormal), 0.0);\n";
        /*** MODE MRT SIMULÉ ***/
                break;
        }
        /*** MODE MRT SIMULÉ ***/
        srcFragmentShader += "}\n";
        return srcFragmentShader.toString();
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
                m_NoMRTTxDiffuseLoc[i] = glGetUniformLocation(m_NoMRTShaderId[i], "txDiffuse");
                m_NoMRTMorphCoefLoc[i] = glGetUniformLocation(m_NoMRTShaderId[i], "morphcoef");
            }
        } else {
        /*** MODE MRT SIMULÉ ***/

            // déterminer où sont les variables uniform spécifiques
            m_TxDiffuseLoc = glGetUniformLocation(m_ShaderId, "txDiffuse");
            m_MorphCoefLoc = glGetUniformLocation(m_ShaderId, "morphcoef");

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
        vboset.addAttribute(MeshVertex.ID_ATTR_COLOR,    Utils.VEC4, "glColor");
        vboset.addAttribute(MeshVertex.ID_ATTR_VERTEX1,  Utils.VEC3, "glVertex1");
        vboset.addAttribute(MeshVertex.ID_ATTR_NORMAL,   Utils.VEC3, "glNormal");
        vboset.addAttribute(MeshVertex.ID_ATTR_NORMAL1,  Utils.VEC3, "glNormal1");
        vboset.addAttribute(MeshVertex.ID_ATTR_TEXCOORD, Utils.VEC2, "glTexCoord");
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

            // activer la texture diffuse
            m_TxDiffuse.setTextureUnit(GL_TEXTURE0, m_NoMRTTxDiffuseLoc[c_NumBuffer]);

            // fournir le coefficient de morphing
            glUniform1f(m_NoMRTMorphCoefLoc[c_NumBuffer], m_MorphCoef);

        } else {
        /*** MODE MRT SIMULÉ ***/

            // activer la texture diffuse
            m_TxDiffuse.setTextureUnit(GL_TEXTURE0, m_TxDiffuseLoc);

            // fournir le coefficient de morphing
            glUniform1f(m_MorphCoefLoc, m_MorphCoef);

        /*** MODE MRT SIMULÉ ***/
        }
        /*** MODE MRT SIMULÉ ***/
    }


    /**
     * Cette méthode désactive le matériau
     * NB : le shader doit être activé
     */
    @Override
    public void disable()
    {
        // désactiver la texture diffuse
        m_TxDiffuse.setTextureUnit(GL_TEXTURE0);

        // appeler la méthode de la superclasse
        super.disable();
    }


    /**
     * définit la valeur du morph pour changer la forme de l'objet
     * @param coef entre 0 et 1
     */
    public void setMorphCoef(float coef)
    {
        m_MorphCoef = coef;
    }
}
