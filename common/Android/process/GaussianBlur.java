// Cette classe permet d'appliquer un flou gaussien sur un FBO
// voir https://github.com/mattdesl/lwjgl-basics/wiki/ShaderLesson5
// voir http://xissburg.com/faster-gaussian-blur-in-glsl/
// voir https://software.intel.com/en-us/blogs/2014/07/15/an-investigation-of-fast-real-time-gpu-based-image-blur-algorithms

package com.dbooker.livreopengl.process;

import com.dbooker.livreopengl.utils.Utils;

import static android.opengl.GLES20.*;


@SuppressWarnings("unused")
public class GaussianBlur extends Process
{
    protected FrameBufferObject m_FBO;

    protected int m_HorizShaderId;
    protected int m_VertiShaderId;

    protected int m_HorizVertexLoc;
    protected int m_VertiVertexLoc;
    protected int m_HorizTexCoordLoc;
    protected int m_VertiTexCoordLoc;
    protected int m_HorizColorMapLoc;
    protected int m_VertiColorMapLoc;
    protected int m_HorizTexelSizeLoc;
    protected int m_VertiTexelSizeLoc;


    public GaussianBlur(int width, int height) throws Exception
    {
        super("GaussianBlur");

        // créer un FBO pour le résultat intermédiaire
        m_FBO = new FrameBufferObject(width, height, GL_TEXTURE_2D, GL_NONE);

        // initialisations
        m_HorizShaderId = 0;
        m_VertiShaderId = 0;

        // il y a deux shaders : un pour flouter horizontalement, l'autre verticalement
        compileHorizShader();
        compileVertiShader();
    }


    /** destructeur */
    public void destroy()
    {
        m_FBO.destroy();
        Utils.deleteShaderProgram(m_HorizShaderId);
        Utils.deleteShaderProgram(m_VertiShaderId);

        super.destroy();
    }


    /**
     * retourne le source du Vertex Shader horizontal
     */
    protected String getHorizVertexShader()
    {
        return
            "#version 310 es\n" +
            "in vec2 glVertex;\n" +
            "in vec2 glTexCoord;\n" +
            "uniform float TexelSize;\n" +
            "out vec2 frgTexCoord;\n" +
            "out vec2 frgBlurTexCoord[14];\n" +
            "void main()\n" +
            "{\n" +
            "    gl_Position = vec4(glVertex, 0.0, 1.0);\n" +
            "    frgTexCoord = GL_TexCoord;\n" +
            "    frgBlurTexCoord[ 0] = GL_TexCoord + vec2(-7.0*TexelSize, 0.0);\n" +
            "    frgBlurTexCoord[ 1] = GL_TexCoord + vec2(-6.0*TexelSize, 0.0);\n" +
            "    frgBlurTexCoord[ 2] = GL_TexCoord + vec2(-5.0*TexelSize, 0.0);\n" +
            "    frgBlurTexCoord[ 3] = GL_TexCoord + vec2(-4.0*TexelSize, 0.0);\n" +
            "    frgBlurTexCoord[ 4] = GL_TexCoord + vec2(-3.0*TexelSize, 0.0);\n" +
            "    frgBlurTexCoord[ 5] = GL_TexCoord + vec2(-2.0*TexelSize, 0.0);\n" +
            "    frgBlurTexCoord[ 6] = GL_TexCoord + vec2(-1.0*TexelSize, 0.0);\n" +
            "    frgBlurTexCoord[ 7] = GL_TexCoord + vec2( 1.0*TexelSize, 0.0);\n" +
            "    frgBlurTexCoord[ 8] = GL_TexCoord + vec2( 2.0*TexelSize, 0.0);\n" +
            "    frgBlurTexCoord[ 9] = GL_TexCoord + vec2( 3.0*TexelSize, 0.0);\n" +
            "    frgBlurTexCoord[10] = GL_TexCoord + vec2( 4.0*TexelSize, 0.0);\n" +
            "    frgBlurTexCoord[11] = GL_TexCoord + vec2( 5.0*TexelSize, 0.0);\n" +
            "    frgBlurTexCoord[12] = GL_TexCoord + vec2( 6.0*TexelSize, 0.0);\n" +
            "    frgBlurTexCoord[13] = GL_TexCoord + vec2( 7.0*TexelSize, 0.0);\n" +
            "}";
    }


    /**
     * retourne le source du Vertex Shader vertical
     */
    protected String getVertiVertexShader()
    {
        return
            "#version 310 es\n" +
            "in vec2 glVertex;\n" +
            "in vec2 glTexCoord;\n" +
            "uniform float TexelSize;\n" +
            "out vec2 frgTexCoord;\n" +
            "out vec2 frgBlurTexCoord[14];\n" +
            "void main()\n" +
            "{\n" +
            "    gl_Position = vec4(glVertex, 0.0, 1.0);\n" +
            "    frgTexCoord = GL_TexCoord;\n" +
            "    frgBlurTexCoord[ 0] = GL_TexCoord + vec2(0.0, -7.0*TexelSize);\n" +
            "    frgBlurTexCoord[ 1] = GL_TexCoord + vec2(0.0, -6.0*TexelSize);\n" +
            "    frgBlurTexCoord[ 2] = GL_TexCoord + vec2(0.0, -5.0*TexelSize);\n" +
            "    frgBlurTexCoord[ 3] = GL_TexCoord + vec2(0.0, -4.0*TexelSize);\n" +
            "    frgBlurTexCoord[ 4] = GL_TexCoord + vec2(0.0, -3.0*TexelSize);\n" +
            "    frgBlurTexCoord[ 5] = GL_TexCoord + vec2(0.0, -2.0*TexelSize);\n" +
            "    frgBlurTexCoord[ 6] = GL_TexCoord + vec2(0.0, -1.0*TexelSize);\n" +
            "    frgBlurTexCoord[ 7] = GL_TexCoord + vec2(0.0,  1.0*TexelSize);\n" +
            "    frgBlurTexCoord[ 8] = GL_TexCoord + vec2(0.0,  2.0*TexelSize);\n" +
            "    frgBlurTexCoord[ 9] = GL_TexCoord + vec2(0.0,  3.0*TexelSize);\n" +
            "    frgBlurTexCoord[10] = GL_TexCoord + vec2(0.0,  4.0*TexelSize);\n" +
            "    frgBlurTexCoord[11] = GL_TexCoord + vec2(0.0,  5.0*TexelSize);\n" +
            "    frgBlurTexCoord[12] = GL_TexCoord + vec2(0.0,  6.0*TexelSize);\n" +
            "    frgBlurTexCoord[13] = GL_TexCoord + vec2(0.0,  7.0*TexelSize);\n" +
            "}";
    }


    /**
     * retourne le source du Fragment Shader
     */
    protected String getFragmentShader()
    {
        return
            "#version 310 es\n" +
            "precision mediump float;\n" +
            "uniform sampler2D ColorMap;\n" +
            "in vec2 frgTexCoord;\n" +
            "in vec2 frgBlurTexCoord[14];\n" +
            "out vec4 glFragColor;\n" +
            "void main()\n" +
            "{\n" +
            "    vec4 sum = vec4(0.0);\n" +
            "    sum += texture(ColorMap, frgBlurTexCoord[ 0] +*0.0044299121055113265;\n" +
            "    sum += texture(ColorMap, frgBlurTexCoord[ 1] +*0.00895781211794;\n" +
            "    sum += texture(ColorMap, frgBlurTexCoord[ 2] +*0.0215963866053;\n" +
            "    sum += texture(ColorMap, frgBlurTexCoord[ 3] +*0.0443683338718;\n" +
            "    sum += texture(ColorMap, frgBlurTexCoord[ 4] +*0.0776744219933;\n" +
            "    sum += texture(ColorMap, frgBlurTexCoord[ 5] +*0.115876621105;\n" +
            "    sum += texture(ColorMap, frgBlurTexCoord[ 6] +*0.147308056121;\n" +
            "    sum += texture(ColorMap, frgTexCoord         +*0.159576912161;\n" +
            "    sum += texture(ColorMap, frgBlurTexCoord[ 7] +*0.147308056121;\n" +
            "    sum += texture(ColorMap, frgBlurTexCoord[ 8] +*0.115876621105;\n" +
            "    sum += texture(ColorMap, frgBlurTexCoord[ 9] +*0.0776744219933;\n" +
            "    sum += texture(ColorMap, frgBlurTexCoord[10] +*0.0443683338718;\n" +
            "    sum += texture(ColorMap, frgBlurTexCoord[11] +*0.0215963866053;\n" +
            "    sum += texture(ColorMap, frgBlurTexCoord[12] +*0.00895781211794;\n" +
            "    sum += texture(ColorMap, frgBlurTexCoord[13] +*0.0044299121055113265;\n" +
            "    \n" +
            "    glFragColor = sum;\n" +
            "}";
    }


    /**
     * compile ou recompile le shader horizontal
     */
    protected void compileHorizShader() throws Exception
    {
        // supprimer l'ancien shader s'il y en avait un
        if (m_HorizShaderId > 0) Utils.deleteShaderProgram(m_HorizShaderId);

        // compiler le shader de floutage horizontal
        String srcVertexShader = getHorizVertexShader();
        String srcFragmentShader = getFragmentShader();
        m_HorizShaderId = Utils.makeShaderProgram(srcVertexShader, srcFragmentShader, "HorizBlur");

        // déterminer où sont les variables attribute
        m_HorizVertexLoc    = glGetAttribLocation(m_HorizShaderId, "glVertex");
        m_HorizTexCoordLoc  = glGetAttribLocation(m_HorizShaderId, "glTexCoord");

        // déterminer où sont les variables uniform
        m_HorizColorMapLoc  = glGetUniformLocation(m_HorizShaderId, "ColorMap");
        m_HorizTexelSizeLoc = glGetUniformLocation(m_HorizShaderId, "TexelSize");
    }


    /**
     * compile ou recompile le shader vertical
     */
    protected void compileVertiShader() throws Exception
    {
        // supprimer l'ancien shader s'il y en avait un
        if (m_VertiShaderId > 0) Utils.deleteShaderProgram(m_VertiShaderId);

        // compiler le shader de floutage vertical
        String srcVertexShader = getVertiVertexShader();
        String srcFragmentShader = getFragmentShader();
        m_VertiShaderId = Utils.makeShaderProgram(srcVertexShader, srcFragmentShader, "VertiBlur");

        // déterminer où sont les variables attribute
        m_VertiVertexLoc    = glGetAttribLocation(m_VertiShaderId, "glVertex");
        m_VertiTexCoordLoc  = glGetAttribLocation(m_VertiShaderId, "glTexCoord");

        // déterminer où sont les variables uniform
        m_VertiColorMapLoc  = glGetUniformLocation(m_VertiShaderId, "ColorMap");
        m_VertiTexelSizeLoc = glGetUniformLocation(m_VertiShaderId, "TexelSize");
    }


    /**
     * applique le traitement sur l'image fournie dans le FBO
     * @param fbo : FBO contenant l'image à flouter
     * @param radius : rayon du flou
     */
    public void process(FrameBufferObject fbo, float radius)
    {
        // Première étape : flouter horizontalement dans un FBO intermédiaire
        m_FBO.enable();

        // désactiver le test du depth buffer
        glDisable(GL_DEPTH_TEST);

        // activer le shader horizontal
        glUseProgram(m_HorizShaderId);

        // activer et lier le buffer contenant les coordonnées
        glEnableVertexAttribArray(m_HorizVertexLoc);
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
        glVertexAttribPointer(m_HorizVertexLoc, 2, GL_FLOAT, false, 0, null);

        // activer et lier le buffer contenant les coordonnées de texture
        glEnableVertexAttribArray(m_HorizTexCoordLoc);
        glBindBuffer(GL_ARRAY_BUFFER, m_TexCoordBufferId);
        glVertexAttribPointer(m_HorizTexCoordLoc, 2, GL_FLOAT, false, 0, null);

        // fournir le color buffer
        setTextureUnit(GL_TEXTURE0, m_HorizColorMapLoc, fbo.getColorBuffer());

        // fournir les paramètres du flou
        glUniform1f(m_HorizTexelSizeLoc, radius/fbo.getWidth());

        // dessiner un quadrilatère avec les quatre vertices
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        // désactiver les textures
        setTextureUnit(GL_TEXTURE0);

        // désactiver le buffer
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDisableVertexAttribArray(m_HorizVertexLoc);
        glDisableVertexAttribArray(m_HorizTexCoordLoc);

        // termine les dessins dans le FBO intermédiaire et remet le précédent FBO
        m_FBO.disable();

        // Deuxième phase : flouter verticalement le précédent FBO

        // activer le shader vertical
        glUseProgram(m_VertiShaderId);

        // activer et lier le buffer contenant les coordonnées
        glEnableVertexAttribArray(m_VertiVertexLoc);
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
        glVertexAttribPointer(m_VertiVertexLoc, 2, GL_FLOAT, false, 0, null);

        // activer et lier le buffer contenant les coordonnées de texture
        glEnableVertexAttribArray(m_VertiTexCoordLoc);
        glBindBuffer(GL_ARRAY_BUFFER, m_TexCoordBufferId);
        glVertexAttribPointer(m_VertiTexCoordLoc, 2, GL_FLOAT, false, 0, null);

        // fournir le color buffer du FBO intermédiaire
        setTextureUnit(GL_TEXTURE0, m_VertiColorMapLoc, m_FBO.getColorBuffer());

        // fournir les paramètres du flou
        glUniform1f(m_VertiTexelSizeLoc, radius/fbo.getHeight());

        // dessiner un quadrilatère avec les quatre vertices
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        // désactiver les textures
        setTextureUnit(GL_TEXTURE0);

        // désactiver le buffer
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDisableVertexAttribArray(m_VertiVertexLoc);
        glDisableVertexAttribArray(m_VertiTexCoordLoc);

        // désactiver le shader
        glUseProgram(0);

        // réactiver le test du depth buffer
        glEnable(GL_DEPTH_TEST);
    }
}
