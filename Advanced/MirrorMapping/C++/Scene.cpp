#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

#include <utils.h>

#include "Scene.h"



/**
 * Constructeur
 */
Scene::Scene() : TurnTableScene(false)
{
    // charger la texture skybox commune aux objets
    m_Texture = new TextureCube("data/textures/Teide");

    // créer les objets de la scène
    m_Skybox = new Skybox(m_Texture);
    m_Teapot = new Teapot(m_Texture);

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // couleur du fond
    glClearColor(0.4, 0.4, 0.4, 0.0);

    // paramètres de la vue
    m_CameraAngleAzimut = 0.0;
    m_CameraAngleElevation = 0.0;
    m_CameraDistance = 1.5;
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
    mat4::perspective(m_Mat4Projection, Utils::radians(30.0), (float)width / height, 0.1, 5.0);
}


/**
 * dessine les objets de la scène
 * @param mat4Projection : matrice de projection
 * @param mat4View : matrice de vue
 */
void Scene::onDraw(mat4& mat4Projection, mat4& mat4View)
{
    // voir onDrawFrame
}


/**
 * Dessine l'image courante
 */
void Scene::onDrawFrame()
{
    // effacer l'écran
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // matrice ModelView pour afficher la skybox
    mat4 mat4ModelViewSkybox = mat4::create();

    // faire pivoter la scène autour de son centre
    mat4::rotateX(mat4ModelViewSkybox, mat4ModelViewSkybox, Utils::radians(m_CameraAngleElevation));
    mat4::rotateY(mat4ModelViewSkybox, mat4ModelViewSkybox, Utils::radians(m_CameraAngleAzimut));

    // dessiner la skybox
    m_Skybox->onDraw(m_Mat4Projection, mat4ModelViewSkybox);


    // matrice ModelView pour afficher la scène
    mat4 mat4ModelViewScene = mat4::create();

    // la théière est décalée par rapport au centre afin qu'on la voie
    mat4::translate(mat4ModelViewScene, mat4ModelViewScene, vec3::fromValues(0.0, 0.0, -m_CameraDistance));

    // appliquer la rotation de la skybox (scène) à la théière
    mat4::multiply(mat4ModelViewScene, mat4ModelViewScene, mat4ModelViewSkybox);

    // dessiner la théière
    m_Teapot->onDraw(m_Mat4Projection, mat4ModelViewScene);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_Teapot;
    delete m_Skybox;
    delete m_Texture;
}
