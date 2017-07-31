#ifndef MATERIAL_SPOTLIGHTDEBUG_H
#define MATERIAL_SPOTLIGHTDEBUG_H

/**
 * Définition de la classe SpotLight, un type de lampe plus sophistiqué
 */

#include <gl-matrix.h>
#include <utils.h>
#include <SpotLight.h>
#include <ShadowMap.h>


class SpotLightDebug: public SpotLight
{
public:

    /**
     * constructeur
     * @param anglemax : angle en degrés d'ouverture total de la lampe
     * @param anglemin : angle dans lequel l'intensité est 100%, il y a un dégradé entre anglemin et anglemax
     * @param shadowmapsize : taille de la shadowmap en pixels si la lampe gère des ombres
     * @param near : distance la plus proche dans la shadowmap
     * @param far : distance la plus lointaine dans la shadowmap
     * @param offsetfill : true si la shadow map doit activer le décalage de polygones
     * @param cullface : mettre GL_FRONT ou GL_BACK pour éliminer les faces avant ou de dos, GL_NONE pour ne rien configurer
     */
    SpotLightDebug(float anglemax, float anglemin, int shadowmapsize, float near, float far, bool offsetfill, GLenum cullface);
    SpotLightDebug(float anglemax, float anglemin, int shadowmapsize, float near, float far);
    SpotLightDebug(float anglemax, float anglemin);

    /**
     * destructeur
     */
    virtual ~SpotLightDebug();

    /**
     * dessine une représentation de la lampe
     * @param mat4Projection : matrice de projection
     * @param mat4View : matrice de vue, ne sert pas car on prend directement les positions caméra
     */
    virtual void onDraw(mat4& mat4Projection, mat4& mat4View);

    /**
     * dessine la shadowmap de cette lampe sur l'écran (remplit tout l'écran)
     * NB: cette méthode ne fait rien dans le cas d'une lampe omnidirectionnelle
     */
    virtual void onDrawShadowMap();


private:

    /** initialisations communes à tous les constructeurs */
    void init();


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
