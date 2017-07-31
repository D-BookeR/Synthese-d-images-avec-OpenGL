#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

#include <utils.h>
#include <Rectangle.h>
#include "Scene.h"



/**
 * Constructeur
 */
Scene::Scene()
{
    // créer l'objet
    m_Object = new Rectangle();

    // matrices de projection et vue = identité
    m_Identity = mat4::create();
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
 * dessine les objets de la scène
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Scene::onDraw(mat4 mat4Projection, mat4 mat4ModelView)
{
}


/**
 * dessin des objets de la scène sur l'écran
 */
void Scene::onDrawFrame()
{
    // dessiner l'objet
    m_Object->onDraw(m_Identity, m_Identity);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_Object;
}
