// Cette classe permet d'appliquer un traitement sur une image dans un FBO

package com.dbooker.livreopengl.process;

import android.util.Log;

import static android.opengl.GLES20.*;

import com.dbooker.livreopengl.utils.Utils;


@SuppressWarnings("unused")
public abstract class Process
{
    private static final String TAG = "LivreOpenGL";

    // VBO pour dessiner un rectangle occupant tout l'écran
    protected static int m_VertexBufferId;
    protected static int m_TexCoordBufferId;


    /**
     * crée les shaders et VBO de dessin
     */
    static {
        // créer et remplir les VBOs des coordonnées
        float[] vertices = { -1.0f,-1.0f,  +1.0f,-1.0f,  +1.0f,+1.0f,  -1.0f,+1.0f };
        m_VertexBufferId = Utils.makeFloatVBO(vertices, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
        float[] texcoords = {  0.0f, 0.0f,   1.0f, 0.0f,   1.0f, 1.0f,   0.0f, 1.0f };
        m_TexCoordBufferId = Utils.makeFloatVBO(texcoords, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
    }


    /**
     * supprime les shaders et VBO de dessin
     */
    protected static void staticdestroy()
    {
        Utils.deleteVBO(m_VertexBufferId);
        Utils.deleteVBO(m_TexCoordBufferId);
    }


    protected String m_Name;

    // shader et les emplacements de ses variables uniform
    protected int m_ShaderId;
    protected int m_VertexLoc;
    protected int m_TexCoordLoc;
    protected int m_ColorMapLoc;


    /**
     * constructeur
     * @param nom du traitement
     */
    public Process(String nom)
    {
        // initialisation
        m_Name = nom;
        m_ShaderId = 0;
        m_VertexLoc = 0;
        m_TexCoordLoc = 0;
        m_ColorMapLoc = 0;
    }


    /**
     * destructeur
     */
    public void destroy()
    {
        Utils.deleteShaderProgram(m_ShaderId);
    }


    /**
     * retourne le source du Vertex Shader commun à tous les traitements
     * NB : le Fragment Shader est spécifique à chaque traitement et défini dans la sous-classe
     */
    protected String getVertexShader()
    {
        String srcVertexShader =
            "#version 310 es\n" +
            "in vec2 glVertex;\n" +
            "in vec2 glTexCoord;\n" +
            "out vec2 frgTexCoord;\n" +
            "void main()\n" +
            "{\n" +
            "    gl_Position = vec4(glVertex, 0.0, 1.0);\n" +
            "    frgTexCoord = glTexCoord;\n" +
            "}";
        return srcVertexShader;
    }


    /**
     * retourne le source du Fragment Shader spécifique au traitement
     */
    protected abstract String getFragmentShader();


    /**
     * compile ou recompile le shader
     * @throws Exception
     */
    public void compileShader() throws Exception
    {
        // supprimer l'ancien shader s'il y en avait un
        if (m_ShaderId > 0) Utils.deleteShaderProgram(m_ShaderId);

        // compiler le shader
        String srcVertexShader = getVertexShader();
        String srcFragmentShader = getFragmentShader();
        m_ShaderId = Utils.makeShaderProgram(srcVertexShader, srcFragmentShader, m_Name);

        // déterminer où sont les variables attribute
        m_VertexLoc   = glGetAttribLocation(m_ShaderId, "glVertex");
        m_TexCoordLoc = glGetAttribLocation(m_ShaderId, "glTexCoord");

        // déterminer où sont les variables uniform (spécifiques à chaque traitement)
        findUniformLocations();
    }


    /**
     * détermine où sont les variables uniform du traitement
     */
    protected void findUniformLocations()
    {
        m_ColorMapLoc = glGetUniformLocation(m_ShaderId, "ColorMap");
    }


    /**
     * active le shader et les VBO pour appliquer le traitement défini dans le shader
     */
    protected void startProcess()
    {
        // activer le shader
        glUseProgram(m_ShaderId);

        // activer et lier le VBO contenant les coordonnées
        glEnableVertexAttribArray(m_VertexLoc);
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
        glVertexAttribPointer(m_VertexLoc, 2, GL_FLOAT, false, 0, 0);

        // activer et lier le VBO contenant les coordonnées de texture
        glEnableVertexAttribArray(m_TexCoordLoc);
        glBindBuffer(GL_ARRAY_BUFFER, m_TexCoordBufferId);
        glVertexAttribPointer(m_TexCoordLoc, 2, GL_FLOAT, false, 0, 0);
    }


    /**
     * désactive le shader et les VBO
     */
    protected void endProcess()
    {
        // désactiver les VBOs
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDisableVertexAttribArray(m_VertexLoc);
        glDisableVertexAttribArray(m_TexCoordLoc);

        // désactiver le shader
        glUseProgram(0);
    }


    /**
     * cette fonction associe l'une des textures de ce FBO à une unité pour un shader
     * NB : le shader concerné doit être actif
     * @param unit : unité de texture concernée, par exemple GL_TEXTURE0
     * @param locSampler : emplacement de la variable uniform sampler* de cette texture dans le shader ou -1 pour désactiver la texture
     * @param bufferId : m_ColorBuffer ou m_DepthBufferId ou 0 pour désactiver
     */
    public void setTextureUnit(int unit, int locSampler, int bufferId)
    {
        /*****DEBUG*****/
        if (unit < GL_TEXTURE0 || unit > GL_TEXTURE7) {
            Log.e(TAG, "Process.setTextureUnit: first parameter, unit is not GL_TEXTURE0 ... GL_TEXTURE7");
            return;
        }
        /*****DEBUG*****/
        glActiveTexture(unit);
        if (locSampler < 0 || bufferId <= 0) {
            glBindTexture(GL_TEXTURE_2D, 0);
        } else {
            glBindTexture(GL_TEXTURE_2D, bufferId);
            glUniform1i(locSampler, unit-GL_TEXTURE0);
        }
    }
    public void setTextureUnit(int unit)
    {
        /*****DEBUG*****/
        if (unit < GL_TEXTURE0 || unit > GL_TEXTURE7) {
            Log.e(TAG, "Process.setTextureUnit: first parameter, unit is not GL_TEXTURE0 ... GL_TEXTURE7");
            return;
        }
        /*****DEBUG*****/
        setTextureUnit(unit, -1, -1);
    }
}
