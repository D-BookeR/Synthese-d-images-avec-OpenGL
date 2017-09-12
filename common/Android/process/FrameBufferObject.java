package com.dbooker.livreopengl.process;

import static android.opengl.GLES20.*;

import java.nio.IntBuffer;

import android.util.Log;

import com.dbooker.livreopengl.material.Material;
import com.dbooker.livreopengl.utils.Utils;


@SuppressWarnings("unused")
public class FrameBufferObject
{
    private static final String TAG = "LivreOpenGL";

    // identifiants du FBO et des buffers
    protected int m_FBO;
    protected IntBuffer m_ColorBufferIds;
    protected IntBuffer m_DrawBufferNames;
    protected int m_DepthBufferId;

    /*** MODE MRT SIMULÉ ***/
    // indique si le FBO est en mode Multiple Render Target, il devrait toujours être true
    private boolean m_IsMRT;
    /*** MODE MRT SIMULÉ ***/

    // dimensions du FBO
    protected int m_Width;
    protected int m_Height;

    // mémorisation du viewport et du FBO précédent
    protected IntBuffer m_Viewport = IntBuffer.allocate(4);
    protected IntBuffer m_PrecFBO = IntBuffer.allocate(1);


    /**
     * constructeur de la classe FrameBufferObject
     * @param width largeur du FBO (nombre de pixels)
     * @param height hauteur du FBO
     * @param color : fournir GL_NONE si aucun, GL_TEXTURE_2D si on veut un buffer de type texture, GL_RENDERBUFFER si c'est un renderbuffer
     * @param depth : fournir GL_NONE si aucun, GL_TEXTURE_2D si on veut un buffer de type texture, GL_RENDERBUFFER si c'est un renderbuffer, NB: il faut impérativement un depth buffer dans un FBO destiné à être rendu
     * @param colorsnb : nombre de color buffer supplémentaires pour faire du dessin différé (MRT), et affecter plusieurs valeurs de gl_FragData[i]
     * @param filtering : filtrage des textures, mettre GL_NEAREST ou GL_LINEAR (valeur par défaut)
     * @throws Exception
     */
    public FrameBufferObject(int width, int height, int color, int depth, int colorsnb, int filtering) throws Exception
    {
        init(width, height, color, depth, colorsnb, filtering);
    }
    public FrameBufferObject(int width, int height, int color, int depth, int colorsnb) throws Exception
    {
        init(width, height, color, depth, colorsnb, GL_LINEAR);
    }
    public FrameBufferObject(int width, int height, int color, int depth) throws Exception
    {
        init(width, height, color, depth, 0, GL_LINEAR);
    }

    private void init(int width, int height, int color, int depth, int colorsnb, int filtering) throws Exception
    {
        // test sur les paramètres pour éviter des bizarreries
        if (colorsnb > 0 && color == GL_NONE) {
            throw new Exception("FrameBufferObject: colorsnb>0 but no main color buffer");
        }

        /*** MODE MRT SIMULÉ ***/
        m_IsMRT = (colorsnb > 0 && Material.GBufferSize == 0);
        /*** MODE MRT SIMULÉ ***/

        // variables d'instance
        m_FBO = 0;
        m_DepthBufferId = 0;
        m_Width = width;
        m_Height = height;
        m_ColorBufferIds = IntBuffer.allocate(1+colorsnb);
        m_DrawBufferNames = IntBuffer.allocate(1+colorsnb);
        final int[] ids = new int[1];

        //float borderColor[] = {1.0f,1.0f,1.0f,0.0f};

        // créer le FBO
        glGenFramebuffers(1, ids, 0);
        m_FBO = ids[0];
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

        // identifiant du buffer courant
        int bufferId = 0;

        // créer le buffer de couleur principal, rempli par glFragColor ou gl_FragData[0]
        switch (color) {

        case GL_TEXTURE_2D:
            // créer une texture 2D pour recevoir les dessins faits via le FBO
            glGenTextures(1, ids, 0);
            bufferId = ids[0];
            glBindTexture(GL_TEXTURE_2D, bufferId);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,  width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, null);

            // configurer la texture
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);

            // attacher la texture au FBO
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferId, 0);
            m_ColorBufferIds.put(bufferId);
            m_DrawBufferNames.put(GL_COLOR_ATTACHMENT0);
            break;

        case GL_RENDERBUFFER:
            // lui ajouter un color buffer de type render buffer
            glGenRenderbuffers(1, ids, 0);
            bufferId = ids[0];
            glBindRenderbuffer(GL_RENDERBUFFER, bufferId);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, width, height);

            // attacher le render buffer au FBO
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, bufferId);
            m_ColorBufferIds.put(bufferId);
            m_DrawBufferNames.put(GL_COLOR_ATTACHMENT0);
            break;

        case GL_NONE:
            break;

        default:
            Log.e(TAG, "FrameBufferObject: color is not among NONE, TEXTURE_2D, TEXTURE_CUBE_MAP, RENDERBUFFER");
        }

        // créer le buffer de profondeur, rempli automatiquement par gl_FragDepth
        switch (depth) {

        case GL_TEXTURE_2D:
            // lui ajouter un depth buffer de type texture
            glGenTextures(1, ids, 0);
            m_DepthBufferId = ids[0];
            glBindTexture(GL_TEXTURE_2D, m_DepthBufferId);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, null);
            //glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, null);

            // configurer la texture
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);

            // couleur du bord (n'existe pas en OpenGL ES)
            //glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

            // attacher le depth buffer au FBO
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthBufferId, 0);
            break;

        case GL_RENDERBUFFER:
            // lui ajouter un depth buffer de type render buffer
            glGenRenderbuffers(1, ids, 0);
            m_DepthBufferId = ids[0];
            glBindRenderbuffer(GL_RENDERBUFFER, m_DepthBufferId);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);

            // attacher le depth buffer au FBO
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthBufferId);
            break;

        case GL_NONE:
            // voir la remarque dans https://www.opengl.org/wiki/Framebuffer_Object
            // Even if you don't plan on reading from this depth_attachment, an off screen buffer that will be rendered to should have a depth attachment.
            break;

        default:
            Log.e(TAG, "FrameBufferObject: depth is not among NONE, TEXTURE_2D, TEXTURE_CUBE_MAP, RENDERBUFFER");
        }

        // créer des buffers supplémentaires si c'est demandé
        for (int i=0; i<colorsnb; i++) {

            // créer une texture 2D pour recevoir les dessins (voir gl_FragData dans les shaders)
            glGenTextures(1, ids, 0);
            bufferId = ids[0];
            glBindTexture(GL_TEXTURE_2D, bufferId);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,  width, height, 0, GL_RGBA, GL_FLOAT, null);

            // configurer la texture
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);

            /*** MODE MRT SIMULÉ ***/
            // attacher la texture au FBO si OpenGL ES en est capable, sinon on la garde sous le coude
            if (m_IsMRT) {
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+1 + i, GL_TEXTURE_2D, bufferId, 0);
            }
            /*** MODE MRT SIMULÉ ***/

            m_ColorBufferIds.put(bufferId);
            m_DrawBufferNames.put(GL_COLOR_ATTACHMENT0+1 + i);
        }

        // indiquer quels sont les buffers utilisés pour le dessin (gl_FragData valides)
        // cette fonction n'existe que dans OpenGL ES 3.0, mettre GLES30 à la place de GLES20
        //if (m_IsMRT) {
        //    glDrawBuffers(m_DrawBufferNames.capacity(), m_DrawBufferNames);
        //}

        // vérifier l'état des lieux
        checkStatus();

        // désactiver le FBO pour l'instant
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }


    /**
     * vérifie l'état du FBO
     * @throws Exception
     */
    protected void checkStatus() throws Exception
    {
        int code = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        String status;
        switch (code) {
            case GL_FRAMEBUFFER_COMPLETE:
                status="FRAMEBUFFER_COMPLETE";
                break;
            //case GL_FRAMEBUFFER_UNDEFINED:
            //    status="FRAMEBUFFER_UNDEFINED";
            //    break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                status="FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                status="FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
                break;
            //case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            //    status="FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
            //    break;
            //case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            //    status="FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
            //    break;
            case GL_FRAMEBUFFER_UNSUPPORTED:
                status="FRAMEBUFFER_UNSUPPORTED";
                break;
            //case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            //    status="FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
            //    break;
            //case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
            //    status="FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
            //    break;
            default:
                status="FRAMEBUFFER_??";
                break;
        }
        if (code != GL_FRAMEBUFFER_COMPLETE) {
            // signaler une erreur
            Log.e(TAG, "FrameBufferObject : create FBO "+m_Width+"x"+m_Height+" : "+status);
            throw new Exception("error creating FBO");
        }
    }


    /**
     * supprime ce FBO
     */
    public void destroy()
    {
        // déterminer quels sont les types des attachements
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
        IntBuffer params = IntBuffer.allocate(1);
        glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, params);
        int color = params.get(0);
        glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, params);
        int depth = params.get(0);

        // supprimer le FBO
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        int[] ids = { m_FBO };
        glDeleteFramebuffers(1, ids, 0);

        // libérer le color buffer s'il y en a un
        params.put(0, m_ColorBufferIds.get(0));
        switch (color) {
        case GL_TEXTURE:
            glDeleteTextures(1, params);
            break;
        case GL_RENDERBUFFER:
            glDeleteRenderbuffers(1, params);
            break;
        }

        // libérer le depth buffer s'il y en a un
        params.put(0, m_DepthBufferId);
        switch (depth) {
        case GL_TEXTURE:
            glDeleteTextures(1, params);
            break;
        case GL_RENDERBUFFER:
            glDeleteRenderbuffers(1, params);
            break;
        }

        // libérer les autres buffers s'il y en a
        for (int i=1; i<m_ColorBufferIds.capacity(); i++) {
            params.put(0, m_ColorBufferIds.get(i));
            glDeleteTextures(1, params);
        }
    }


    /**
     * compte les buffers supplémentaires
     * @return le nombre de buffers de dessin supplémentaires
     */
    public boolean isMRT()
    {
        return m_IsMRT;
    }


    /**
     * retourne le nombre de color buffers de ce FBO
     * @return nombre de buffers
     */
    public int getBufferNumber()
    {
        return m_ColorBufferIds.capacity();
    }


    /*** MODE MRT SIMULÉ ***/
    /**
     * redirige tous les tracés suivants vers l'un des color buffers du FBO
     * @param num numéro du color buffer, 0..getBufferNumber()-1
     */
    public void enable(int num)
    {
        // activer le FBO
        enable();

        // activer seulement le buffer num pour glFragColor ou gl_FragData[0]
        if (!m_IsMRT) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorBufferIds.get(num), 0);
        }
    }
    /*** MODE MRT SIMULÉ ***/


    /**
     * redirige tous les tracés suivants vers le FBO
     */
    public void enable()
    {
        // sauver le viewport
        glGetIntegerv(GL_VIEWPORT, m_Viewport);

        // redéfinir le viewport pour correspondre au FBO
        glViewport(0, 0, m_Width, m_Height);

        // enregistrer le précédent FBO actif
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, m_PrecFBO);

        // activer le FBO
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    }


    /**
     * redirige tous les tracés suivants vers l'écran à nouveau
     */
    public void disable()
    {
        // désactiver le FBO, remettre le précédent en place
        glBindFramebuffer(GL_FRAMEBUFFER, m_PrecFBO.get(0));

        // remettre le viewport à ce qu'il était avant
        glViewport(m_Viewport.get(0), m_Viewport.get(1), m_Viewport.get(2), m_Viewport.get(3));
    }


    /**
     * retourne l'identifiant du Color Buffer
     * @param num = 0 pour l'attachement COLOR_ATTACHMENT0, 1 pour COLOR_ATTACHMENT1...
     * @return color buffer de ce FBO
     */
    public int getColorBuffer(int num)
    {
        return m_ColorBufferIds.get(num);
    }
    public int getColorBuffer()
    {
        return m_ColorBufferIds.get(0);
    }


    /**
     * retourne l'identifiant du Depth Buffer
     * @return depth buffer de ce FBO
     */
    public int getDepthBuffer()
    {
        return m_DepthBufferId;
    }


    /**
     * retourne la largeur de ce FBO
     * @return largeur
     */
    public int getWidth()
    {
        return m_Width;
    }


    /**
     * retourne la hauteur de ce FBO
     * @return hauteur
     */
    public int getHeight()
    {
        return m_Height;
    }


    /**
     * cette fonction associe l'une des textures de ce FBO à une unité pour un shader
     * NB : le shader concerné doit être actif
     * @param unit : unité de texture concernée, par exemple GL_TEXTURE0
     * @param locSampler : emplacement de la variable uniform sampler* de cette texture dans le shader ou -1 pour désactiver la texture
     * @param bufferId : l'un des buffers retourné par getColorBuffer ou getDepthBuffer ou 0 pour désactiver
     */
    public void setTextureUnit(int unit, int locSampler, int bufferId)
    {
        /*****DEBUG*****/
        if (unit < GL_TEXTURE0 || unit > GL_TEXTURE7) {
            Log.e(TAG, "FrameBufferObject.setTextureUnit: first parameter, unit is not GL_TEXTURE0 ... GL_TEXTURE7");
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
        setTextureUnit(unit, -1, 0);
    }


    /**
     * Cette méthode échange les color buffers associés à this et l'autre FBO.
     * Elle attache le color buffer num1 de this à l'emplacement num2 de otherFBO et
     * elle attache le color buffer num2 de otherFBO à l'emplacement num1 de this.
     * Si on rappelle cette méthode avec les mêmes paramètres, tout est remis comme avant.
     * Cette méthode permet de dessiner temporairement avec un g-buffer composé de buffers
     * appartenant à d'autres FBO.
     * @param num1 numéro du color buffer de this
     * @param otherFBO  autre FBO concerné
     * @param num2 numéro du color buffer de otherFBO
     */
    public void swapColorBuffers(int num1, FrameBufferObject otherFBO, int num2)
    {
        // identifiants des textures actuellement associées aux deux FBO
        int bufferId1 = m_ColorBufferIds.get(num1);
        int bufferId2 = otherFBO.m_ColorBufferIds.get(num2);

        // enregistrer le précédent FBO actif
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, m_PrecFBO);

        // activer le FBO this
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

        // changer la texture attachée
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+num1, GL_TEXTURE_2D, bufferId2, 0);
        m_ColorBufferIds.put(num1, bufferId2);

        // activer l'autre FBO
        glBindFramebuffer(GL_FRAMEBUFFER, otherFBO.m_FBO);

        // changer la texture attachée
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+num2, GL_TEXTURE_2D, bufferId1, 0);
        otherFBO.m_ColorBufferIds.put(num2, bufferId1);

        // remettre le précédent FBO en place
        glBindFramebuffer(GL_FRAMEBUFFER, m_PrecFBO.get(0));
    }



    /*** Tout ce qui suit concerne l'affichage du FBO sur l'écran pour la mise au point ***/



    // shader pour dessiner l'un des color buffers
    protected static int m_ShaderRGBId;
    protected static int m_VertexRGBLoc;
    protected static int m_TexCoordRGBLoc;
    protected static int m_BufferRGBLoc;

    // shader pour dessiner le canal alpha d'un des color buffers
    protected static int m_ShaderAlphaId;
    protected static int m_VertexAlphaLoc;
    protected static int m_TexCoordAlphaLoc;
    protected static int m_BufferAlphaLoc;

    // shader pour dessiner le depth buffer
    protected static int m_ShaderDepthId;
    protected static int m_VertexDepthLoc;
    protected static int m_TexCoordDepthLoc;
    protected static int m_BufferDepthLoc;

    // VBO pour dessiner un rectangle sur tout l'écran
    protected static int m_VertexBufferId;
    protected static int m_TexCoordBufferId;


    /**
     * construit les structures de données permettant de dessiner les FBO sur l'écran
     * @see #onDraw
     */
    static
    {
        /// générer un shader et un VBO pour dessiner l'un des buffers de ce FBO
        final String VertexShader =
            "#version 310 es\n" +
            "in vec2 glVertex;\n" +
            "in vec2 glTexCoord;\n" +
            "out vec2 frgTexCoord;\n" +
            "void main()\n" +
            "{\n" +
            "    gl_Position = vec4(glVertex, 0.0, 1.0);\n" +
            "    frgTexCoord = glTexCoord;\n" +
            "}";

        final String FragmentShaderRGB =
            "#version 310 es\n" +
            "precision mediump float;\n" +
            "uniform sampler2D Buffer;\n" +
            "in vec2 frgTexCoord;\n" +
            "out vec4 glFragColor;\n" +
            "void main()\n" +
            "{\n" +
            "    glFragColor = texture(Buffer, frgTexCoord);\n" +
            "}";

        // compiler le shader de dessin
        try {
            m_ShaderRGBId = Utils.makeShaderProgram(VertexShader, FragmentShaderRGB, "FrameBufferObjectRGB");
        } catch (Exception e) {
            e.printStackTrace();
        }

        // déterminer où sont les variables attribute et uniform
        m_VertexRGBLoc   = glGetAttribLocation(m_ShaderRGBId, "glVertex");
        m_TexCoordRGBLoc = glGetAttribLocation(m_ShaderRGBId, "glTexCoord");
        m_BufferRGBLoc   = glGetUniformLocation(m_ShaderRGBId, "Buffer");

        /// générer un shader et un VBO pour dessiner le canal alpha de l'un des buffers de ce FBO
        String FragmentShaderAlpha =
            "#version 310 es\n" +
            "precision mediump float;\n" +
            "uniform sampler2D Buffer;\n" +
            "in vec2 frgTexCoord;\n" +
            "out vec4 glFragColor;\n" +
            "void main()\n" +
            "{\n" +
            "    float a = texture(Buffer, frgTexCoord).a;\n" +
            "    glFragColor = vec4(a,a,a,1.0);\n" +
            "}";

        // compiler le shader de dessin
        try {
            m_ShaderAlphaId = Utils.makeShaderProgram(VertexShader, FragmentShaderAlpha, "FrameBufferObjectAlpha");
        } catch (Exception e) {
            e.printStackTrace();
        }

        // déterminer où sont les variables attribute et uniform
        m_VertexAlphaLoc   = glGetAttribLocation(m_ShaderAlphaId, "glVertex");
        m_TexCoordAlphaLoc = glGetAttribLocation(m_ShaderAlphaId, "glTexCoord");
        m_BufferAlphaLoc   = glGetUniformLocation(m_ShaderAlphaId, "Buffer");

        /// générer un shader et un VBO pour dessiner le depth buffer de ce FBO
        String FragmentShaderDepth =
            "#version 310 es\n" +
            "precision mediump float;\n" +
            "uniform sampler2D Buffer;\n" +
            "in vec2 frgTexCoord;\n" +
            "out vec4 glFragColor;\n" +
            "void main()\n" +
            "{\n" +
            "    float d = texture(Buffer, frgTexCoord).r;\n" +
            "    glFragColor = vec4(d,d,d,1.0);\n" +
            "}";

        // compiler le shader de dessin
        try {
            m_ShaderDepthId = Utils.makeShaderProgram(VertexShader, FragmentShaderDepth, "FrameBufferObjectDepth");
        } catch (Exception e) {
            e.printStackTrace();
        }

        // déterminer où sont les variables attribute et uniform
        m_VertexDepthLoc   = glGetAttribLocation(m_ShaderDepthId, "glVertex");
        m_TexCoordDepthLoc = glGetAttribLocation(m_ShaderDepthId, "glTexCoord");
        m_BufferDepthLoc   = glGetUniformLocation(m_ShaderDepthId, "Buffer");

        // créer et remplir le buffer des coordonnées
        float[] vertices =  { -1,-1,  +1,-1,  +1,+1,  -1,+1 };
        m_VertexBufferId = Utils.makeFloatVBO(vertices, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
        float[] texcoords = {  0, 0,   1, 0,   1, 1,   0, 1 };
        m_TexCoordBufferId = Utils.makeFloatVBO(texcoords, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
    }


    /**
     * supprime les shaders et VBO de dessin
     */
    public static void staticdestroy()
    {
        Utils.deleteShaderProgram(m_ShaderRGBId);
        Utils.deleteShaderProgram(m_ShaderDepthId);
        Utils.deleteShaderProgram(m_ShaderAlphaId);
        Utils.deleteVBO(m_VertexBufferId);
        Utils.deleteVBO(m_TexCoordBufferId);
    }


    /**
     * dessine l'un des buffers dans le viewport (pour mise au point)
     * @param attachment : mettre GL_COLOR_ATTACHMENT0+i pour le ième color buffer, GL_DEPTH_ATTACHMENT pour le depth buffer
     * @param alpha : true si on dessine le canal alpha du color buffer
     */
    public void onDraw(int attachment, boolean alpha)
    {
        switch (attachment) {
            case GL_DEPTH_ATTACHMENT:
                onDrawDepth();
                break;
            default:
                if (alpha) {
                    onDrawAlpha(attachment);
                } else {
                    onDrawRGB(attachment);
                }
                break;
        }
    }
    public void onDraw(int attachment)
    {
        onDraw(attachment, false);
    }


    /**
     * dessine le color buffer indiqué dans le viewport (pour mise au point)
     * @param attachment : mettre GL_COLOR_ATTACHMENT0+i pour le ième color buffer
     */
    protected void onDrawRGB(int attachment)
    {
        // désactiver le test du depth buffer
        glDisable(GL_DEPTH_TEST);

        // activer le shader
        glUseProgram(m_ShaderRGBId);

        // activer et lier le buffer contenant les coordonnées
        glEnableVertexAttribArray(m_VertexRGBLoc);
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
        glVertexAttribPointer(m_VertexRGBLoc, 2, GL_FLOAT, false, 0, 0);

        // activer et lier le buffer contenant les coordonnées de texture
        glEnableVertexAttribArray(m_TexCoordRGBLoc);
        glBindBuffer(GL_ARRAY_BUFFER, m_TexCoordBufferId);
        glVertexAttribPointer(m_TexCoordRGBLoc, 2, GL_FLOAT, false, 0, 0);

        // activer la texture à dessiner (ne marche pas si ce sont des RenderBuffers)
        setTextureUnit(GL_TEXTURE0, m_BufferRGBLoc, m_ColorBufferIds.get(attachment - GL_COLOR_ATTACHMENT0));

        // dessiner un quadrilatère avec les quatre vertices
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        // désactiver la texture
        setTextureUnit(GL_TEXTURE0);

        // désactiver le buffer
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDisableVertexAttribArray(m_VertexRGBLoc);
        glDisableVertexAttribArray(m_TexCoordRGBLoc);

        // désactiver le shader
        glUseProgram(0);

        // réactiver le test du depth buffer
        glEnable(GL_DEPTH_TEST);
    }


    /**
     * dessine le canal alpha de l'un des buffers dans le viewport (pour mise au point)
     * @param attachment : mettre GL_COLOR_ATTACHMENT0+i pour le ième color buffer
     */
    protected void onDrawAlpha(int attachment)
    {
        // désactiver le test du depth buffer
        glDisable(GL_DEPTH_TEST);

        // activer le shader
        glUseProgram(m_ShaderAlphaId);

        // activer et lier le buffer contenant les coordonnées
        glEnableVertexAttribArray(m_VertexAlphaLoc);
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
        glVertexAttribPointer(m_VertexAlphaLoc, 2, GL_FLOAT, false, 0, 0);

        // activer et lier le buffer contenant les coordonnées de texture
        glEnableVertexAttribArray(m_TexCoordAlphaLoc);
        glBindBuffer(GL_ARRAY_BUFFER, m_TexCoordBufferId);
        glVertexAttribPointer(m_TexCoordAlphaLoc, 2, GL_FLOAT, false, 0, 0);

        // activer la texture à dessiner (ne marche pas si ce sont des RenderBuffers)
        setTextureUnit(GL_TEXTURE0, m_BufferAlphaLoc, m_ColorBufferIds.get(attachment - GL_COLOR_ATTACHMENT0));

        // dessiner un quadrilatère avec les quatre vertices
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        // désactiver la texture
        setTextureUnit(GL_TEXTURE0);

        // désactiver le buffer
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDisableVertexAttribArray(m_VertexAlphaLoc);
        glDisableVertexAttribArray(m_TexCoordAlphaLoc);

        // désactiver le shader
        glUseProgram(0);

        // réactiver le test du depth buffer
        glEnable(GL_DEPTH_TEST);
    }


    /**
     * dessine le depth buffer dans le viewport (pour mise au point)
     */
    protected void onDrawDepth()
    {
        // désactiver le test du depth buffer
        glDisable(GL_DEPTH_TEST);

        // activer le shader
        glUseProgram(m_ShaderDepthId);

        // activer et lier le buffer contenant les coordonnées
        glEnableVertexAttribArray(m_VertexDepthLoc);
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
        glVertexAttribPointer(m_VertexDepthLoc, 2, GL_FLOAT, false, 0, 0);

        // activer et lier le buffer contenant les coordonnées de texture
        glEnableVertexAttribArray(m_TexCoordDepthLoc);
        glBindBuffer(GL_ARRAY_BUFFER, m_TexCoordBufferId);
        glVertexAttribPointer(m_TexCoordDepthLoc, 2, GL_FLOAT, false, 0, 0);

        // activer la texture à dessiner (ne marche pas si c'est un RenderBuffer)
        setTextureUnit(GL_TEXTURE0, m_BufferDepthLoc, m_DepthBufferId);

        // dessiner un quadrilatère avec les quatre vertices
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        // désactiver la texture
        setTextureUnit(GL_TEXTURE0);

        // désactiver le buffer
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDisableVertexAttribArray(m_VertexDepthLoc);
        glDisableVertexAttribArray(m_TexCoordDepthLoc);

        // désactiver le shader
        glUseProgram(0);

        // réactiver le test du depth buffer
        glEnable(GL_DEPTH_TEST);
    }
}

