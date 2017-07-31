#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

#include <utils.h>
#include <DeferredShadingMaterial.h>
#include <MeshObjectFromObj.h>

#include "Scene.h"



/**
 * Constructeur
 */
Scene::Scene() : TurnTableScene(true)
{
    // définir une lampe directionnelle
    m_Light0 = new OmniLight();
    m_Light0->setPosition(vec4::fromValues(1, 1, -1, 0));
    m_Light0->setColor(vec3::fromValues(1.0,1.0,1.0));
    addLight(m_Light0);

    // définir une lampe positionnelle
    m_Light1 = new OmniLight();
    m_Light1->setPosition(vec4::fromValues(-10, 5, 5, 1));
    m_Light1->setColor(vec3::fromValues(150,150,150));
    addLight(m_Light1);

    // créer les objets à dessiner
    m_Cow = new Cow();
    m_Grass = new MeshObjectFromObj("data/models/TerrainSimple", "Terrain.obj", "TerrainHerbe.mtl", 3.0);

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // couleur du fond
    glClearColor(0.4, 0.4, 0.4, 0.0);

    // paramètres de la vue
    m_CameraAngleAzimut = 50.0;
    m_CameraAngleElevation = 10.0;
    vec3::set(m_CameraPivot, 0.0, -0.9, 0.0);
    m_CameraDistance = 7;
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
    mat4::perspective(m_Mat4Projection, Utils::radians(18.0), (float)width / height, 0.1, 20.0);
}


/**
 * dessine les objets de la scène
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Scene::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
{
    // dessiner les objets
    m_Grass->onDraw(mat4Projection, mat4ModelView);
    m_Cow->onDraw(mat4Projection, mat4ModelView);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_Light1;
    delete m_Light0;
    delete m_Grass;
    delete m_Cow;
}
