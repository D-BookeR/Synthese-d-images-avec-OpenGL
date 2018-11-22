#include <GL/glew.h>
#include <GL/gl.h>

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
    m_Ground = new MeshObjectFromObj("data/models/TerrainSimple", "Terrain_2faces.obj", "Terrain.mtl", 5.0);
    m_Lorry  = new MeshObjectFromObj("data/models/Camion", "camion.obj", "camion.mtl", 2.0);

    // définir une lampe spot
    m_SpotLight = new SpotLightDebug(40.0, 30.0);
    m_SpotLight->setPosition(vec4::fromValues(0, 5, 6, 1));
    m_SpotLight->setTarget(vec4::fromValues(0, 0, 1, 1));
    m_SpotLight->setColor(vec3::fromValues(50.0, 50.0, 50.0));

    // définir une lampe positionnelle
    m_OmniLight = new OmniLightDebug();
    m_OmniLight->setPosition(vec4::fromValues(0, 10, 0, 1));
    m_OmniLight->setColor(vec3::fromValues(50.0, 50.0, 50.0));

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // couleur du fond
    glClearColor(0.4, 0.5, 0.6, 0.0);

    // paramètres de la vue
    m_CameraAngleAzimut = 40.0;
    m_CameraAngleElevation = 40.0;
    m_CameraDistance = 35;
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
    mat4::perspective(m_Mat4Projection, Utils::radians(12.0), (float)width / height, 2.0, 100.0);
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
    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(0.0,0.0,1.0));
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
    mat4 mat4View = getModelView();

    // transformer les lampes
    m_SpotLight->transform(mat4View);
    m_OmniLight->transform(mat4View);

    // dessiner les objets dans cette transformation
    drawDeferredShading(m_Mat4Projection, mat4View);

    // dessiner l'éclairement des lampes
    addLighting(m_SpotLight, true);
    addLighting(m_OmniLight);

    // dessiner les lampes
    m_SpotLight->onDraw(m_Mat4Projection, mat4View);
    m_OmniLight->onDraw(m_Mat4Projection, mat4View);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_OmniLight;
    delete m_SpotLight;
    delete m_Lorry;
    delete m_Ground;
}
