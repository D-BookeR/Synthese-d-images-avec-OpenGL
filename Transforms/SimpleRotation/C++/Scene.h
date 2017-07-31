#ifndef SCENE_H
#define SCENE_H

// Définition de la classe Scene

#include <gl-matrix.h>

#include "TriangleGLSL.h"
#include "TriangleProg.h"


class Scene
{
private:

    TriangleGLSL* m_TriangleGLSL;
    TriangleProg* m_TriangleProg;

    int m_Width, m_Height;

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
