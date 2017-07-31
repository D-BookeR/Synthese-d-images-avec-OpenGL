#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>

#include <utils.h>

#include "Scene.h"


/**
 * Constructeur
 */
Scene::Scene() : SceneBase(false)
{
    // charger la texture
    m_Texture = new Texture2D("data/textures/1024px-LandscapeArchPano.jpg");

    // traitement d'image
    m_BlackAndWhite = new BlackAndWhite();

    // désactiver les tests de profondeur
    glDisable(GL_DEPTH_TEST);
}


/**
 * Dessine l'image courante
 */
void Scene::onDrawFrame()
{
    // on applique le traitement d'image sur la texture et ça remplit l'écran
    m_BlackAndWhite->process(m_Texture);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_BlackAndWhite;
    delete m_Texture;
}
