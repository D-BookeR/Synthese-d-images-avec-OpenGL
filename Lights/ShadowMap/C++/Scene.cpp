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

    // crée le plan et son matériau
    m_PlaneMaterial = new ShadowMaterial(vec4::fromValues(0.8,0.8,0.8,1.0));
    m_Plane = new Plane(m_PlaneMaterial);

    // crée le cube et son matériau
    m_CubeMaterial = new ShadowMaterial(vec4::fromValues(1.0,0.2,0.6,1.0));
    m_Cube = new Cube(m_CubeMaterial);

    // créer une shadow map
    // défaut n°1 : crénelage
    // modifier la taille de la shadow map, ex: 128, 256, 512, 1024, 2048
    m_ShadowMap = new ShadowMap(256);
    m_ShadowMatrix = mat4::create();

    // définir un point de vue pour la lampe
    // défaut n°2 : champ incorrect, modifier les valeurs d'angle, near et far
    m_MaxAngle = 40.0;
    m_Near = 5.0;
    m_Far = 15.0;

    // point de vue et cible de la lampe en coordonnées scène
    m_Position = vec3::fromValues(5.0, 5.0, -2.0);
    m_Target = vec3::fromValues(0.0, 0.0, 0.0);

    // point de vue en coordonnées caméra (mis à jour auto dans makeShadowMap)
    m_PositionCamera = vec3::create();
    m_TargetCamera = vec3::create();

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // couleur du fond
    glClearColor(0.5, 0.6, 0.7, 0.0);

    // paramètres de la vue
    m_CameraAngleAzimut = -46.0;
    m_CameraAngleElevation = 35.0;
    m_CameraDistance = 15;
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
    mat4::perspective(m_Mat4Projection, Utils::radians(12.0), (float)width / height, 1.0, 100.0);
}


/**
 * dessine les objets de la scène
 * @param mat4Projection : matrice de projection
 * @param mat4View : matrice de vue
 */
void Scene::onDraw(mat4& mat4Projection, mat4& mat4View)
{
    // matrice temporaire
    mat4 mat4ModelView = mat4::create();

    // dessiner le plan
    mat4::scale(mat4ModelView, mat4View, vec3::fromValues(2.0, 2.0, 2.0));
    m_Plane->onDraw(mat4Projection, mat4ModelView);

    // dessiner le cube
    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(0.0, 0.5, 0.0));
    mat4::scale(mat4ModelView, mat4ModelView, vec3::fromValues(0.5, 0.5, 0.5));
    m_Cube->onDraw(mat4Projection, mat4ModelView);
}


/**
 * dessine la scène dans la shadowmap de cette lampe
 * @param mat4ViewCamera : matrice de transformation dans laquelle sont dessinés les objets
 */
void Scene::makeShadowMap(mat4& mat4ViewCamera)
{
    // transformer la position et la cible de la lampe dans le repère caméra
    vec3::transformMat4(m_PositionCamera, m_Position, mat4ViewCamera);
    vec3::transformMat4(m_TargetCamera, m_Target, mat4ViewCamera);

    // construire une matrice de projection à partir de la lampe
    // TODO changer le type de perspective selon la nature de position et target (directions ou positions)
    mat4 mat4LightProjection = mat4::create();
    mat4::perspective(mat4LightProjection, Utils::radians(m_MaxAngle), 1.0, m_Near, m_Far);

    // construire une matrice de vue à partir de la lampe
    mat4 mat4LightView = mat4::create();
    mat4::lookAt(mat4LightView, m_PositionCamera, m_TargetCamera, vec3::fromValues(0,1,0));

    // appliquer la matrice de la scène
    mat4::multiply(mat4LightView, mat4LightView, mat4ViewCamera);

    // calculer la matrice d'ombre
    mat4::invert(m_ShadowMatrix, mat4ViewCamera);
    mat4::multiply(m_ShadowMatrix, mat4LightView, m_ShadowMatrix);
    mat4::multiply(m_ShadowMatrix, mat4LightProjection, m_ShadowMatrix);
    mat4::multiply(m_ShadowMatrix, ShadowMap::c_MatBias, m_ShadowMatrix);

    // passer en mode dessin dans la shadowmap
    m_ShadowMap->enable();

    // avant de voir les solutions aux problèmes, regarder ce que fait la méthode m_ShadowMap->enable
    // elle met en place un décalage de polygones, et peut (mais c'est commenté) dessiner les faces
    // vues de dos seulement. Donc dans la suite, je désactive ça.

    // remettre comme c'est normalement, mais ça crée des défauts
    glDisable(GL_POLYGON_OFFSET_FILL);
    glCullFace(GL_BACK);


    // défaut n°3 : "acné de surface"
    // décommenter les deux lignes suivantes pour le résoudre
    // (c'est fait par défaut dans la méthode enable de la classe ShadowMap)
    // ou bien modifier la valeur de DECALAGE dans le shader de la classe ShadowMaterial, la rendre négative
    //glEnable(GL_POLYGON_OFFSET_FILL);
    //glPolygonOffset(1.0, 1.0);


    // défaut n°4 : "murs volants"
    // modifier la constante DECALAGE dans le shader de ShadowMaterial, la rendre positive
    // ou bien décommenter les deux lignes suivantes pour dessiner seulement le dos des objets, pas leurs faces avant
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_FRONT);


    // dessiner les objets
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    onDraw(mat4LightProjection, mat4LightView);

    // annuler le décalage
    glPolygonOffset(0.0, 0.0);
    glDisable(GL_POLYGON_OFFSET_FILL);

    // remettre l'élimination des faces vues de dos
    glCullFace(GL_BACK);

    // revenir en mode dessin normal
    m_ShadowMap->disable();
}


/**
 * Dessine l'image courante
 */
void Scene::onDrawFrame()
{
    // construire la matrice caméra
    mat4 mat4ViewCamera = getModelView();

    // enlever la shadow map des matériaux sinon ils vont tenter de l'utiliser pendant son dessin
    m_PlaneMaterial->setShadowMap(m_ShadowMatrix, m_PositionCamera, nullptr);
    m_CubeMaterial->setShadowMap(m_ShadowMatrix, m_PositionCamera, nullptr);

    // calculer la shadow map
    makeShadowMap(mat4ViewCamera);

    // mise au point : décommenter la ligne suivante
    //m_ShadowMap->onDraw();/*

    // effacer l'écran
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // fournir la shadow map et la matrice aux matériaux
    m_PlaneMaterial->setShadowMap(m_ShadowMatrix, m_PositionCamera, m_ShadowMap);
    m_CubeMaterial->setShadowMap(m_ShadowMatrix, m_PositionCamera, m_ShadowMap);

    // dessiner la scène
    onDraw(m_Mat4Projection, mat4ViewCamera);

    //*/
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_Plane;
    delete m_PlaneMaterial;
    delete m_Cube;
    delete m_CubeMaterial;
}
