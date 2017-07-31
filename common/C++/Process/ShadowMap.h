#ifndef PROCESS_SHADOWMAP_H
#define PROCESS_SHADOWMAP_H

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


/**
 * C'est un FBO particulier, il n'y a qu'un depth buffer
 * On l'utilise pour dessiner les ombres dans une scène
 */
class ShadowMap: public FrameBufferObject
{
public:

    /**
     * constructeur de la classe ShadowMap
     * @param width : largeur et hauteur de la carte d'ombre
     * @param offsetfill : true si la shadow map doit activer le décalage de polygones
     * @param cullface : mettre GL_FRONT ou GL_BACK pour éliminer les faces avant ou de dos, GL_NONE pour ne rien configurer
     */
    ShadowMap(int width, bool offsetfill=false, GLenum cullface=GL_NONE);

    // destructeur
    virtual ~ShadowMap();

    /**
     * cette méthode change les modes de réduction de l'acné de surface
     * @param offsetfill : true si la shadow map doit activer le décalage de polygones
     * @param cullface : mettre GL_FRONT ou GL_BACK pour éliminer les faces avant ou de dos, GL_NONE pour ne rien configurer
     */
    void setAcneReduction(bool offsetfill, GLenum cullface);

    /**
     * redirige tous les tracés suivants vers le FBO
     */
    virtual void enable();

    /**
     * redirige tous les tracés suivants vers l'écran à nouveau
     */
    virtual void disable();

    /**
     * cette fonction associe cette shadowmap en tant que texture2D à une unité de texture pour un shader
     * NB: le shader concerné doit être actif
     * @param unit : unité de texture concernée, par exemple GL_TEXTURE0
     * @param locSampler : emplacement de la variable uniform sampler* de cette texture dans le shader ou 0 pour désactiver la texture
     */
    void setTextureUnit(GLint unit, GLint locSampler=-1);

    /** dessine le depth buffer dans le viewport (pour mise au point) */
    void onDraw();

    /** initialise la matrice de biais */
    static void staticinit();

    // matrice de décalage des coordonnées de texture [-1,+1] -> [0,1]
    static mat4 c_MatBias;

protected:

    // informations
    GLint m_CullFacePrec;

    // options de réduction de l'acné
    bool m_OffsetFill;
    GLenum m_CullFace;
};


#endif
