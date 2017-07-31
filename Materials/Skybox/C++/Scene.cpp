#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

#include <utils.h>
#include <Skybox.h>
#include <Cow.h>
#include "Scene.h"



/**
 * Constructeur
 */
Scene::Scene() : TurnTableScene(true)
{
    // créer les objets de la scène
    m_Skybox = new Skybox();
    m_Cow = new Cow();

    // définir une lampe ambiante
    m_Light0 = new Light();
    m_Light0->setColor(vec3::fromValues(0.5,0.5,0.5));

    // définir une lampe directionnelle
    m_Light1 = new OmniLight();
    m_Light1->setPosition(vec4::fromValues(-0.2, 1.0, 0.0, 0));
    m_Light1->setColor(vec3::fromValues(1.0,1.0,1.0));

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // couleur du fond
    glClearColor(0.4, 0.4, 0.4, 0.0);

    // paramètres de la vue
    m_CameraAngleAzimut = 140.0;
    m_CameraAngleElevation = 0.0;
    m_CameraDistance = 1.0;
}


void Scene::updateInfos()
{
    std::cout << "\razimut = " << round(m_CameraAngleAzimut * 10)/10.0;
    std::cout << " height = " << round(m_CameraAngleElevation * 10)/10.0;
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
    mat4::perspective(m_Mat4Projection, Utils::radians(60.0), (float)width / height, 0.1, 20.0);
}


/**
 * appelée quand on enfonce le bouton du milieu et qu'on déplace la souris
 * @param dz : nombre de pixels parcourus
 */
void Scene::onZoom(float dz)
{
}


void Scene::onPan(float dx, float dy)
{
}


void Scene::onKeyDown(unsigned char code)
{
}


/**
 * dessine les objets de la scène
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Scene::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
{
    // dessiner la vache en la décalant un peu vers le bas
    mat4::translate(mat4ModelView, mat4ModelView, vec3::fromValues(0.0, -0.4, 0.0));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(270.0));
    m_Cow->onDraw(mat4Projection, mat4ModelView);
}


/**
 * Dessine l'image courante
 */
void Scene::onDrawFrame()
{
    // effacer l'écran
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // construire la matrice MV : la caméra est au centre
    mat4 mat4ModelView = mat4::create();

    // faire pivoter la scène autour de son centre
    mat4::rotateX(mat4ModelView, mat4ModelView, Utils::radians(m_CameraAngleElevation));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(m_CameraAngleAzimut));

    // dessiner la skybox
    m_Skybox->onDraw(m_Mat4Projection, mat4ModelView);

    // réinitialiser la matrice pour tourner autour de l'objet
    mat4::identity(mat4ModelView);
    mat4::translate(mat4ModelView, mat4ModelView, vec3::fromValues(0.0, 0.0, -m_CameraDistance));

    // faire pivoter la scène autour de son centre
    mat4::rotateX(mat4ModelView, mat4ModelView, Utils::radians(m_CameraAngleElevation));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(m_CameraAngleAzimut));

    // positionner les lampes et dessiner leur shadow map s'il y en a
    prepareLights(m_Mat4ModelView);

    // dessiner les objets de la scène
    drawDeferredShading(m_Mat4Projection, mat4ModelView);
    addLighting(m_Light0, true);
    addLighting(m_Light1);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_Light1;
    delete m_Light0;
    delete m_Cow;
    delete m_Skybox;
}
