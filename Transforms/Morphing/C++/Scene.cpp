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
    m_Ground   = new MeshObjectFromObj("data/models/TerrainSimple", "Terrain.obj", "Terrain.mtl", 3.0);
    m_DeadTree = new MeshObjectFromObj("data/models/DeadTrees", "deadtree1.obj", "deadtree1.mtl", 3.0);
    m_Pumpkin1 = new Pumpkin();
    m_Pumpkin2 = new Pumpkin();
    m_Pumpkin3 = new Pumpkin();

    // définir les éléments de la scène et leur hiérarchie
    m_GroundElement = new SceneElement(m_Ground, nullptr);
    m_DeadTreeElement1 = new SceneElement(m_DeadTree, m_GroundElement);
    m_DeadTreeElement2 = new SceneElement(m_DeadTree, m_GroundElement);
    m_DeadTreeElement3 = new SceneElement(m_DeadTree, m_GroundElement);

    // arbre n°1
    m_DeadTreeElement1->translate(vec3::fromValues(2.5,0,-1.0));
    m_DeadTreeElement1->rotateY(Utils::radians(120.0));

    // arbre n°2
    m_DeadTreeElement2->translate(vec3::fromValues(1.0,0,1.5));
    m_DeadTreeElement2->rotateY(Utils::radians(20.0));

    // arbre n°3
    m_DeadTreeElement3->translate(vec3::fromValues(-1.0,0.0,-2.0));
    m_DeadTreeElement3->rotateY(Utils::radians(80.0));

    // citrouille n°1
    m_PumpkinElement1 = new SceneElement(m_Pumpkin1, m_GroundElement);
    m_PumpkinElement1->translate(vec3::fromValues(-2.0,0.0,0.5));
    m_PumpkinElement1->rotateY(Utils::radians(60.0));

    // définir une lampe spot à l'intérieur de la citrouille n°1
    m_PumpkinSpotLight1 = new SpotLight(90.0, 75.0, 512, 0.2, 7.0);
    m_PumpkinSpotLight1->setColor(vec3::fromValues(10,7,5));
    m_PumpkinSpotLight1->setPosition(vec4::fromValues(0.0,0.3,0.0, 1));
    m_PumpkinSpotLight1->setTarget(vec4::fromValues(0.0,0.3,1.0, 1));
    addLight(m_PumpkinSpotLight1);
    m_PumpkinSpotLightElement1 = new SceneElement(m_PumpkinSpotLight1, m_PumpkinElement1);

    // définir une lampe omni à l'intérieur de la citrouille n°1
    m_PumpkinOmniLight1 = new OmniLight();
    m_PumpkinOmniLight1->setColor(vec3::fromValues(0.05,0.025,0.0));
    m_PumpkinOmniLight1->setPosition(vec4::fromValues(0.0,0.3,0.0, 1));
    addLight(m_PumpkinOmniLight1);
    m_PumpkinOmniLightElement1 = new SceneElement(m_PumpkinOmniLight1, m_PumpkinElement1);

    // citrouille n°2
    m_PumpkinElement2 = new SceneElement(m_Pumpkin2, m_GroundElement);
    m_PumpkinElement2->translate(vec3::fromValues(-1.0,0.0,2.0));
    m_PumpkinElement2->rotateY(Utils::radians(160.0));

    // définir une lampe spot à l'intérieur de la citrouille n°2
    m_PumpkinSpotLight2 = new SpotLight(90.0, 75.0, 512, 0.2, 7.0);
    m_PumpkinSpotLight2->setColor(vec3::fromValues(10,7,5));
    m_PumpkinSpotLight2->setPosition(vec4::fromValues(0.0,0.3,0.0, 1));
    m_PumpkinSpotLight2->setTarget(vec4::fromValues(0.0,0.3,1.0, 1));
    addLight(m_PumpkinSpotLight2);
    m_PumpkinSpotLightElement2 = new SceneElement(m_PumpkinSpotLight2, m_PumpkinElement2);

    // définir une lampe omni à l'intérieur de la citrouille n°2
    m_PumpkinOmniLight2 = new OmniLight();
    m_PumpkinOmniLight2->setColor(vec3::fromValues(0.05,0.025,0.0));
    m_PumpkinOmniLight2->setPosition(vec4::fromValues(0.0,0.3,0.0, 1));
    addLight(m_PumpkinOmniLight2);
    m_PumpkinOmniLightElement2 = new SceneElement(m_PumpkinOmniLight2, m_PumpkinElement2);

    // citrouille n°3
    m_PumpkinElement3 = new SceneElement(m_Pumpkin3, m_GroundElement);
    m_PumpkinElement3->translate(vec3::fromValues(1.0,0.0,-1.0));
    m_PumpkinElement3->rotateY(Utils::radians(-50.0));

    // définir une lampe spot à l'intérieur de la citrouille n°3
    m_PumpkinSpotLight3 = new SpotLight(90.0, 75.0, 512, 0.2, 7.0);
    m_PumpkinSpotLight3->setColor(vec3::fromValues(10,7,5));
    m_PumpkinSpotLight3->setPosition(vec4::fromValues(0.0,0.3,0.0, 1));
    m_PumpkinSpotLight3->setTarget(vec4::fromValues(0.0,0.3,1.0, 1));
    addLight(m_PumpkinSpotLight3);
    m_PumpkinSpotLightElement3 = new SceneElement(m_PumpkinSpotLight3, m_PumpkinElement3);

    // définir une lampe omni à l'intérieur de la citrouille n°3
    m_PumpkinOmniLight3 = new OmniLight();
    m_PumpkinOmniLight3->setColor(vec3::fromValues(0.05,0.025,0.0));
    m_PumpkinOmniLight3->setPosition(vec4::fromValues(0.0,0.3,0.0, 1));
    addLight(m_PumpkinOmniLight3);
    m_PumpkinOmniLightElement3 = new SceneElement(m_PumpkinOmniLight3, m_PumpkinElement3);

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // couleur du fond
    glClearColor(0.1, 0.2, 0.3, 0.0);

    // paramètres de la vue
    m_CameraAngleAzimut = 20.0;
    m_CameraAngleElevation = 15.0;
    m_CameraDistance = 20;
    vec3::set(m_CameraPivot, -0.1, -0.7, 0.3);
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
    // modifier les citrouilles
    m_Pumpkin1->setMorphCoef(pow(0.5*sin(Utils::Time * 10.0)+0.5, 2.5));
    m_Pumpkin2->setMorphCoef(0.5*sin(Utils::Time * 12.0)+0.5);
    m_Pumpkin3->setMorphCoef(pow(0.5*sin(Utils::Time * 15.0)+0.5, 0.5));

    // dessiner le terrain, les palmiers, les citrouilles et les lampes
    m_GroundElement->onDraw(mat4Projection, mat4View);
}


/**
 * dessine les objets de la scène
 */
void Scene::onDrawFrame()
{
    // effacer l'écran
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // construire la matrice caméra
    mat4 mat4ViewCamera = getModelView();

    // positionner les lampes
    m_GroundElement->transform(mat4ViewCamera);

    // préparer les shadow maps (s'il y en a)
    makeShadowMaps(mat4ViewCamera);

    // dessiner les objets dans cette transformation
    drawDeferredShading(m_Mat4Projection, mat4ViewCamera);

    // rajouter les éclairements des lampes
    addLightings();
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_PumpkinOmniLightElement3;
    delete m_PumpkinSpotLightElement3;
    delete m_PumpkinElement3;
    delete m_PumpkinOmniLightElement2;
    delete m_PumpkinSpotLightElement2;
    delete m_PumpkinElement2;
    delete m_PumpkinOmniLightElement1;
    delete m_PumpkinSpotLightElement1;
    delete m_PumpkinElement1;
    delete m_DeadTreeElement3;
    delete m_DeadTreeElement2;
    delete m_DeadTreeElement1;
    delete m_GroundElement;
    delete m_PumpkinOmniLight3;
    delete m_PumpkinSpotLight3;
    delete m_PumpkinOmniLight2;
    delete m_PumpkinSpotLight2;
    delete m_PumpkinOmniLight1;
    delete m_PumpkinSpotLight1;
    delete m_Pumpkin3;
    delete m_Pumpkin2;
    delete m_Pumpkin1;
    delete m_DeadTree;
    delete m_Ground;
}
