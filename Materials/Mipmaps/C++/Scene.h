#ifndef SCENE_H
#define SCENE_H

// Définition de la classe Scene

#include <gl-matrix.h>
#include <utils.h>
#include <TurnTableScene.h>
#include <Texture2D.h>
#include <TextureMaterial.h>

#include "Rectangle.h"


class Scene: public TurnTableScene
{
private:

    // textures
    Texture2D* m_TextureAvec;
    Texture2D* m_TextureWithout;

    // matériaux
    TextureMaterial* m_MaterialWith;
    TextureMaterial* m_MaterialWithout;

    // rectangles
    Rectangle* m_ObjectWith;
    Rectangle* m_ObjectWithout;



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
     * dessin des objets de la scène sur l'écran
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    void onDraw(mat4& mat4Projection, mat4& mat4ModelView);

};

#endif
