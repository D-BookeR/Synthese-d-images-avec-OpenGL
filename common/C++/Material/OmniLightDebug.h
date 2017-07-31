#ifndef MATERIAL_OMNILIGHTDEBUG_H
#define MATERIAL_OMNILIGHTDEBUG_H

/**
 * Définition de la classe OmniLight, une base pour construire des éclairages diffus et localisés
 */

#include <gl-matrix.h>
#include <utils.h>

#include <OmniLight.h>


class OmniLightDebug: public OmniLight
{
public:

    /**
     * constructeur
     * C'est une lampe toute simple, positionnelle ou directionnelle, sans ombre
     */
    OmniLightDebug();

    /**
     * destructeur
     */
    virtual ~OmniLightDebug();

    /**
     * dessine une représentation de la lampe afin de la voir pendant la mise au point
     * @param mat4Projection : matrice de projection
     * @param mat4View : matrice de vue (nb: pas utilisée)
     * @param taille : taille de l'étoile représentant la lampe
     */
    virtual void onDraw(mat4& mat4Projection, mat4& mat4View, float taille=1.0);

    /**
     * dessine la shadowmap de cette lampe sur l'écran (remplit tout l'écran)
     * NB: cette méthode ne fait rien dans le cas d'une lampe omnidirectionnelle
     */
    virtual void onDrawShadowMap();


protected:

    /** pour dessiner la lampe */
    GLuint m_DebugShaderId;
    GLint m_DebugVertexLoc;
    GLint m_DebugMatProjectionLoc;
    GLint m_DebugMatModelViewLoc;
    GLint m_DebugVertexBufferId;
    int m_DebugLineCount;

};

#endif
