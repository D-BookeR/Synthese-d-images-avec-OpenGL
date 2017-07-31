#ifndef SCENE_H
#define SCENE_H

// Définition de la classe Scene

#include <gl-matrix.h>
#include <utils.h>
#include <FrameBufferObject.h>
#include <TurnTableScene.h>
#include <Light.h>
#include <OmniLight.h>

#include "WaterMaterial.h"
#include "Water.h"
#include "Island.h"
#include "Ground.h"
#include "Fog.h"


class Scene: public TurnTableScene
{
private:

    // objets de la scène
    WaterMaterial* m_WaterMaterial;
    Island* m_Island;
    Water* m_Water;
    Ground* m_Ground;

    Light* m_Light0;
    OmniLight* m_Light1;

    FrameBufferObject* m_FBOreflection;
    FrameBufferObject* m_FBObackground;
    FrameBufferObject* m_FBOfog;

    Fog* m_Fog;

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

    /**
     * dessin des objets de la scène sur l'écran
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    void onDraw(mat4& mat4Projection, mat4& mat4ModelView);

    /** Dessine l'image courante */
    void onDrawFrame();

};

#endif
