#ifndef PROCESS_FRAMEBUFFEROBJECT_H
#define PROCESS_FRAMEBUFFEROBJECT_H

#include <GL/glew.h>
#include <GL/gl.h>

#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <stdexcept>

#include <gl-matrix.h>
#include <utils.h>


class FrameBufferObject
{

public:

    /**
     * constructeur de la classe FrameBufferObject
     * @param largeur du FBO (nombre de pixels)
     * @param hauteur du FBO
     * @param color : fournir GL_NONE si aucun, GL_TEXTURE_2D si on veut un buffer de type texture, GL_RENDERBUFFER si c'est un renderbuffer
     * @param depth : fournir GL_NONE si aucun, GL_TEXTURE_2D si on veut un buffer de type texture, GL_RENDERBUFFER si c'est un renderbuffer
     * @param numcolors : nombre de color buffer supplémentaires pour faire du dessin différé (gl_FragData[i])
     * @param filtrage : filtrage des textures, mettre GL_NEAREST (valeur par défaut) ou GL_LINEAR
     */
    FrameBufferObject(int width, int height, GLenum color=GL_TEXTURE_2D, GLenum depth=GL_RENDERBUFFER, int numcolors=0, GLenum filtering=GL_LINEAR);

    // destructeur
    virtual ~FrameBufferObject();

    /**
     * redirige tous les tracés suivants vers le FBO
     */
    virtual void enable();

    /**
     * redirige tous les tracés suivants vers l'écran à nouveau
     */
    virtual void disable();

    /**
     * retourne l'identifiant du FBO
     * @return identifiant du FBO
     */
    GLuint getId()
    {
        return m_FBO;
    }

    /**
     * retourne l'identifiant du Color Buffer
     * @param num = 0 pour l'attachement COLOR_ATTACHMENT0, 1 pour COLOR_ATTACHMENT1...
     * @return color buffer de ce FBO
     */
    GLuint getColorBuffer(int num=0);

    /**
     * retourne l'identifiant du Depth Buffer
     * @return depth buffer de ce FBO
     */
    GLuint getDepthBuffer();

    /**
     * retourne la largeur de ce FBO
     * @return largeur
     */
    GLuint getWidth();

    /**
     * retourne la hauteur de ce FBO
     * @return hauteur
     */
    GLuint getHeight();

    /**
     * cette fonction associe l'une des textures de ce FBO à une unité pour un shader
     * @note le shader concerné doit être actif
     * @param unit : unité de texture concernée, par exemple GL_TEXTURE0
     * @param locSampler : emplacement de la variable uniform sampler* de cette texture dans le shader ou 0 pour désactiver la texture
     * @param bufferId : m_ColorBuffer ou m_DepthBuffer ou 0
     */
    virtual void setTextureUnit(GLint unit, GLint locSampler=-1, GLuint bufferId=0);

    /**
     * Cette méthode échange les color buffers associés à this et l'autre FBO.
     * Elle attache le color buffer num1 de this à l'emplacement num2 de autreFBO et
     * elle attache le color buffer num2 de autreFBO à l'emplacement num1 de this.
     * Si on rappelle cette méthode avec les mêmes paramètres, tout est remis comme avant.
     * Cette méthode permet de dessiner temporairement avec un g-buffer composé de buffers
     * appartenant à d'autres FBO.
     * @param num1 numéro du color buffer de this
     * @param autreFBO  autre FBO concerné
     * @param num2 numéro du color buffer de autreFBO
     */
    void swapColorBuffers(int num1, FrameBufferObject* autreFBO, int num2);

    /**
     * dessine l'un des buffers dans le viewport (pour mise au point)
     * @param attachment : mettre GL_COLOR_ATTACHMENT0 pour le color buffer, GL_DEPTH_ATTACHMENT pour le depth buffer
     * @param alpha : true si on dessine le canal alpha du color buffer
     */
    virtual void onDraw(GLenum attachment, bool alpha=false);

    /** initialise les VBO et shaders pour dessiner les FBO */
    static void staticinit();

    /** supprime les VBO et shaders pour dessiner les FBO */
    static void staticdestroy();

protected:

    /** constructeur par défaut */
    FrameBufferObject();

    /** initialise les variables membres */
    virtual void init(int width, int height);

    /**
     * vérifie l'état du FBO
     */
    void checkStatus();

    /** génére les VBO et shaders pour dessiner ce VBO sur l'écran */
    void initShadersVBO();

    /** supprime les shaders et VBO de dessin */
    void destroyShadersVBO();

    /**
     * dessine le canal alpha de l'un des buffers dans le viewport (pour mise au point)
     * @param attachment : mettre GL_COLOR_ATTACHMENT0 pour le color buffer, GL_DEPTH_ATTACHMENT pour le depth buffer
     */
    virtual void onDrawRGB(GLenum attachment);

    /**
     * dessine le canal alpha de l'un des buffers dans le viewport (pour mise au point)
     * @param attachment : mettre GL_COLOR_ATTACHMENT0 pour le color buffer, GL_DEPTH_ATTACHMENT pour le depth buffer
     */
    virtual void onDrawAlpha(GLenum attachment);

    /**
     * dessine le depth buffer dans le viewport (pour mise au point)
     */
    virtual void onDrawDepth();


protected:

    // identifiants du FBO et des buffers
    GLuint m_FBO;
    std::vector<GLuint> m_ColorBufferIds;
    std::vector<GLuint> m_DrawBufferNames;
    GLuint m_DepthBufferId;

    // dimensions du FBO
    GLuint m_Width;
    GLuint m_Height;

    // mémorisation du viewport et du FBO précédent
    GLint m_Viewport[4];
    GLint m_PrecFBO;

    // shader pour dessiner l'un des color buffers
    static GLint m_ShaderRGBId;
    static GLint m_VertexRGBLoc;
    static GLint m_TexCoordRGBLoc;
    static GLint m_BufferRGBLoc;

    // shader pour dessiner le canal alpha d'un des color buffers
    static GLint m_ShaderAlphaId;
    static GLint m_VertexAlphaLoc;
    static GLint m_TexCoordAlphaLoc;
    static GLint m_BufferAlphaLoc;

    // shader pour dessiner le depth buffer
    static GLint m_ShaderDepthId;
    static GLint m_VertexDepthLoc;
    static GLint m_TexCoordDepthLoc;
    static GLint m_BufferDepthLoc;

    // VBO pour dessiner un rectangle sur tout l'écran
    static GLint m_VertexBufferId;
    static GLint m_TexCoordBufferId;

};


#endif
