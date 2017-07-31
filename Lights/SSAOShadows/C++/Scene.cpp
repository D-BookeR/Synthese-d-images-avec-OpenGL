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
    m_Buildings = new MeshObjectFromObj("data/models/Immeubles", "immeubles.obj", "immeubles.mtl", 0.5);
    m_Ground    = new MeshObjectFromObj("data/models/Immeubles", "sol.obj", "sol.mtl", 0.5);

    // définir une lampe omnidirectionnelle
    m_Light0 = new OmniLight();
    m_Light0->setPosition(vec4::fromValues(5, 5, 0, 1));
    m_Light0->setColor(vec3::fromValues(8.0,8.0,8.0));

    // définir une lampe ambiante avec ombres de type SSAO
    m_Light1 = new SSAOLight(0.05, 1.0, 4.0);
    m_Light1->setColor(vec3::fromValues(0.8,0.8,0.8));

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // couleur du fond
    glClearColor(0.7, 0.8, 0.9, 0.0);

    // paramètres de la vue
    m_CameraAngleAzimut = 190.0;
    m_CameraAngleElevation = 25.0;
    vec3::set(m_CameraPivot, 0.0,-1.0,0.0);
    m_CameraDistance = 24;
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
    // dessiner les immeubles et le sol
    m_Buildings->onDraw(mat4Projection, mat4View);
    m_Ground->onDraw(mat4Projection, mat4View);
}


/**
 * positionne et dessine les shadow maps des lampes
 * @param mat4View : matrice de vue de la lampe
 */
void Scene::prepareLights(mat4& mat4View)
{
    // rendre les lampes fixes par rapport à la scène
    m_Light0->transform(mat4View);

    // les cibles des lampes sont relatives au pivot des rotations
    m_Light0->transformTarget(mat4View);

    // calculer les shadow maps des lampes
    m_Light0->makeShadowMap(this, mat4View);
}


/**
 * Dessine l'image courante
 */
void Scene::onDrawFrame()
{
    // construire la matrice caméra
    mat4 mat4View = getModelView();

    // faire tourner la lampe autour de la scène
    mat4 mat4Light = mat4::create();
    mat4::rotateY(mat4Light, mat4View, Utils::Time*0.5);

    // préparer les lampes (transformation et shadow maps)
    prepareLights(mat4Light);

    // dessiner les objets dans cette transformation
    drawDeferredShading(m_Mat4Projection, mat4View);

    // (debug) dessiner l'un des buffers sur l'écran
    //m_GBuffer->onDraw(GL_COLOR_ATTACHMENT0);
    //m_GBuffer->onDraw(GL_DEPTH_ATTACHMENT);

    // effacer l'écran
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // dessiner l'éclairement de la première lampe
    addLighting(m_Light0, true);

    // rajouter les éclairements des lampes suivantes
    addLighting(m_Light1);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_Light1;
    delete m_Light0;
    delete m_Ground;
    delete m_Buildings;
}
