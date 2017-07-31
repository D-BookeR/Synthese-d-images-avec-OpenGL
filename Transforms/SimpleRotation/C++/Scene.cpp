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
    m_TriangleGLSL = new TriangleGLSL();
    m_TriangleProg = new TriangleProg();

    // couleur du fond : gris foncé
    glClearColor(0.4, 0.4, 0.4, 0.0);

    // viewport
    m_Width = 0;
    m_Height = 0;
}


/**
 * appelée quand la taille de la vue OpenGL change
 * @param width : largeur en nombre de pixels de la fenêtre
 * @param height : hauteur en nombre de pixels de la fenêtre
 */
void Scene::onSurfaceChanged(int width, int height)
{
    // mémorise le viewport
    m_Width = width/2;
    m_Height = height;
}


/**
 * Dessine l'image courante
 */
void Scene::onDrawFrame()
{
    // effacer l'écran
    glClear(GL_COLOR_BUFFER_BIT);

    // viewport pour la vue de gauche
    glViewport(0, 0, m_Width, m_Height);

    // dessiner le triangle géré par GLSL
    m_TriangleGLSL->onDraw();

    // viewport pour la vue de droite
    glViewport(m_Width, 0, m_Width, m_Height);

    // dessiner le triangle géré par programme
    m_TriangleProg->onDraw();
}


/** supprime tous les objets de cette scène */
Scene::~Scene()
{
    delete m_TriangleGLSL;
    delete m_TriangleProg;
}
