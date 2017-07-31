#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>

#include <utils.h>

#include "Scene.h"


/**
 * Constructeur
 */
Scene::Scene()
{
    // créer l'objet à dessiner
    m_Earth = new Earth(64,32);

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);

    // couleur du fond : gris foncé
    glClearColor(0.4, 0.4, 0.4, 0.0);

    // paramètres de la vue
    m_Mat4Projection = mat4::create();
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

    // matrice de projection (champ de vision)
    mat4::perspective(m_Mat4Projection, Utils::radians(20.0), (float)width / height, 0.1, 20.0);
}


/**
 * dessine les objets de la scène
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Scene::onDraw(mat4 mat4Projection, mat4 mat4ModelView)
{
    m_Earth->onDraw(mat4Projection, mat4ModelView);
}


/**
 * Dessine l'image courante
 */
void Scene::onDrawFrame()
{
    // effacer l'écran
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // position de la lumière animée en fonction du temps
    mat4 lightMatrix = mat4::create();
    mat4::identity(lightMatrix);
    mat4::rotate(lightMatrix, lightMatrix, Utils::radians(Utils::Time*20.0), vec3::fromValues(0.1, 1, 0));
    vec4 pos_light = vec4::fromValues(0.0, 2.0, 10.0, 1.0);
    vec4::transformMat4(pos_light, pos_light, lightMatrix);
    m_Earth->setLightPosition(pos_light);

    // construire la matrice MV animée en fonction du temps
    mat4 mat4ModelView = mat4::create();
    mat4::identity(mat4ModelView);
    mat4::translate(mat4ModelView, mat4ModelView, vec3::fromValues(0.0, 0.0, -10));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(Utils::Time*25.0));
    mat4::translate(mat4ModelView, mat4ModelView, vec3::fromValues(0.5, 0.0, 0.0));

    // dessiner les objets
    onDraw(m_Mat4Projection, mat4ModelView);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_Earth;
}
