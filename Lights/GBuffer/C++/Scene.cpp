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
Scene::Scene() : TurnTableScene(false)
{
    // créer les objets à dessiner
    m_Ground = new MeshObjectFromObj("data/models/TerrainSimple", "Terrain.obj", "Terrain.mtl", 4.0);
    m_Lorry  = new MeshObjectFromObj("data/models/Camion", "camion.obj", "camion.mtl", 4.0);
    m_Apple  = new MeshObjectFromObj("data/models/Apple", "apple.obj", "apple.mtl", 0.02);

    // définir une lampe omnidirectionnelle
    m_Light0 = new OmniLight();
    m_Light0->setPosition(vec4::fromValues(5, 5, 0, 1));
    m_Light0->setColor(vec3::fromValues(20.0,20.0,20.0));
    addLight(m_Light0);

    // définir une lampe ambiante
    m_Light1 = new Light();
    m_Light1->setColor(vec3::fromValues(0.4,0.4,0.4));
    addLight(m_Light1);

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // couleur du fond
    glClearColor(0.7, 0.8, 0.9, 0.0);

    // paramètres de la vue
    m_CameraAngleAzimut = 30.0;
    m_CameraAngleElevation = 15.0;
    vec3::set(m_CameraPivot, 0.0,-0.7,0.0);
    m_CameraDistance = 22;

    // taille des sous-fenêtres : 1/3 et 1/2 de la fenêtre principale
    m_WindowWidth3 = 0;
    m_WindowHeight2 = 0;
}


/**
 * appelée quand la taille de la vue OpenGL change
 * @param width : largeur en nombre de pixels de la fenêtre
 * @param height : hauteur en nombre de pixels de la fenêtre
 */
void Scene::onSurfaceChanged(int width, int height)
{
    // taille des sous-fenêtres : 1/3 et 1/2 de la fenêtre principale
    m_WindowWidth3  = width/3.0;
    m_WindowHeight2 = height/2.0;

    // matrice de projection (champ de vision)
    mat4::perspective(m_Mat4Projection, Utils::radians(12.0), (float)m_WindowWidth3 / m_WindowHeight2, 15.0, 30.0);

    // créer un FBO pour dessiner hors écran avec plusieurs buffers pour stocker toutes les informations nécessaires
    if (m_GBuffer != nullptr) delete m_GBuffer;
    m_GBuffer = new FrameBufferObject(m_WindowWidth3, m_WindowHeight2, GL_TEXTURE_2D, GL_TEXTURE_2D, 3);
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

    // dessiner le camion
    mat4::rotateY(mat4ModelView, mat4View, Utils::radians(20.0));
    m_Lorry->onDraw(mat4Projection, mat4ModelView);

    // dessiner la pomme
    mat4::translate(mat4ModelView, mat4ModelView, vec3::fromValues(0.0,1.0,-0.2));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(120.0));
    m_Apple->onDraw(mat4Projection, mat4ModelView);
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
    mat4::rotateY(mat4Light, mat4View, Utils::Time*0.5+3.0);

    // préparer les lampes (transformation et shadow maps)
    prepareLights(mat4Light);

    // rediriger les dessins vers le FBO, attention tout doit être dessiné avec le matériau DeferredShadingMaterial
    m_GBuffer->enable();

    // effacer l'écran
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // dessiner les objets de la scène
    onDraw(m_Mat4Projection, mat4View);

    // revenir au dessin sur l'écran
    m_GBuffer->disable();

    // effacer l'écran
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // dessiner le diffuse buffer dans le coin haut gauche
    glViewport(0, m_WindowHeight2, m_WindowWidth3, m_WindowHeight2);
    m_GBuffer->onDraw(GL_COLOR_ATTACHMENT0);

    // dessiner le specular buffer au milieu haut
    glViewport(m_WindowWidth3, m_WindowHeight2, m_WindowWidth3, m_WindowHeight2);
    m_GBuffer->onDraw(GL_COLOR_ATTACHMENT1);

    // dessiner le position buffer dans le coin bas gauche
    glViewport(0, 0, m_WindowWidth3, m_WindowHeight2);
    m_GBuffer->onDraw(GL_COLOR_ATTACHMENT2);

    // dessiner le normal buffer au milieu bas
    glViewport(m_WindowWidth3, 0, m_WindowWidth3, m_WindowHeight2);
    m_GBuffer->onDraw(GL_COLOR_ATTACHMENT3);

    // dessiner le depth buffer dans le coin haut droit
    glViewport(m_WindowWidth3*2, m_WindowHeight2, m_WindowWidth3, m_WindowHeight2);
    m_GBuffer->onDraw(GL_DEPTH_ATTACHMENT);

    // dessiner dans le coin bas droite
    glViewport(m_WindowWidth3*2, 0, m_WindowWidth3, m_WindowHeight2);

    // dessiner l'éclairement de la première lampe
    m_Light0->process(m_GBuffer);

    // passer en mode blending pour additionner les éclairements
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    // rajouter les éclairements des lampes suivantes
    m_Light1->process(m_GBuffer);

    // revenir en mode normal
    glDisable(GL_BLEND);

    debugGLFatal("Scene::onDrawFrame");
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_Light1;
    delete m_Light0;
    delete m_Apple;
    delete m_Lorry;
    delete m_Ground;
}
