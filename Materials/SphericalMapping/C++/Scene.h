#ifndef SCENE_H
#define SCENE_H

// Définition de la classe Scene

#include <gl-matrix.h>
#include <utils.h>
#include <Texture2D.h>
#include <FrameBufferObject.h>
#include <TurnTableScene.h>

#include "Rock.h"
#include "Sphere.h"


class Scene: public TurnTableScene
{
private:

    Texture2D* m_Texture;
    Rock* m_Rock;
    Sphere* m_Sphere;

    bool m_DisplaySphere;

    FrameBufferObject* m_FBOpreceeding;
    FrameBufferObject* m_FBOcurrent;
    FrameBufferObject* m_FBOresult;

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

};

#endif
