#ifndef MISC_SCENEBASE_H
#define MISC_SCENEBASE_H

// Définition de la classe SceneBase

#include <vector>

// prédéclaration
class Scene;

#include <gl-matrix.h>
#include <utils.h>
#include <FrameBufferObject.h>
#include <ShadowMap.h>
#include <Light.h>


class SceneBase
{
protected:

    // matrice de projection et de transformation
    mat4 m_Mat4Projection;
    mat4 m_Mat4ModelView;

    // lampes de la scène
    std::vector<Light*> m_Lights;

    // dessin différé
    bool m_DeferredShading;
    FrameBufferObject* m_GBuffer;



public:

    /** constructeur, mettre true si le dessin se fait en deux passes */
    SceneBase(bool deferredShading);

    /** destructeur, libère les ressources */
    virtual ~SceneBase();

    /**
     * appelée quand la taille de la vue OpenGL change
     * @param width : largeur en nombre de pixels de la fenêtre
     * @param height : hauteur en nombre de pixels de la fenêtre
     * @param scale : facteur d'agrandissement du FBO pour simuler un antialiasing global
     */
    virtual void onSurfaceChanged(int width, int height, int scale=2);

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

    /**
     * appelée quand on enfonce le bouton gauche et qu'on déplace la souris
     * @param oldX : ancienne absisse de la souris
     * @param oldY : ancienne ordonnée de la souris
     * @param x : nouvelle absisse de la souris
     * @param y : nouvelle ordonnée de la souris
     * @param width : largeur de la vue OpenGL
     * @param height: hauteur de la vue OpenGL
     */
    virtual void onRotate(float oldX, float oldY, float x, float y, int width, int height);

    /**
     * appelée quand on appuie sur une touche du clavier
     * @param code : touche enfoncée
     */
    virtual void onKeyDown(unsigned char code);

    /**
     * dessin des objets de la scène sur l'écran
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    virtual void onDraw(mat4& mat4Projection, mat4& mat4ModelView);

    /**
     * dessine l'image courante
     */
    virtual void onDrawFrame();

protected:

    /** affiche les informations sur la caméra **/
    virtual void updateInfos();

    /** retourne la matrice ModelView pour dessiner la scène */
    virtual mat4 getModelView();

    /**
     * ajoute une lampe pour éclairer le matériau du renderer
     * @param light : lampe à ajouter
     */
    void addLight(Light* light);

    /**
     * transforme toutes les lampes
     * c'est à dire calcule la position relative à la caméra m_PositionCamera
     * à partir de la position relative à la scène m_Position
     * @param mat4View matrice de positionnement de la scène par rapport à la caméra
     */
    virtual void transformLights(mat4& mat4View);

    /**
     * dessine les shadow maps des lampes
     * @param mat4View : matrice de positionnement de la scène par rapport à la caméra
     */
    virtual void makeShadowMaps(mat4& mat4View);

    /**
     * transforme les lampes puis calcule leur shadow map s'il y en a
     * @param mat4View : matrice de positionnement de la scène par rapport à la caméra
     */
    virtual void prepareLights(mat4& mat4View);

    /**
     * appelée pour dessiner en mode MRT
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    void drawDeferredShading(mat4& mat4Projection, mat4& mat4ModelView);

    /**
     * rajoute l'éclairement d'une lampe
     * @param light : lampe à ajouter
     * @param is_first : mettre true si c'est la première qu'on ajoute ainsi, false sinon
     */
    void addLighting(Light* light, bool is_first=false);

    /** rajoute l'éclairement de toutes les lampes */
    void addLightings();
};

#endif
