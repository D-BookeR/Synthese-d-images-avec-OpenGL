// Définition de la classe TextureMaterial, une spécialisation de Material
// Ce matériau permet de dessiner en mode Multiple Render Targets

package com.dbooker.livreopengl.material;


import static android.opengl.GLES20.*;

import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.material.VBOset;
import com.dbooker.livreopengl.mesh.MeshVertex;
import com.dbooker.livreopengl.utils.Utils;


@SuppressWarnings("unused")
public class TextureMaterial extends Material
{
    private Texture2D m_Texture;

    private int m_TextureLoc;

    /*** MODE MRT SIMULÉ ***/
    /** mode MRT émulé sur Android (rustine en attendant des GPU complets) **/
    protected int[] m_NoMRTTextureLoc;
    /*** MODE MRT SIMULÉ ***/

    /**
     * constructeur
     * @param texture du matériau
     */
    public TextureMaterial(Texture2D texture) throws Exception
    {
        super("TextureMaterial");

        // texture
        m_Texture = texture;

        /*** MODE MRT SIMULÉ ***/
        if (GBufferSize > 0) {
            m_NoMRTTextureLoc = new int[GBufferSize];
            for (int i = 0; i < GBufferSize; i++) {
                m_NoMRTTextureLoc[i] = -1;
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
        return
            "#version 310 es\n"+
            "\n"+
            "// paramètres uniform\n"+
            "uniform mat4 mat4ModelView;\n"+
            "uniform mat4 mat4Projection;\n"+
            "uniform mat3 mat3Normal;\n"+
            "\n"+
            "// attributs de sommets\n"+
            "in vec3 glVertex;\n"+
            "in vec3 glNormal;\n"+
            "in vec2 glTexCoord;\n"+
            "\n"+
            "// interpolation vers les fragments\n"+
            "out vec4 frgPosition;\n"+
            "out vec3 frgNormal;\n"+
            "out vec2 frgTexCoord;\n"+
            "\n"+
            "void main()\n"+
            "{\n"+
            "    frgPosition = mat4ModelView * vec4(glVertex, 1.0);\n"+
            "    gl_Position = mat4Projection * frgPosition;\n"+
            "    frgNormal   = mat3Normal * glNormal;\n"+
            "    frgTexCoord = glTexCoord;\n"+
            "}";
    }


    /**
     * retourne le source du Fragment Shader
     * @param num sert pour simuler le mode MRT sur les machines où il est absent
     */
    @Override
    protected String getFragmentShader(int num)
    {
        String srcFragmentShader = "";
        srcFragmentShader += "#version 310 es\n";
        if (m_ClipPlaneOn) {
            srcFragmentShader += "\n";
            srcFragmentShader += "// plan de coupe\n";
            srcFragmentShader += "uniform vec4 ClipPlane;\n";
        }
        srcFragmentShader += "\n";
        srcFragmentShader += "// interpolation vers les fragments\n";
        srcFragmentShader += "precision mediump float;\n";
        /*** MODE MRT SIMULÉ ***/
        if (num>=0) {
            srcFragmentShader += "out vec4 glFragColor;\n";
        } else {
        /*** MODE MRT SIMULÉ ***/
        srcFragmentShader += "out vec4 glFragData[4];\n";
        /*** MODE MRT SIMULÉ ***/
        }
        /*** MODE MRT SIMULÉ ***/
        srcFragmentShader += "in vec4 frgPosition;\n";
        srcFragmentShader += "in vec3 frgNormal;\n";
        srcFragmentShader += "in vec2 frgTexCoord;\n";
        srcFragmentShader += "\n";
        srcFragmentShader += "uniform sampler2D txColor;\n";
        srcFragmentShader += "\n";
        srcFragmentShader += "void main()\n";
        srcFragmentShader += "{\n";
        if (m_ClipPlaneOn) {
            srcFragmentShader += "    // plan de coupe\n";
            srcFragmentShader += "    if (dot(frgPosition, ClipPlane) < 0.0) discard;\n";
            srcFragmentShader += "\n";
        }
        /*** MODE MRT SIMULÉ ***/
        switch (num) {
            case 0:
                srcFragmentShader += "    glFragColor = texture(txColor, frgTexCoord);\n";
                break;
            case 1:
                srcFragmentShader += "    glFragColor = vec4(0.0);\n";
                break;
            case 2:
                srcFragmentShader += "    glFragColor = frgPosition;\n";
                break;
            case 3:
                srcFragmentShader += "    glFragColor = vec4(frgNormal, 0.0);\n";
                break;
            default:
        /*** MODE MRT SIMULÉ ***/
        srcFragmentShader += "    glFragData[0] = texture(txColor, frgTexCoord);\n";
        srcFragmentShader += "    glFragData[1] = vec4(0.0);\n";
        srcFragmentShader += "    glFragData[2] = vec4(frgPosition.xyz, 1.0);\n";
        srcFragmentShader += "    glFragData[3] = vec4(frgNormal, 0.0);\n";
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
                m_NoMRTTextureLoc[i] = glGetUniformLocation(m_NoMRTShaderId[i], "txColor");
            }
        } else {
        /*** MODE MRT SIMULÉ ***/

            // déterminer où sont les variables uniform spécifiques
            m_TextureLoc = glGetUniformLocation(m_ShaderId, "txColor");

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
        vboset.addAttribute(MeshVertex.ID_ATTR_NORMAL,   Utils.VEC3, "glNormal");
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

            // activer la texture sur l'unité 0
            m_Texture.setTextureUnit(GL_TEXTURE0, m_NoMRTTextureLoc[c_NumBuffer]);

        } else {
        /*** MODE MRT SIMULÉ ***/

            // activer la texture sur l'unité 0
            m_Texture.setTextureUnit(GL_TEXTURE0, m_TextureLoc);

        /*** MODE MRT SIMULÉ ***/
        }
        /*** MODE MRT SIMULÉ ***/
    }


    /**
     * Cette méthode désactive le matériau
     */
    @Override
    public void disable()
    {
        // désactiver la texture
        m_Texture.setTextureUnit(GL_TEXTURE0);

        // appeler la méthode de la superclasse
        super.disable();
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    @Override
    public void destroy()
    {
        super.destroy();
    }
}
