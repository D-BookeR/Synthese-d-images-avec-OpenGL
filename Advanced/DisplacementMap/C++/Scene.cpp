#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

#include <utils.h>
#include "Scene.h"



/**
 * Constructeur
 */
Scene::Scene() : TrackBallScene(false)
{
    // créer l'objet
    m_Object = new Rectangle();

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);   // pour voir l'arrière du rectangle

    // couleur du fond
    glClearColor(0.2, 0.2, 0.2, 0.0);

    // paramètres de la vue
    m_CameraDistance = 12.0;
    quat::setAxisAngle(m_QuatP1P2, vec3::fromValues(-1,0.5,0), Utils::radians(30.0));
    quat::multiply(m_QuatOrientation, m_QuatP1P2, m_QuatOrientation);
}


/**
 * appelée quand la taille de la vue OpenGL change
 * @param width : largeur en nombre de pixels de la fenêtre
 * @param height : hauteur en nombre de pixels de la fenêtre
 */
void Scene::onSurfaceChanged(int width, int height)
{
    // appeler la méthode de la superclasse
    TrackBallScene::onSurfaceChanged(width, height);

    // matrice de projection (champ de vision)
    mat4::perspective(m_Mat4Projection, Utils::radians(10.0), (float)width / height, 0.1, 20.0);
}


/**
 * dessine les objets de la scène
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Scene::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
{
    // dessiner l'objet
    m_Object->onDraw(mat4Projection, mat4ModelView);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_Object;
}
