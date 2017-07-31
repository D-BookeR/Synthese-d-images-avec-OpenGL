#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

#include <utils.h>
#include <MeshObjectFromObj.h>

#include "Scene.h"



/**
 * Constructeur
 */
Scene::Scene() : TurnTableScene(false)
{
    // créer les matériaux pour les pommes
    m_LightMaterial1 = new LightMaterial1(vec3::fromValues(0.1,0.9,0.0));
    m_LightMaterial2 = new LightMaterial2(vec3::fromValues(0.1,0.6,1.0), vec3::fromValues(0.5,0.5,0.5), 64);
    m_LightMaterial3 = new LightMaterial3(vec3::fromValues(0.7,0.2,0.1), vec3::fromValues(0.5,0.5,0.5), 64);

    // créer les objets
    m_Apple1 = new MeshObjectFromObj("data/models/Apple","apple.obj", m_LightMaterial1, 0.01);
    m_Apple2 = new MeshObjectFromObj("data/models/Apple","apple.obj", m_LightMaterial2, 0.01);
    m_Apple3 = new MeshObjectFromObj("data/models/Apple","apple.obj", m_LightMaterial3, 0.01);

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // couleur du fond
    glClearColor(0.4, 0.4, 0.4, 0.0);

    // paramètres de la vue
    m_CameraAngleAzimut = 15.0;
    m_CameraAngleElevation = 10.0;
    vec3::set(m_CameraPivot, 0.0, -0.4, 0.0);
    m_CameraDistance = 8.0;
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
    mat4::perspective(m_Mat4Projection, Utils::radians(10.0), (float)width / height, 0.1, 15.0);
}


/**
 * dessine les objets de la scène
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Scene::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
{
    mat4 mat4ModelViewApple = mat4::create();

    // positionner et dessiner le premier objet
    mat4::translate(mat4ModelViewApple, mat4ModelView, vec3::fromValues(-0.5, 0.0, 0.3));
    m_Apple1->onDraw(mat4Projection, mat4ModelViewApple);

    // positionner et dessiner le deuxième objet
    mat4::translate(mat4ModelViewApple, mat4ModelView, vec3::fromValues(+0.5, 0.0, 0.3));
    m_Apple2->onDraw(mat4Projection, mat4ModelViewApple);

    // positionner et dessiner le troisième objet
    mat4::translate(mat4ModelViewApple, mat4ModelView, vec3::fromValues( 0.0, 0.0,-0.5));
    m_Apple3->onDraw(mat4Projection, mat4ModelViewApple);
}


/** supprime tous les objets alloués */
Scene::~Scene()
{
    delete m_Apple3;
    delete m_Apple2;
    delete m_Apple1;
    delete m_LightMaterial1;
    delete m_LightMaterial2;
    delete m_LightMaterial3;
}
