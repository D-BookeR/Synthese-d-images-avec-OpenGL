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
    // créer les types d'objets à dessiner
    m_Ground   = new MeshObjectFromObj("data/models/TerrainSimple", "Terrain.obj", "Terrain.mtl", 4.0);
    m_Bee      = new MeshObjectFromObj("data/models/Bee", "bee.obj", "bee.mtl", 20.0);
    m_Lorry    = new MeshObjectFromObj("data/models/Camion", "camion.obj", "camion.mtl", 2.0);
    m_PalmTree = new MeshObjectFromObj("data/models/Palm_Tree", "Palm_Tree.obj", "Palm_Tree.mtl", 0.4);

    // définir les éléments de la scène et leur hiérarchie
    m_ElementGround    = new SceneElement(m_Ground, nullptr);
    m_ElementLorry     = new SceneElement(m_Lorry,  m_ElementGround);
    m_ElementBee       = new SceneElement(m_Bee, m_ElementLorry);
    m_ElementPalmTree1 = new SceneElement(m_PalmTree, m_ElementGround);
    m_ElementPalmTree2 = new SceneElement(m_PalmTree, m_ElementGround);
    m_ElementPalmTree3 = new SceneElement(m_PalmTree, m_ElementGround);
    m_ElementPalmTree4 = new SceneElement(m_PalmTree, m_ElementGround);
    m_ElementPalmTree5 = new SceneElement(m_PalmTree, m_ElementGround);

    // définir la transformation du palmier n°1 par rapport à son parent
    m_ElementPalmTree1->translate(vec3::fromValues(2.5,0,-1.0));
    m_ElementPalmTree1->rotateY(Utils::radians(120.0));

    // définir la transformation du palmier n°2 par rapport à son parent
    m_ElementPalmTree2->translate(vec3::fromValues(-2.0,0,-3.0));

    // définir la transformation du palmier n°3 par rapport à son parent
    m_ElementPalmTree3->translate(vec3::fromValues(-1.5,0,1.5));
    m_ElementPalmTree3->rotateY(Utils::radians(30.0));

    // définir la transformation du palmier n°4 par rapport à son parent
    m_ElementPalmTree4->translate(vec3::fromValues(0.5,0,0.5));
    m_ElementPalmTree4->rotateY(Utils::radians(270.0));

    // définir la transformation du palmier n°5 par rapport à son parent
    m_ElementPalmTree5->translate(vec3::fromValues(2.5,0,2.0));

    // définir une lampe directionnelle au dessus du terrain
    m_Light0 = new OmniLight();
    m_Light0->setPosition(vec4::fromValues(-1.0, 2.0, 0.0, 0.0));
    m_Light0->setColor(vec3::fromValues(0.8,0.8,0.8));
    m_ElementLight0 = new SceneElement(m_Light0, m_ElementGround);
    addLight(m_Light0);

    // définir une lampe positionnelle placée devant le camion
    m_Light1 = new OmniLight();
    m_Light1->setPosition(vec4::fromValues(0.0, 0.5, 1.5, 1));
    m_Light1->setColor(vec3::fromValues(0.5,0.5,0.5));
    m_ElementLight1 = new SceneElement(m_Light1, m_ElementLorry);
    addLight(m_Light1);

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // couleur du fond
    glClearColor(0.4, 0.5, 0.6, 0.0);

    // paramètres de la vue
    m_CameraAngleAzimut = 30.0;
    m_CameraAngleElevation = 20.0;
    m_CameraDistance = 20;

    /** élément attaché à la caméra, décommenter une seule ligne */
    //ELEMENT_CAMERA = m_ElementGround;
    ELEMENT_CAMERA = m_ElementLorry;
    //ELEMENT_CAMERA = m_ElementBee;
    //ELEMENT_CAMERA = m_ElementPalmTree1;
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
    // définir la transformation du camion par rapport à son parent
    m_ElementLorry->identity();
    m_ElementLorry->translate(vec3::fromValues(-1.0,0,0.5));
    m_ElementLorry->rotateY(Utils::radians(Utils::Time * 30.0 + 180.0));
    m_ElementLorry->translate(vec3::fromValues(-2.5,0,0.0));

    // définir la transformation de l'abeille par rapport à son parent
    m_ElementBee->identity();
    m_ElementBee->rotateX(Utils::radians(30.0)); // son plan de rotation est incliné de 30°
    m_ElementBee->rotateY(Utils::radians(Utils::Time * 70.0 + 60.0));
    m_ElementBee->translate(vec3::fromValues(-1.0,1.0,0.0));

    // dessiner l'élément attaché à la caméra , il dessinera les autres objets
    ELEMENT_CAMERA->onDraw(mat4Projection, mat4View);
}


/**
 * dessin de la scène sur l'écran
 */
void Scene::onDrawFrame()
{
    // effacer l'écran
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // construire la matrice MV
    mat4 mat4ModelView = getModelView();

    // positionner les lampes
    ELEMENT_CAMERA->transform(mat4ModelView);

    // dessiner les shadow map des lampes s'il y en a
    makeShadowMaps(mat4ModelView);

    // dessiner les objets dans le FBO mrt (ou sur l'écran)
    drawDeferredShading(m_Mat4Projection, mat4ModelView);

    // ajouter les éclairements des lampes
    addLightings();
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_ElementLight1;
    delete m_ElementLight0;
    delete m_ElementPalmTree5;
    delete m_ElementPalmTree4;
    delete m_ElementPalmTree3;
    delete m_ElementPalmTree2;
    delete m_ElementPalmTree1;
    delete m_ElementBee;
    delete m_ElementLorry;
    delete m_ElementGround;

    delete m_Light1;
    delete m_Light0;
    delete m_PalmTree;
    delete m_Lorry;
    delete m_Bee;
    delete m_Ground;
}
