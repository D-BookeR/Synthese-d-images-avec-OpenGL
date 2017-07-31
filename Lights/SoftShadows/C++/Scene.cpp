#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>

#include <utils.h>
#include <MeshObjectFromObj.h>

#include "Scene.h"



/**
 * Constructeur
 */
Scene::Scene() : TurnTableScene(true)
{
    // créer les objets à dessiner
    m_Ground   = new MeshObjectFromObj("data/models/TerrainSimple", "Terrain.obj", "Terrain.mtl", 4.0);
    m_Lorry    = new MeshObjectFromObj("data/models/Camion", "camion.obj", "camion.mtl", 2.0);
    m_PalmTree = new MeshObjectFromObj("data/models/Palm_Tree", "Palm_Tree.obj", "Palm_Tree.mtl", 0.3);

    // définir une lampe ambiante
    m_Light0 = new Light();
    m_Light0->setColor(vec3::fromValues(0.4,0.4,0.4));

    // définir une lampe spot douce
    // pour PCSS : mettre true, false, gl.FRONT
    // pour PCF : mettre false, true, gl.BACK
    m_Light1 = new SoftSpotLight(20.0, 14.0, 0.1, 512, 16, 25, true, false, GL_FRONT);
    //m_Light1 = new SoftSpotLight(20.0, 14.0, 0.1, 512, 16, 25, false, false, GL_FRONT);
    m_Light1->setPosition(vec4::fromValues(15, 15, 0, 1));
    m_Light1->setTarget(vec4::fromValues(0, 0, 0, 1));
    m_Light1->setColor(vec3::fromValues(400,400,400));

    // définir une lampe spot douce
    // pour PCSS : mettre true, false, gl.FRONT
    // pour PCF : mettre false, true, gl.BACK
    m_Light2 = new SoftSpotLight(20.0, 14.0, 0.1, 512, 4, 60, true, false, GL_FRONT);
    //m_Light2 = new SoftSpotLight(20.0, 14.0, 0.1, 512, 4, 60, false, false, GL_FRONT);
    m_Light2->setPosition(vec4::fromValues(5, 5, 0, 1));
    m_Light2->setTarget(vec4::fromValues(0, 0, 0, 1));
    m_Light2->setColor(vec3::fromValues(600,600,600));

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // couleur du fond
    glClearColor(0.4, 0.5, 0.6, 0.0);

    // paramètres de la vue
    m_CameraAngleAzimut = -40.0;
    m_CameraAngleElevation = 35.0;
    m_CameraDistance = 30;
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

    // dessiner le terrain
    m_Ground->onDraw(mat4Projection, mat4View);

    // dessiner les palmiers
    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(2.5,0,-1.0));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(120.0));
    m_PalmTree->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(3.0,0,1.5));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(20.0));
    m_PalmTree->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(-1.0,0.0,-2.0));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(80.0));
    m_PalmTree->onDraw(mat4Projection, mat4ModelView);

    // dessiner les camions
    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(-2.0,0.0,0.5));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(120.0));
    m_Lorry->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(1.0,0.0,1.0));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(-20.0));
    m_Lorry->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(-1.0,0.0,2.0));
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
    mat4 mat4ViewCamera = mat4::create();

    // la position de la lampe2 est relative à la caméra
    m_Light2->transformPosition(mat4ViewCamera);

    // éloignement de la scène
    mat4::translate(mat4ViewCamera, mat4ViewCamera, vec3::fromValues(0.0, 0.0, -m_CameraDistance));

    // orientation de la scène par rapport à la caméra
    mat4::rotateX(mat4ViewCamera, mat4ViewCamera, Utils::radians(m_CameraAngleElevation));
    mat4::rotateY(mat4ViewCamera, mat4ViewCamera, Utils::radians(m_CameraAngleAzimut));

    // définir le pivot des rotations
    mat4::translate(mat4ViewCamera, mat4ViewCamera, m_CameraPivot);

    // rendre la lampe 1 fixe par rapport à la scène
    m_Light1->transform(mat4ViewCamera);

    // la cible de la lampe 2 est relative à la scène
    m_Light2->transformTarget(mat4ViewCamera);

    // calculer les shadow maps des lampes
    m_Light1->makeShadowMap(this, mat4ViewCamera);
    m_Light2->makeShadowMap(this, mat4ViewCamera);

    // dessiner les objets dans cette transformation
    drawDeferredShading(m_Mat4Projection, mat4ViewCamera);

    // dessiner l'éclairement de la première lampe
    addLighting(m_Light0, true);

    // rajouter les éclairements des lampes suivantes
    addLighting(m_Light1);
    addLighting(m_Light2);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_Light2;
    delete m_Light1;
    delete m_Light0;
    delete m_PalmTree;
    delete m_Lorry;
    delete m_Ground;
}
