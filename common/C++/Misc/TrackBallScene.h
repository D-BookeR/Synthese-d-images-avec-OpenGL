#ifndef MISC_TRACKBALLSCENE_H
#define MISC_TRACKBALLSCENE_H

// Définition de la classe TrackBallScene

#include <vector>

#include <gl-matrix.h>
#include <utils.h>
#include <FrameBufferObject.h>
#include <Light.h>
#include <SceneBase.h>


class TrackBallScene: public SceneBase
{
protected:

    // paramètres de la vue
    quat m_QuatP1P2;
    quat m_QuatOrientation;
    float m_CameraDistance;


public:

    /** constructeur, mettre true si le dessin se fait en deux passes */
    TrackBallScene(bool deferredShading);

    /** destructeur, libère les ressources */
    virtual ~TrackBallScene();

    /**
     * appelée quand on enfonce le bouton du milieu et qu'on déplace la souris
     * @param dd : nombre de pixels parcourus
     */
    virtual void onZoom(float dd);

    /**
     * appelée quand on enfonce le bouton droit et qu'on déplace la souris
     * @param dx : nombre de pixels parcourus horizontalement
     * @param dy : nombre de pixels parcourus verticalement
     */
    virtual void onPan(float dx, float dy);

    /** appelée quand on enfonce le bouton gauche et qu'on déplace la souris */
    virtual void onRotate(float oldX, float oldY, float x, float y, int width, int height);

    /**
     * appelée quand on appuie sur une touche du clavier
     * @param code : touche enfoncée
     */
    virtual void onKeyDown(unsigned char code);

protected:

    /** affiche les informations sur la caméra **/
    virtual void updateInfos();

    /** retourne la matrice ModelView pour dessiner la scène */
    virtual mat4 getModelView();
};

#endif
