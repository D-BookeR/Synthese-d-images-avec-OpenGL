#ifndef FRAMEBUFFEROBJECTMS_H
#define FRAMEBUFFEROBJECTMS_H

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

#include <FrameBufferObject.h>


class FrameBufferObjectMS: public FrameBufferObject
{
public:

    /**
     * constructeur de la classe FrameBufferObjectMS, un Frame Buffer Object pour le multisampling
     * @param width : largeur du FBO (nombre de pixels)
     * @param height : hauteur du FBO
     * @param samplesnb : nombre d'échantillons 1..GL_MAX_SAMPLES
     * @param color : fournir GL_NONE si aucun, GL_TEXTURE_2D si on veut un buffer de type texture, GL_RENDERBUFFER si c'est un renderbuffer
     * @param depth : fournir GL_NONE si aucun, GL_TEXTURE_2D si on veut un buffer de type texture, GL_RENDERBUFFER si c'est un renderbuffer
     * @param colorsnb : nombre de color buffer supplémentaires pour faire du dessin différé (gl_FragData[i])
     */
    FrameBufferObjectMS(int width, int height, int samplesnb, GLenum color=GL_TEXTURE_2D, GLenum depth=GL_RENDERBUFFER, int colorsnb=0);

    // destructeur
    virtual ~FrameBufferObjectMS();


protected:

    /**
     * cette fonction associe l'une des textures de ce FBO à une unité pour un shader
     * @note le shader concerné doit être actif
     * @param unit : unité de texture concernée, par exemple GL_TEXTURE0
     * @param locSampler : emplacement de la variable uniform sampler* de cette texture dans le shader ou 0 pour désactiver la texture
     * @param bufferId : m_ColorBuffer ou m_DepthBufferId ou 0
     */
    virtual void setTextureUnit(GLint unit, GLint locSampler=-1, GLuint bufferId=0);


    /** nombre d'échantillons */
    int m_SamplesNumber;

};


#endif
