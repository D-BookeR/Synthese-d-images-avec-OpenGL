#ifndef SCENE_H
#define SCENE_H

// Définition de la classe Scene

#include <gl-matrix.h>
#include <utils.h>
#include <SceneBase.h>
#include <AxesXYZ.h>
#include <Light.h>
#include <OmniLight.h>

#include "Earth.h"


class Scene: public SceneBase
{
private:

    Earth* m_Earth;
    AxesXYZ* m_Axes;

    OmniLight* m_Light0;
    Light* m_Light1;


    // paramètres de la vue
    mat4 m_Mat4Projection;
    quat m_QuatOrientation;
    float m_CameraZ;


public:

    /** constructeur, crée les objets 3D à dessiner */
    Scene();

    /** destructeur, libère les ressources */
    ~Scene();

    /**
     * appelée quand la taille de la vue OpenGL change
     * @param width : largeur en nombre de pixels de la fenêtre
     * @param height : hauteur en nombre de pixels de la fenêtre
     */
    void onSurfaceChanged(int width, int height);

    /**
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    void onDraw(mat4& mat4Projection, mat4& mat4ModelView);

    /** Dessine l'image courante */
    void onDrawFrame();

    /**
     * appelée quand on enfonce le bouton du milieu et qu'on déplace la souris
     * @param dz : nombre de pixels parcourus
     */
    void onZoom(float dz);

    /**
     * appelée quand on enfonce le bouton droit et qu'on déplace la souris
     * @param dx : nombre de pixels parcourus horizontalement
     * @param dy : nombre de pixels parcourus verticalement
     */
    void onPan(float dx, float dy);

    /**
     * appelée quand on enfonce le bouton gauche et qu'on déplace la souris
     * @param oldX : ancienne absisse de la souris
     * @param oldY : ancienne ordonnée de la souris
     * @param x : nouvelle absisse de la souris
     * @param y : nouvelle ordonnée de la souris
     * @param width : largeur de la vue OpenGL
     * @param height : hauteur de la vue OpenGL
     */
    void onRotate(float oldX, float oldY, float x, float y, int width, int height);

    /**
     * appelée quand on appuie sur une touche du clavier
     * @param code : touche enfoncée
     */
    void onKeyDown(unsigned char code);

private:

    /** affiche les informations sur la caméra **/
    void updateInfos();
};

#endif
