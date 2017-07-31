package com.dbooker.livreopengl;

import static android.opengl.GLES20.*;

import com.dbooker.livreopengl.glmatrix.mat3;
import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.material.VBOset;
import com.dbooker.livreopengl.mesh.MeshVertex;
import com.dbooker.livreopengl.material.Material;
import com.dbooker.livreopengl.material.Texture2D;
import com.dbooker.livreopengl.utils.Utils;


public class GroundMaterial extends Material
{
    // caractéristiques du matériau
    private Texture2D m_TxHeightmap;
    private Texture2D m_TxDiffuse;
    private float m_HMax;
    private float m_Delta;
    private vec3 m_Ks;
    private float m_Ns;

    // identifiants liés au shader
    private int m_TxDiffuseLoc;
    private int m_TxHeightmapLoc;

    /*** MODE MRT SIMULÉ ***/
    /** mode MRT émulé sur Android (rustine en attendant des GPU complets) **/
    private int m_NoMRTTxDiffuseLoc[];
    private int m_NoMRTTxHeightmapLoc[];
    /*** MODE MRT SIMULÉ ***/


    /**
     * Cette fonction définit la classe GroundMaterial pour dessiner le terrain.
     * @param heightmap : nom d'un fichier image contenant le relief
     * @param hmax : float qui donne la hauteur relative du terrain, ex: 0.4
     * @param delta : float qui indique la distance pour calculer la normale
     * @param diffuse : nom d'un fichier image contenant la texture diffuse
     * @param Ks : vec3
     * @param Ns : poli du matériau
     * @throws Exception
     */
    public GroundMaterial(String heightmap, float hmax, float delta, String diffuse, vec3 Ks, float Ns) throws Exception
    {
        super("GroundMaterial");

        // charger le relief et la texture diffuse
        m_TxHeightmap = new Texture2D(heightmap);
        m_TxDiffuse = new Texture2D(diffuse);
        m_TxDiffuseLoc = -1;
        m_TxHeightmapLoc = -1;

        m_HMax = hmax;
        m_Delta = delta;
        m_Ks = Ks;
        m_Ns = Ns;

        /*** MODE MRT SIMULÉ ***/
        if (GBufferSize > 0) {
            m_NoMRTTxDiffuseLoc = new int[GBufferSize];
            m_NoMRTTxHeightmapLoc = new int[GBufferSize];
            for (int i = 0; i < GBufferSize; i++) {
                m_NoMRTTxDiffuseLoc[i] = -1;
                m_NoMRTTxHeightmapLoc[i] = -1;
            }
        }
        /*** MODE MRT SIMULÉ ***/

        // compiler le shader
        compileShader();
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
            "// attributs de sommets\n" +
            "in vec3 glVertex;\n" +
            "in vec2 glTexCoord;\n" +
            "\n" +
            "// paramètres du matériau\n" +
            "const float delta = "+m_Delta+";\n" +
            "const float hmax = "+m_HMax+";\n" +
            "uniform sampler2D txHeightmap;\n" +
            "\n" +
            "// interpolation pour le fragment shader\n" +
            "out vec2 frgTexCoord;\n" +
            "out vec4 frgPosition;\n" +
            "out vec3 frgNormal;\n" +
            "\n" +
            "// matrices de transformation\n" +
            "uniform mat4 mat4ModelView;\n" +
            "uniform mat4 mat4Projection;\n" +
            "uniform mat3 mat3Normal;\n" +
            "void main()\n" +
            "{\n" +
            "    // transformation du point par la heightmap\n" +
            "    vec3 position = glVertex;\n" +
            "    float height = texture(txHeightmap, glTexCoord).g * hmax;\n" +
            "    position.y += height;\n" +
            "    // position du fragment par rapport à la caméra et projection écran\n" +
            "    frgPosition = mat4ModelView * vec4(position, 1.0);\n" +
            "    gl_Position = mat4Projection * frgPosition;\n" +
            "    // détermination de la normale\n" +
            "    float heightN = texture(txHeightmap, glTexCoord+vec2(0.0,+delta)).g;\n" +
            "    float heightS = texture(txHeightmap, glTexCoord+vec2(0.0,-delta)).g;\n" +
            "    float heightE = texture(txHeightmap, glTexCoord+vec2(+delta,0.0)).g;\n" +
            "    float heightW = texture(txHeightmap, glTexCoord+vec2(-delta,0.0)).g;\n" +
            "    float dX = (heightE - heightW) * hmax;\n" +
            "    float dZ = (heightS - heightN) * hmax;\n" +
            "    vec3 N = vec3(-dX, 2.0*delta, -dZ);\n" +
            "    frgNormal = mat3Normal * N;\n" +
            "    // coordonnées de texture\n" +
            "    frgTexCoord = glTexCoord;\n" +
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
        String fragdata1 = "vec4(0.0)";
        if (m_Ns >= 0.0) {
            fragdata1 = "vec4(Ks, Ns)";
        }
        String srcFragmentShader = "";
        srcFragmentShader += "#version 310 es\n";
        srcFragmentShader += "precision mediump float;\n";
        srcFragmentShader += "in vec4 frgPosition;\n";
        srcFragmentShader += "in vec3 frgNormal;\n";
        srcFragmentShader += "in vec2 frgTexCoord;\n";
        srcFragmentShader += "uniform sampler2D txDiffuse;\n";
        /*** MODE MRT SIMULÉ ***/
        if (num>=0) {
            srcFragmentShader += "out vec4 glFragColor;\n";
        } else {
        /*** MODE MRT SIMULÉ ***/
        srcFragmentShader += "out vec4 glFragData[4];\n";
        /*** MODE MRT SIMULÉ ***/
        }
        /*** MODE MRT SIMULÉ ***/
        srcFragmentShader += "void main()\n";
        srcFragmentShader += "{\n";
        srcFragmentShader += "    // couleur diffuse\n";
        srcFragmentShader += "    vec4 Kd = texture(txDiffuse, frgTexCoord);\n";
        srcFragmentShader += "    // couleur spéculaire\n";
        srcFragmentShader += "    const vec3 Ks = "+vec3.str(m_Ks)+";\n";
        srcFragmentShader += "    const float Ns = float("+m_Ns+");\n";
        srcFragmentShader += "    // calcul du vecteur normal\n";
        srcFragmentShader += "    vec3 N = normalize(frgNormal);\n";
        /*** MODE MRT SIMULÉ ***/
        switch (num) {
            case 0:
                srcFragmentShader += "    glFragColor = Kd;\n";
                break;
            case 1:
                srcFragmentShader += "    glFragColor = "+fragdata1+";\n";
                break;
            case 2:
                srcFragmentShader += "    glFragColor = frgPosition;\n";
                break;
            case 3:
                srcFragmentShader += "    glFragColor = vec4(N, 0.0);\n";
                break;
            default:
        /*** MODE MRT SIMULÉ ***/
        srcFragmentShader += "    glFragData[0] = Kd;\n";
        srcFragmentShader += "    glFragData[1] = "+fragdata1+";\n";
        srcFragmentShader += "    glFragData[2] = vec4(frgPosition.xyz, 1.0);\n";
        srcFragmentShader += "    glFragData[3] = vec4(N, 0.0);\n";
        /*** MODE MRT SIMULÉ ***/
                break;
        }
        /*** MODE MRT SIMULÉ ***/
        srcFragmentShader += "}";
        return srcFragmentShader;
    }


    /**
     * recompile le shader du matériau
     */
    @Override
    public void compileShader() throws Exception
    {
        // appeler la méthode de la superclasse
        super.compileShader();

        /*** MODE MRT SIMULÉ ***/
        if (GBufferSize > 0) {
            for (int i = 0; i < GBufferSize; i++) {
            // déterminer où sont les variables uniform spécifiques
            m_NoMRTTxDiffuseLoc[i]   = glGetUniformLocation(m_NoMRTShaderId[i], "txDiffuse");
            m_NoMRTTxHeightmapLoc[i] = glGetUniformLocation(m_NoMRTShaderId[i], "txHeightmap");
            }
        } else {
        /*** MODE MRT SIMULÉ ***/

            // déterminer où sont les variables uniform spécifiques
            m_TxDiffuseLoc   = glGetUniformLocation(m_ShaderId, "txDiffuse");
            m_TxHeightmapLoc = glGetUniformLocation(m_ShaderId, "txHeightmap");

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

            // activer la texture d'altitude sur l'unité 0
            m_TxHeightmap.setTextureUnit(GL_TEXTURE0, m_NoMRTTxHeightmapLoc[c_NumBuffer]);

            // activer la texture diffuse sur l'unité 1
            m_TxDiffuse.setTextureUnit(GL_TEXTURE1, m_NoMRTTxDiffuseLoc[c_NumBuffer]);

        } else {
        /*** MODE MRT SIMULÉ ***/

            // activer la texture d'altitude sur l'unité 0
            m_TxHeightmap.setTextureUnit(GL_TEXTURE0, m_TxHeightmapLoc);

            // activer la texture diffuse sur l'unité 1
            m_TxDiffuse.setTextureUnit(GL_TEXTURE1, m_TxDiffuseLoc);

        /*** MODE MRT SIMULÉ ***/
        }
        /*** MODE MRT SIMULÉ ***/
    }


    /**
     * Cette méthode désactive le matériau
     * NB: le shader doit être activé
     */
    @Override
    public void disable()
    {
        // désactiver les textures
        m_TxHeightmap.setTextureUnit(GL_TEXTURE0);
        m_TxDiffuse.setTextureUnit(GL_TEXTURE1);

        // appeler la méthode de la superclasse
        super.disable();
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    @Override
    public void destroy()
    {
        m_TxDiffuse.destroy();
        m_TxHeightmap.destroy();
        super.destroy();
    }
}
