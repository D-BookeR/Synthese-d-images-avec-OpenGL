#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

#include <utils.h>
#include <Cube3Mats.h>
#include <Pyramid.h>
#include "Scene.h"



/**
 * Constructeur
 */
Scene::Scene() : TurnTableScene(false)
{
    // créer les objets
    m_Cube = new Cube3Mats();
    m_Pyramid = new Pyramid();

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // couleur du fond
    glClearColor(0.4, 0.4, 0.4, 0.0);

    // paramètres de la vue
    m_CameraAngleAzimut = -15.0;
    m_CameraAngleElevation = 30.0;
    m_CameraDistance = 10.0;
}


/**
 * appelée quand la taille de la vue OpenGL change
 * @param width : largeur en nombre de pixels de la fenêtre
 * @param height : hauteur en nombre de pixels de la fenêtre
 */
void Scene::onSurfaceChanged(int width, int height)
{
    // appeler la méthode de la superclasse
    TurnTableScene::onSurfaceChanged(width, height);

    // matrice de projection (champ de vision)
    mat4::perspective(m_Mat4Projection, Utils::radians(20.0), (float)width / height, 5.0, 20.0);
}


/**
 * dessine les objets de la scène
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Scene::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
{
    // matrice temporaire
    mat4 mat4ModelViewTmp = mat4::create();

    // dessiner le cube
    mat4::translate(mat4ModelViewTmp, mat4ModelView, vec3::fromValues(-1, 0, 0));
    mat4::scale(mat4ModelViewTmp, mat4ModelViewTmp, vec3::fromValues(0.75, 0.75, 0.75));
    m_Cube->onDraw(mat4Projection, mat4ModelViewTmp);

    // dessiner la pyramide
    mat4::translate(mat4ModelViewTmp, mat4ModelView, vec3::fromValues(+1, 0, 0));
    mat4::scale(mat4ModelViewTmp, mat4ModelViewTmp, vec3::fromValues(0.75, 0.75, 0.75));
    m_Pyramid->onDraw(mat4Projection, mat4ModelViewTmp);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_Pyramid;
    delete m_Cube;
}
