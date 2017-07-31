#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

#include <utils.h>
#include "Scene.h"
#include <MeshObjectFromObj.h>


/**
 * écart entre les deux yeux
 *   négatif pour une visée parallèle (Loreo Pixi 3D Viewer)
 *   positif pour une visée croisée (loucher et se faire mal aux yeux)
 */
const float EyeSeparation = 0.5;


/**
 * Constructeur
 */
Scene::Scene() : TurnTableScene(true)
{
    // créer les objets à dessiner
    m_Ground   = new MeshObjectFromObj("data/models/TerrainSimple", "Terrain.obj", "Terrain.mtl", 2.0);
    m_Lorry    = new MeshObjectFromObj("data/models/Camion", "camion.obj", "camion.mtl", 2.0);
    m_PalmTree = new MeshObjectFromObj("data/models/Palm_Tree", "Palm_Tree.obj", "Palm_Tree.mtl", 2.0);

    // définir une lampe directionnelle
    m_Light0 = new OmniLight();
    m_Light0->setPosition(vec4::fromValues(10, 5, 10, 0));
    m_Light0->setColor(vec3::fromValues(0.9,0.9,0.9));
    addLight(m_Light0);

    // définir une lampe positionnelle
    m_Light1 = new OmniLight();
    m_Light1->setPosition(vec4::fromValues(-10, 5, 5, 1));
    m_Light1->setColor(vec3::fromValues(80,80,80));
    addLight(m_Light1);

    // définir une lampe directionnelle
    m_Light2 = new OmniLight();
    m_Light2->setPosition(vec4::fromValues(-5, 5, -10, 0));
    m_Light2->setColor(vec3::fromValues(1.0,1.0,1.0));
    addLight(m_Light2);

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // couleur du fond
    glClearColor(0.4, 0.5, 0.6, 0.0);

    // paramètres de la vue
    m_CameraAngleAzimut = 60.0;
    m_CameraAngleElevation = 30.0;
    m_CameraDistance = 35;

    m_WindowWidth2 = m_WindowHeight = 0;
}


/**
 * appelée quand la taille de la vue OpenGL change
 * @param width : largeur en nombre de pixels de la fenêtre
 * @param height : hauteur en nombre de pixels de la fenêtre
 */
void Scene::onSurfaceChanged(int width, int height)
{
    // dimensions réduites, deux vues dans la fenêtre
    m_WindowWidth2 = width/2;
    m_WindowHeight = height;

    // appeler la méthode de la superclasse
    TurnTableScene::onSurfaceChanged(m_WindowWidth2, m_WindowHeight);

    // matrice de projection (champ de vision)
    mat4::perspective(m_Mat4Projection, Utils::radians(12.0), (float)m_WindowWidth2 / m_WindowHeight, 1.0, 100.0);
}


/**
 * dessine les objets de la scène
 * @param mat4Projection : matrice de projection
 * @param mat4View : matrice de vue
 */
void Scene::onDraw(mat4& mat4Projection, mat4& mat4View)
{
    // définir le pivot des rotations
    mat4::translate(mat4View, mat4View, m_CameraPivot);

    // dessiner le terrain
    mat4 mat4ModelView = mat4::create();
    mat4::scale(mat4ModelView, mat4View, vec3::fromValues(2.0, 2.0, 2.0));
    m_Ground->onDraw(mat4Projection, mat4ModelView);

    // dessiner les palmiers
    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(2.5,0,-1.0));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(120.0));
    mat4::scale(mat4ModelView, mat4ModelView, vec3::fromValues(0.2, 0.2, 0.2));
    m_PalmTree->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(3.0,0,1.5));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(20.0));
    mat4::scale(mat4ModelView, mat4ModelView, vec3::fromValues(0.2, 0.2, 0.2));
    m_PalmTree->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(-1,0,-2.0));
    mat4::rotate(mat4ModelView, mat4ModelView, Utils::radians(80.0), vec3::fromValues(-1,0,-1));
    mat4::scale(mat4ModelView, mat4ModelView, vec3::fromValues(0.2, 0.2, 0.2));
    m_PalmTree->onDraw(mat4Projection, mat4ModelView);

    // dessiner les camions
    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(1.5,0.5,-1.5));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(-70.0));
    mat4::rotate(mat4ModelView, mat4ModelView, Utils::radians(82.0), vec3::fromValues(0,0,1));
    m_Lorry->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(-2,0,0.5));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(120.0));
    m_Lorry->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(-1,0,-1));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(320.0));
    mat4::scale(mat4ModelView, mat4ModelView, vec3::fromValues(1.5,3.0,0.5));
    m_Lorry->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(1.15,1.0,0.7));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(60.0));
    m_Lorry->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(1,0,1));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(-20.0));
    mat4::scale(mat4ModelView, mat4ModelView, vec3::fromValues(2.0, 2.0, 2.0));
    m_Lorry->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(-1,0,2));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(200.0));
    m_Lorry->onDraw(mat4Projection, mat4ModelView);
}


/**
 * Dessine l'image courante
 */
void Scene::onDrawFrame()
{
    // effacer l'écran
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // construire la matrice caméra
    mat4 mat4View = mat4::create();
    mat4 mat4ViewTmp = mat4::create();

    // éloignement de la scène
    mat4::translate(mat4View, mat4View, vec3::fromValues(0.0, 0.0, -m_CameraDistance));

    // orientation de la scène par rapport à la caméra
    mat4::rotateX(mat4View, mat4View, Utils::radians(m_CameraAngleElevation));
    mat4::rotateY(mat4View, mat4View, Utils::radians(m_CameraAngleAzimut));

    // transformer les lampes
    prepareLights(mat4View);

    // vue gauche
    glViewport(0, 0, m_WindowWidth2, m_WindowHeight);
    mat4::fromTranslation(mat4ViewTmp, vec3::fromValues(-EyeSeparation, 0.0, 0.0));
    mat4::multiply(mat4ViewTmp, mat4ViewTmp, mat4View);

    // dessiner les objets dans cette transformation
    drawDeferredShading(m_Mat4Projection, mat4ViewTmp);

    // dessiner les éclairements des lampes
    addLightings();

    // vue droite
    glViewport(m_WindowWidth2, 0, m_WindowWidth2, m_WindowHeight);
    mat4::fromTranslation(mat4ViewTmp, vec3::fromValues(+EyeSeparation, 0.0, 0.0));
    mat4::multiply(mat4ViewTmp, mat4ViewTmp, mat4View);

    // dessiner les objets dans cette transformation
    drawDeferredShading(m_Mat4Projection, mat4ViewTmp);

    // dessiner les éclairements des lampes
    addLightings();
}


/** supprime tous les objets créés */
Scene::~Scene()
{
    delete m_Light2;
    delete m_Light1;
    delete m_Light0;
    delete m_PalmTree;
    delete m_Lorry;
    delete m_Ground;
}
