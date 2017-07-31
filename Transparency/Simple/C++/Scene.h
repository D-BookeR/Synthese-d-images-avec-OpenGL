#ifndef SCENE_H
#define SCENE_H

// Définition de la classe Scene

#include <gl-matrix.h>

#include "RedTriangle.h"
#include "GreenTriangle.h"

class Scene
{
private:

    RedTriangle* m_RedTriangle;
    GreenTriangle* m_GreenTriangle;

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

    /** Dessine l'image courante */
    void onDrawFrame();
};

#endif
