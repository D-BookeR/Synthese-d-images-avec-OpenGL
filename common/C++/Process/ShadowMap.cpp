//// ce script fournit des fonctions utilitaires pour les programmes
//// du livre Synthèse d'images à l'aide d'OpenGL

// consulter http://blog.tojicode.com/2012/07/using-webgldepthtexture.html

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <ShadowMap.h>


/**
 * constructeur de la classe ShadowMap
 * @param shadowmapsize : largeur et hauteur de la carte d'ombre
 * @param offsetfill : true si la shadow map doit activer le décalage de polygones
 * @param cullface : mettre GL_FRONT ou GL_BACK pour éliminer les faces avant ou de dos, GL_NONE pour ne rien configurer
 */
ShadowMap::ShadowMap(int shadowmapsize, bool offsetfill, GLenum cullface):
    // c'est un FBO ne contenant qu'une depth map de type texture
    FrameBufferObject(shadowmapsize, shadowmapsize, GL_NONE, GL_TEXTURE_2D, 0, GL_LINEAR)
{
    // options de réduction de l'acné
    m_OffsetFill = offsetfill;
    m_CullFace   = cullface;
    m_CullFacePrec = 0;
}


/**
 * supprime cette shadowmap
 */
ShadowMap::~ShadowMap()
{
}


/**
 * cette méthode change les modes de réduction de l'acné de surface
 * @param offsetfill : true si la shadow map doit activer le décalage de polygones
 * @param cullface : mettre GL_FRONT ou GL_BACK pour éliminer les faces avant ou de dos, GL_NONE pour ne rien configurer
 */
void ShadowMap::setAcneReduction(bool offsetfill, GLenum cullface)
{
    m_OffsetFill = offsetfill;
    m_CullFace   = cullface;
}


/**
 * redirige tous les tracés suivants vers la ShadowMap
 */
void ShadowMap::enable()
{
    // activer le FBO
    FrameBufferObject::enable();

    // (optionnel) éliminer les faces avant, afin d'éviter l'acné de surface
    glGetIntegerv(GL_CULL_FACE_MODE, &m_CullFacePrec);
    if (m_CullFace != GL_NONE) {
        glEnable(GL_CULL_FACE);
        glCullFace(m_CullFace);
    }

    // (optionnel) décalage de polygones, afin d'éviter l'acné de surface
    if (m_OffsetFill) {
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.0, 1.0);
    }

    // effacer le depth buffer
    glClear(GL_DEPTH_BUFFER_BIT);
}


/**
 * cesser de rediriger les dessins dans la ShadowMap
 */
void ShadowMap::disable()
{
    // désactiver le FBO
    FrameBufferObject::disable();

    // remettre les modes tels qu'ils étaient avant
    glCullFace(m_CullFacePrec);

    // annuler le décalage
    glPolygonOffset(0.0, 0.0);
    glDisable(GL_POLYGON_OFFSET_FILL);
}


/**
 * cette fonction associe cette shadowmap en tant que texture2D à une unité de texture pour un shader
 * NB: le shader concerné doit être actif
 * @param unit : unité de texture concernée, par exemple GL_TEXTURE0
 * @param locSampler : emplacement de la variable uniform sampler* de cette texture dans le shader ou -1 pour désactiver la texture
 */
void ShadowMap::setTextureUnit(GLint unit, GLint locSampler)
{
    /*****DEBUG*****/
    if (unit < GL_TEXTURE0 || unit > GL_TEXTURE7) {
        throw std::invalid_argument("ShadowMap::setTextureUnit: first parameter, unit is not GL_TEXTURE0 ... GL_TEXTURE7");
    }
    /*****DEBUG*****/

    // activer la shadow map en tant que texture
    glActiveTexture(unit);
    if (locSampler < 0) {
        glBindTexture(GL_TEXTURE_2D, 0);
    } else {
        glBindTexture(GL_TEXTURE_2D, m_DepthBufferId);
        glUniform1i(locSampler, unit-GL_TEXTURE0);
    }
}


/**
 * dessine le depth buffer dans le viewport (pour la mise au point)
 */
void ShadowMap::onDraw()
{
    FrameBufferObject::onDrawDepth();
}


mat4 ShadowMap::c_MatBias;

/** initialise la matrice de biais */
void ShadowMap::staticinit()
{
    c_MatBias = mat4::create();
    mat4::translate(c_MatBias, c_MatBias, vec3::fromValues(0.5, 0.5, 0.5));
    mat4::scale(c_MatBias, c_MatBias, vec3::fromValues(0.5, 0.5, 0.5));
}
