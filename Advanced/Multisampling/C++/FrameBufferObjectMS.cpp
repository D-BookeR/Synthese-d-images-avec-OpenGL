//// ce fichier source fournit des fonctions utilitaires pour les programmes
//// du livre Synthèse d'images à l'aide d'OpenGL

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <FrameBufferObjectMS.h>


/**
 * constructeur de la classe FrameBufferObjectMS
 * @param width : largeur du FBO (nombre de pixels)
 * @param height : hauteur du FBO
 * @param samplesnb : nombre d'échantillons 1..GL_MAX_SAMPLES
 * @param color : fournir GL_NONE si aucun, GL_TEXTURE_2D si on veut un buffer de type texture, GL_RENDERBUFFER si c'est un renderbuffer, GL_TEXTURE_CUBE_MAP si on veut un buffer de type cube map
 * @param depth : fournir GL_NONE si aucun, GL_TEXTURE_2D si on veut un buffer de type texture, GL_RENDERBUFFER si c'est un renderbuffer, NB: il faut impérativement un depth buffer dans un FBO destiné à être rendu
 * @param colorsnb : nombre de color buffer supplémentaires pour faire du dessin différé (glFragData[i])
 */
FrameBufferObjectMS::FrameBufferObjectMS(int width, int height, int samplesnb, GLenum color, GLenum depth, int colorsnb)
{
    // test sur les paramètres pour éviter des bizarreries
    if (colorsnb > 0 && color == GL_NONE) {
        throw std::invalid_argument("FrameBufferObjectMS: colorsnb>0 but no main color buffer");
    }

    // variables d'instance
    m_FBO = 0;
    m_DepthBufferId = 0;
    m_Width = width;
    m_Height = height;
    m_SamplesNumber = samplesnb;
    m_PrecFBO = 0;
    std::vector<GLenum> m_DrawBufferNames;

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
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, bufferId);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_SamplesNumber, GL_RGBA, width, height, GL_TRUE);

        // attacher la texture au FBO
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, bufferId, 0);
        m_ColorBufferIds.push_back(bufferId);
        m_DrawBufferNames.push_back(GL_COLOR_ATTACHMENT0);
        break;

    case GL_RENDERBUFFER:
        // lui ajouter un color buffer de type render buffer
        glGenRenderbuffers(1, &bufferId);
        glBindRenderbuffer(GL_RENDERBUFFER, bufferId);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_SamplesNumber, GL_RGBA, width, height);

        // attacher le render buffer au FBO
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, bufferId);
        m_ColorBufferIds.push_back(bufferId);
        m_DrawBufferNames.push_back(GL_COLOR_ATTACHMENT0);
        break;

    case GL_NONE:
        break;

    default:
        std::cerr << "FrameBufferObjectMS: color is not among NONE, TEXTURE_2D, TEXTURE_CUBE_MAP, RENDERBUFFER" << std::endl;
    }

    // créer le buffer de profondeur principal, rempli automatiquement par gl_FragDepth
    switch (depth) {

    case GL_TEXTURE_2D:
        // lui ajouter un depth buffer de type texture
        glGenTextures(1, &m_DepthBufferId);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_DepthBufferId);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_SamplesNumber, GL_DEPTH_COMPONENT, width, height, GL_TRUE);

        // couleur du bord
        glTexParameterfv(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_BORDER_COLOR, borderColor);

        // attacher le depth buffer au FBO
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, m_DepthBufferId, 0);
        break;

    case GL_RENDERBUFFER:
        // lui ajouter un depth buffer de type render buffer
        glGenRenderbuffers(1, &m_DepthBufferId);
        glBindRenderbuffer(GL_RENDERBUFFER, m_DepthBufferId);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_SamplesNumber, GL_DEPTH_COMPONENT, width, height);

        // attacher le depth buffer au FBO
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthBufferId);
        break;

    case GL_NONE:
        // voir la remarque dans https://www.opengl.org/wiki/Framebuffer_Object
        // Even if you don't plan on reading from this depth_attachment, an off screen buffer that will be rendered to should have a depth attachment.
        break;

    default:
        std::cerr << "FrameBufferObjectMS: depth is not among NONE, TEXTURE_2D, TEXTURE_CUBE_MAP, RENDERBUFFER" << std::endl;
    }

    // créer des buffers supplémentaires si c'est demandé
    for (int i=0; i<colorsnb; i++) {

        // créer une texture 2D pour recevoir les dessins (voir glFragData dans les shaders)
        glGenTextures(1, &bufferId);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, bufferId);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_SamplesNumber, GL_RGBA32F, width, height, GL_TRUE);

        // attacher la texture au FBO
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1+i, GL_TEXTURE_2D_MULTISAMPLE, bufferId, 0);
        m_ColorBufferIds.push_back(bufferId);
        m_DrawBufferNames.push_back(GL_COLOR_ATTACHMENT1+i);
    }

    // indiquer quels sont les buffers utilisés pour le dessin (glFragData valides)
    glDrawBuffers(m_DrawBufferNames.size(), &m_DrawBufferNames[0]);

    // vérifier l'état des lieux
    checkStatus();

    // désactiver le FBO pour l'instant
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



/**
 * supprime ce FBO
 */
FrameBufferObjectMS::~FrameBufferObjectMS()
{
}



/**
 * cette fonction associe l'une des textures de ce FBO à une unité pour un shader
 * NB: le shader concerné doit être actif
 * @param unit : unité de texture concernée, par exemple GL_TEXTURE0
 * @param locSampler : emplacement de la variable uniform sampler* de cette texture dans le shader ou -1 pour désactiver la texture
 * @param bufferId : l'un des buffers retourné par getColorBuffer ou getDepthBuffer ou 0 pour désactiver
 */
void FrameBufferObjectMS::setTextureUnit(GLint unit, GLint locSampler, GLuint bufferId)
{
    /*****DEBUG*****/
    if (unit < GL_TEXTURE0 || unit > GL_TEXTURE7) {
        throw std::invalid_argument("FrameBufferObject::setTextureUnit: first parameter, unit is not GL_TEXTURE0 ... GL_TEXTURE7");
    }
    /*****DEBUG*****/
    glActiveTexture(unit);
    if (locSampler < 0 || bufferId <= 0) {
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    } else {
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, bufferId);
        glUniform1i(locSampler, unit-GL_TEXTURE0);
    }
}
