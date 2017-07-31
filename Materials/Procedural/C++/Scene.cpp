#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

#include <utils.h>
#include <MeshObjectFromObj.h>

#include "AppleMaterial.h"
#include "GroundMaterial.h"
#include "Scene.h"



/**
 * Constructeur
 */
Scene::Scene() : TurnTableScene(false)
{
    // créer les matériaux
    m_AppleMaterial = new AppleMaterial();
    m_GroundMaterial = new GroundMaterial();

    // créer les objets
    m_Apple  = new MeshObjectFromObj("data/models/Apple","apple.obj", m_AppleMaterial, 0.01);
    m_Ground = new MeshObjectFromObj("data/models/Terrain","Terrain.obj", m_GroundMaterial, 1.0);

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);   // laisser voir le dos des facettes

    // couleur du fond
    glClearColor(0.4, 0.4, 0.4, 0.0);

    // paramètres de la vue
    m_CameraAngleAzimut = 15.0;
    m_CameraAngleElevation = 50.0;
    vec3::set(m_CameraPivot, 0.0, -0.05, 0.0);
    m_CameraDistance = 9.5;
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
    // dessiner le terrain
    m_Ground->onDraw(mat4Projection, mat4ModelView);

    // mettre la pomme au dessus
    mat4 mat4ModelViewApple = mat4::create();
    mat4::translate(mat4ModelViewApple, mat4ModelView, vec3::fromValues(0.0, 0.1, 0.2));
    m_Apple->onDraw(mat4Projection, mat4ModelViewApple);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_Apple;
    delete m_Ground;
    delete m_AppleMaterial;
    delete m_GroundMaterial;
}
