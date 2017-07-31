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
    m_RedTriangle = new RedTriangle();
    m_GreenTriangle = new GreenTriangle(0.75);

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

    // dessiner le triangle rouge
    m_RedTriangle->onDraw();

    // mettre en place le blending
    glEnable(GL_BLEND);

    // formule de blending
    glBlendEquation(GL_FUNC_ADD);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFunc(GL_ONE, GL_SRC_COLOR);

    // dessiner le triangle vert
    m_GreenTriangle->onDraw();

    // arrêter le blending
    glDisable(GL_BLEND);
}


/** supprime tous les objets de cette scène */
Scene::~Scene()
{
    delete m_GreenTriangle;
    delete m_RedTriangle;
}
