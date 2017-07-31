#ifndef MISC_FIRSTPERSONSCENE_H
#define MISC_FIRSTPERSONSCENE_H

// Définition de la classe FirstPersonScene

#include <vector>

#include <gl-matrix.h>
#include <utils.h>
#include <FrameBufferObject.h>
#include <Light.h>
#include <SceneBase.h>


class FirstPersonScene: public SceneBase
{
protected:

    // paramètres de la vue
    float m_CameraAngleAzimut;
    float m_CameraAngleElevation;
    float m_CameraDistance;
    vec3 m_CameraInvPosition;
    mat4 m_Mat4TMP;


public:

    /** constructeur, mettre true si le dessin se fait en deux passes */
    FirstPersonScene(bool deferredShading);

    /** destructeur, libère les ressources */
    virtual ~FirstPersonScene();

    /**
     * appelée quand on enfonce le bouton du milieu et qu'on déplace la souris
     * @param dz : nombre de pixels parcourus
     */
    virtual void onZoom(float dz);

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
