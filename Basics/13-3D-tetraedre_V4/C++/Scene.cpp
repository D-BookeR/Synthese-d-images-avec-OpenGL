#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

#include <utils.h>

#include "Scene.h"


/** constructeur */
Scene::Scene()
{
    // créer les objets à dessiner
    m_Tetraedre = new Tetraedre();

    // couleur du fond : gris foncé
    glClearColor(0.4, 0.4, 0.4, 0.0);

    // activer le depth buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // créer une matrice de rotation
    mat4 rotation = mat4::create();
    mat4::rotate(rotation, mat4::create(), Utils::radians(25.0 * Utils::Time), vec3::fromValues(1,2,0));

    // dessiner le tétraèdre
    m_Tetraedre->onDraw(rotation);
}


/** supprime tous les objets de cette scène */
Scene::~Scene()
{
    delete m_Tetraedre;
}
