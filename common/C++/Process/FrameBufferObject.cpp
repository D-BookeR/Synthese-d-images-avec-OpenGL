//// ce script fournit des fonctions utilitaires pour les programmes
//// du livre Synthèse d'images à l'aide d'OpenGL

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <FrameBufferObject.h>

FrameBufferObject::FrameBufferObject()
{
    init(0,0);
}


void FrameBufferObject::init(int width, int height)
{
    // variables d'instance
    m_FBO = 0;
    m_DepthBufferId = 0;
    m_Width = width;
    m_Height = height;
    m_PrecFBO = 0;
}


/**
 * constructeur de la classe FrameBufferObject
 * @param width largeur du FBO (nombre de pixels)
 * @param height hauteur du FBO
 * @param color : fournir GL_NONE si aucun, GL_TEXTURE_2D si on veut un buffer de type texture, GL_RENDERBUFFER si c'est un renderbuffer.
 * @param depth : fournir GL_NONE si aucun, GL_TEXTURE_2D si on veut un buffer de type texture, GL_RENDERBUFFER si c'est un renderbuffer, NB: il faut impérativement un depth buffer dans un FBO destiné à être rendu
 * @param numcolors : nombre de color buffer supplémentaires pour faire du dessin différé (MRT), et affecter plusieurs valeurs de glFragData[i]
 * @param filtering : filtrage des textures, mettre GL_NEAREST ou GL_LINEAR (valeur par défaut)
 */
FrameBufferObject::FrameBufferObject(int width, int height, GLenum color, GLenum depth, int numcolors, GLenum filtering)
{
    // test sur les paramètres pour éviter des bizarreries
    if (numcolors > 0 && color == GL_NONE) {
        throw std::invalid_argument("FrameBufferObject: numcolors>0 but no main color buffer");
    }

    // variables d'instance
    init(width, height);
    std::vector<GLenum> drawBuffers;

    const GLfloat borderColor[] = {1.0,1.0,1.0,0.0};

    // créer le FBO
    glGenFramebuffers(1, &m_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

    // identifiant du buffer courant
    GLuint bufferId = 0;

    // créer le buffer de couleur principal, rempli par glFragColor ou glFragData[0]
    switch (color) {

    case GL_TEXTURE_2D:
        // créer une texture 2D pour recevoir les dessins faits via le FBO
        glGenTextures(1, &bufferId);
        glBindTexture(GL_TEXTURE_2D, bufferId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F,  width, height, 0, GL_RGBA, GL_FLOAT, 0);

        // configurer la texture
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);

        // attacher la texture au FBO
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferId, 0);
        m_ColorBufferIds.push_back(bufferId);
        m_DrawBufferNames.push_back(GL_COLOR_ATTACHMENT0);
        break;

    case GL_RENDERBUFFER:
        // lui ajouter un color buffer de type render buffer
        glGenRenderbuffers(1, &bufferId);
        glBindRenderbuffer(GL_RENDERBUFFER, bufferId);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, width, height);

        // attacher le render buffer au FBO
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, bufferId);
        m_ColorBufferIds.push_back(bufferId);
        m_DrawBufferNames.push_back(GL_COLOR_ATTACHMENT0);
        break;

    case GL_NONE:
        break;

    default:
        std::cerr << "FrameBufferObject: color is not among GL_NONE, GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP, GL_RENDERBUFFER" << std::endl;
    }

    // créer le buffer de profondeur, rempli automatiquement par gl_FragDepth
    switch (depth) {

    case GL_TEXTURE_2D:
        // lui ajouter un depth buffer de type texture
        glGenTextures(1, &m_DepthBufferId);
        glBindTexture(GL_TEXTURE_2D, m_DepthBufferId);
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

        // configurer la texture
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);

        // couleur du bord
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        // attacher le depth buffer au FBO
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthBufferId, 0);
        break;

    case GL_RENDERBUFFER:
        // lui ajouter un depth buffer de type render buffer
        glGenRenderbuffers(1, &m_DepthBufferId);
        glBindRenderbuffer(GL_RENDERBUFFER, m_DepthBufferId);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

        // attacher le depth buffer au FBO
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthBufferId);
        break;

    case GL_NONE:
        // voir la remarque dans https://www.opengl.org/wiki/Framebuffer_Object
        // Even if you don't plan on reading from this depth_attachment, an off screen buffer that will be rendered to should have a depth attachment.
        break;

    default:
        std::cerr << "FrameBufferObject: depth is not among GL_NONE, GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP, GL_RENDERBUFFER" << std::endl;
    }

    // créer des buffers supplémentaires si c'est demandé
    for (int i=0; i<numcolors; i++) {

        // créer une texture 2D pour recevoir les dessins (voir glFragData dans les shaders)
        glGenTextures(1, &bufferId);
        glBindTexture(GL_TEXTURE_2D, bufferId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);

        // configurer la texture
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);

        // attacher la texture au FBO
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1+i, GL_TEXTURE_2D, bufferId, 0);
        m_ColorBufferIds.push_back(bufferId);
        m_DrawBufferNames.push_back(GL_COLOR_ATTACHMENT1+i);
    }

    // indiquer quels sont les buffers utilisés pour le dessin (glFragData valides)
    glDrawBuffers(m_DrawBufferNames.size(), &m_DrawBufferNames[0]);

    // vérifier l'état des lieux
    checkStatus();

    // désactiver le FBO pour l'instant
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


/**
 * vérifie l'état du FBO
 */
void FrameBufferObject::checkStatus()
{
    GLuint code = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    const char* status;
    switch (code) {
        case GL_FRAMEBUFFER_COMPLETE:
            status="FRAMEBUFFER_COMPLETE";
            break;
        case GL_FRAMEBUFFER_UNDEFINED:
            status="FRAMEBUFFER_UNDEFINED";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            status="FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            status="FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            status="FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            status="FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            status="FRAMEBUFFER_UNSUPPORTED";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            status="FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
            status="FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
            break;
        default:
            status="FRAMEBUFFER_??";
            break;
    }
    if (code != GL_FRAMEBUFFER_COMPLETE) {
        // signaler une erreur
        std::cout << "FrameBufferObject : creating FBO " << m_Width<<"x"<<m_Height << " : " << status << std::endl;
        throw std::invalid_argument("creating FBO");
    }
}


/**
 * supprime ce FBO
 */
FrameBufferObject::~FrameBufferObject()
{
    // déterminer quels sont les types des attachements
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    GLint color = GL_NONE;
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &color);
    GLint depth = GL_NONE;
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &depth);

    // supprimer le FBO
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &m_FBO);

    // libérer le color buffer s'il y en a un
    switch (color) {
    case GL_TEXTURE:
        glDeleteTextures(1, &m_ColorBufferIds[0]);
        break;
    case GL_RENDERBUFFER:
        glDeleteRenderbuffers(1, &m_ColorBufferIds[0]);
        break;
    }

    // libérer le depth buffer s'il y en a un
    switch (depth) {
    case GL_TEXTURE:
        glDeleteTextures(1, &m_DepthBufferId);
        break;
    case GL_RENDERBUFFER:
        glDeleteRenderbuffers(1, &m_DepthBufferId);
        break;
    }

    // libérer les autres buffers s'il y en a
    for (int i=1; i<m_ColorBufferIds.size(); i++) {
        glDeleteTextures(1, &m_ColorBufferIds[i]);
    }
}


/**
 * redirige tous les tracés suivants vers le FBO
 */
void FrameBufferObject::enable()
{
    // sauver le viewport
    glGetIntegerv(GL_VIEWPORT, m_Viewport);

    // redéfinir le viewport pour correspondre au FBO
    glViewport(0, 0, m_Width, m_Height);

    // enregistrer le précédent FBO actif
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_PrecFBO);

    // activer le FBO
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
}


/**
 * redirige tous les tracés suivants vers l'écran à nouveau
 */
void FrameBufferObject::disable()
{
    // désactiver le FBO, remettre le précédent en place
    glBindFramebuffer(GL_FRAMEBUFFER, m_PrecFBO);

    // remettre le viewport à ce qu'il était avant
    glViewport(m_Viewport[0], m_Viewport[1], m_Viewport[2], m_Viewport[3]);
}


/**
 * retourne l'identifiant du Color Buffer
 * @param num = 0 pour l'attachement COLOR_ATTACHMENT0, 1 pour COLOR_ATTACHMENT1...
 * @return color buffer de ce FBO
 */
GLuint FrameBufferObject::getColorBuffer(int num)
{
    return m_ColorBufferIds[num];
}


/**
 * retourne l'identifiant du Depth Buffer
 * @return depth buffer de ce FBO
 */
GLuint FrameBufferObject::getDepthBuffer()
{
    return m_DepthBufferId;
}


/**
 * retourne la largeur de ce FBO
 * @return largeur
 */
GLuint FrameBufferObject::getWidth()
{
    return m_Width;
}


/**
 * retourne la hauteur de ce FBO
 * @return hauteur
 */
GLuint FrameBufferObject::getHeight()
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
void FrameBufferObject::setTextureUnit(GLint unit, GLint locSampler, GLuint bufferId)
{
    /*****DEBUG*****/
    if (unit < GL_TEXTURE0 || unit > GL_TEXTURE7) {
        throw std::invalid_argument("FrameBufferObject::setTextureUnit: first parameter, unit is not GL_TEXTURE0 ... GL_TEXTURE7");
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
void FrameBufferObject::swapColorBuffers(int num1, FrameBufferObject* otherFBO, int num2)
{
    // identifiants des textures actuellement associées aux deux FBO
    GLuint bufferId1 = m_ColorBufferIds[num1];
    GLuint bufferId2 = otherFBO->m_ColorBufferIds[num2];

    // enregistrer le précédent FBO actif
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_PrecFBO);

    // activer le FBO this
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

    // changer la texture attachée
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+num1, GL_TEXTURE_2D, bufferId2, 0);
    m_ColorBufferIds[num1] = bufferId2;

    // activer l'autre FBO
    glBindFramebuffer(GL_FRAMEBUFFER, otherFBO->m_FBO);

    // changer la texture attachée
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+num2, GL_TEXTURE_2D, bufferId1, 0);
    otherFBO->m_ColorBufferIds[num2] = bufferId1;

    // remettre le précédent FBO en place
    glBindFramebuffer(GL_FRAMEBUFFER, m_PrecFBO);
}



/*** Tout ce qui suit concerne l'affichage du FBO sur l'écran pour la mise au point ***/



// shader pour dessiner l'un des color buffers
GLint FrameBufferObject::m_ShaderRGBId = -1;
GLint FrameBufferObject::m_VertexRGBLoc = -1;
GLint FrameBufferObject::m_TexCoordRGBLoc = -1;
GLint FrameBufferObject::m_BufferRGBLoc = -1;

// shader pour dessiner le canal alpha d'un des color buffers
GLint FrameBufferObject::m_ShaderAlphaId = -1;
GLint FrameBufferObject::m_VertexAlphaLoc = -1;
GLint FrameBufferObject::m_TexCoordAlphaLoc = -1;
GLint FrameBufferObject::m_BufferAlphaLoc = -1;

// shader pour dessiner le depth buffer
GLint FrameBufferObject::m_ShaderDepthId = -1;
GLint FrameBufferObject::m_VertexDepthLoc = -1;
GLint FrameBufferObject::m_TexCoordDepthLoc = -1;
GLint FrameBufferObject::m_BufferDepthLoc = -1;

// VBO pour dessiner un rectangle sur tout l'écran
GLint FrameBufferObject::m_VertexBufferId = -1;
GLint FrameBufferObject::m_TexCoordBufferId = -1;


/**
 * construit les structures de données permettant de dessiner les FBO sur l'écran
 * @see #onDraw
 */
void FrameBufferObject::staticinit()
{
    /// générer un shader et un VBO pour dessiner l'un des buffers de ce FBO
    const char* VertexShader =
        "#version 300 es\n"
        "in vec2 glVertex;\n"
        "in vec2 glTexCoord;\n"
        "out vec2 frgTexCoord;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(glVertex, 0.0, 1.0);\n"
        "    frgTexCoord = glTexCoord;\n"
        "}";

    const char* FragmentShaderRGB =
        "#version 300 es\n"
        "precision mediump float;\n"
        "uniform sampler2D Buffer;\n"
        "in vec2 frgTexCoord;\n"
        "out vec4 glFragColor;\n"
        "void main()\n"
        "{\n"
        "    glFragColor = texture(Buffer, frgTexCoord);\n"
        "}";

    // compiler le shader de dessin
    m_ShaderRGBId = Utils::makeShaderProgram(VertexShader, FragmentShaderRGB, "FrameBufferObjectRGB");

    // déterminer où sont les variables attribute et uniform
    m_VertexRGBLoc   = glGetAttribLocation(m_ShaderRGBId, "glVertex");
    m_TexCoordRGBLoc = glGetAttribLocation(m_ShaderRGBId, "glTexCoord");
    m_BufferRGBLoc   = glGetUniformLocation(m_ShaderRGBId, "Buffer");

    /// générer un shader et un VBO pour dessiner le canal alpha de l'un des buffers de ce FBO
    const char* FragmentShaderAlpha =
        "#version 300 es\n"
        "precision mediump float;\n"
        "uniform sampler2D Buffer;\n"
        "in vec2 frgTexCoord;\n"
        "out vec4 glFragColor;\n"
        "void main()\n"
        "{\n"
        "    float a = texture(Buffer, frgTexCoord).a;\n"
        "    glFragColor = vec4(a,a,a,1.0);\n"
        "}";

    // compiler le shader de dessin
    m_ShaderAlphaId = Utils::makeShaderProgram(VertexShader, FragmentShaderAlpha, "FrameBufferObjectAlpha");

    // déterminer où sont les variables attribute et uniform
    m_VertexAlphaLoc   = glGetAttribLocation(m_ShaderAlphaId, "glVertex");
    m_TexCoordAlphaLoc = glGetAttribLocation(m_ShaderAlphaId, "glTexCoord");
    m_BufferAlphaLoc   = glGetUniformLocation(m_ShaderAlphaId, "Buffer");

    /// générer un shader et un VBO pour dessiner le depth buffer de ce FBO
    const char* FragmentShaderDepth =
        "#version 300 es\n"
        "precision mediump float;\n"
        "uniform sampler2D Buffer;\n"
        "in vec2 frgTexCoord;\n"
        "out vec4 glFragColor;\n"
        "void main()\n"
        "{\n"
        "    float d = texture(Buffer, frgTexCoord).r;\n"
        "    glFragColor = vec4(d,d,d,1.0);\n"
        "}";

    // compiler le shader de dessin
    m_ShaderDepthId = Utils::makeShaderProgram(VertexShader, FragmentShaderDepth, "FrameBufferObjectDepth");

    // déterminer où sont les variables attribute et uniform
    m_VertexDepthLoc   = glGetAttribLocation(m_ShaderDepthId, "glVertex");
    m_TexCoordDepthLoc = glGetAttribLocation(m_ShaderDepthId, "glTexCoord");
    m_BufferDepthLoc   = glGetUniformLocation(m_ShaderDepthId, "Buffer");

    // créer et remplir le buffer des coordonnées
    std::vector<float> vertices =  { -1,-1,  +1,-1,  +1,+1,  -1,+1 };
    m_VertexBufferId = Utils::makeFloatVBO(vertices, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
    std::vector<float> texcoords = {  0, 0,   1, 0,   1, 1,   0, 1 };
    m_TexCoordBufferId = Utils::makeFloatVBO(texcoords, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
}


/**
 * supprime les shaders et VBO de dessin
 */
void FrameBufferObject::staticdestroy()
{
    Utils::deleteShaderProgram(m_ShaderRGBId);
    Utils::deleteShaderProgram(m_ShaderDepthId);
    Utils::deleteShaderProgram(m_ShaderAlphaId);
    Utils::deleteVBO(m_VertexBufferId);
    Utils::deleteVBO(m_TexCoordBufferId);
}


/**
 * dessine l'un des buffers dans le viewport (pour mise au point)
 * @param attachment : mettre GL_COLOR_ATTACHMENT0+i pour le ième color buffer, GL_DEPTH_ATTACHMENT pour le depth buffer
 * @param alpha : true si on dessine le canal alpha du color buffer
 */
void FrameBufferObject::onDraw(GLenum attachment, bool alpha)
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


/**
 * dessine le color buffer indiqué dans le viewport (pour mise au point)
 * @param attachment : mettre GL_COLOR_ATTACHMENT0+i pour le ième color buffer
 */
void FrameBufferObject::onDrawRGB(GLenum attachment)
{
    // désactiver le test du depth buffer
    glDisable(GL_DEPTH_TEST);

    // activer le shader
    glUseProgram(m_ShaderRGBId);

    // activer et lier le buffer contenant les coordonnées
    glEnableVertexAttribArray(m_VertexRGBLoc);
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
    glVertexAttribPointer(m_VertexRGBLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // activer et lier le buffer contenant les coordonnées de texture
    glEnableVertexAttribArray(m_TexCoordRGBLoc);
    glBindBuffer(GL_ARRAY_BUFFER, m_TexCoordBufferId);
    glVertexAttribPointer(m_TexCoordRGBLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // activer la texture à dessiner (ne marche pas si ce sont des RenderBuffers)
    setTextureUnit(GL_TEXTURE0, m_BufferRGBLoc, m_ColorBufferIds[attachment - GL_COLOR_ATTACHMENT0]);

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
void FrameBufferObject::onDrawAlpha(GLenum attachment)
{
    // désactiver le test du depth buffer
    glDisable(GL_DEPTH_TEST);

    // activer le shader
    glUseProgram(m_ShaderAlphaId);

    // activer et lier le buffer contenant les coordonnées
    glEnableVertexAttribArray(m_VertexAlphaLoc);
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
    glVertexAttribPointer(m_VertexAlphaLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // activer et lier le buffer contenant les coordonnées de texture
    glEnableVertexAttribArray(m_TexCoordAlphaLoc);
    glBindBuffer(GL_ARRAY_BUFFER, m_TexCoordBufferId);
    glVertexAttribPointer(m_TexCoordAlphaLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // activer la texture à dessiner (ne marche pas si ce sont des RenderBuffers)
    setTextureUnit(GL_TEXTURE0, m_BufferAlphaLoc, m_ColorBufferIds[attachment - GL_COLOR_ATTACHMENT0]);

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
void FrameBufferObject::onDrawDepth()
{
    // désactiver le test du depth buffer
    glDisable(GL_DEPTH_TEST);

    // activer le shader
    glUseProgram(m_ShaderDepthId);

    // activer et lier le buffer contenant les coordonnées
    glEnableVertexAttribArray(m_VertexDepthLoc);
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
    glVertexAttribPointer(m_VertexDepthLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // activer et lier le buffer contenant les coordonnées de texture
    glEnableVertexAttribArray(m_TexCoordDepthLoc);
    glBindBuffer(GL_ARRAY_BUFFER, m_TexCoordBufferId);
    glVertexAttribPointer(m_TexCoordDepthLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);

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

