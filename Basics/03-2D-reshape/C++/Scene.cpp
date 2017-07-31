#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>

#include <utils.h>

#include "Scene.h"



/** constructeur */
Scene::Scene()
{
    // créer les objets à dessiner
    m_Triangle = new Triangle();

    // couleur du fond : gris foncé
    glClearColor(0.4, 0.4, 0.4, 0.0);
}


/**
 * appelée quand la taille de la vue OpenGL change
 * @param width : largeur en nombre de pixels de la fenêtre
 * @param height : hauteur en nombre de pixels de la fenêtre
 */
void Scene::onSurfaceChanged(int width, int height)
{
    // met en place le viewport
    glViewport(0, 0, width, height);
}


/**
 * Dessine l'image courante
 */
void Scene::onDrawFrame()
{
    // effacer l'écran
    glClear(GL_COLOR_BUFFER_BIT);

    // dessiner le triangle
    m_Triangle->onDraw();
}


/** supprime tous les objets de cette scène */
Scene::~Scene()
{
    delete m_Triangle;
}
