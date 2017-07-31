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
    // crée les objets à dessiner
    m_Ground   = new MeshObjectFromObj("data/models/TerrainSimple", "Terrain.obj", "Terrain.mtl", 3.0);
    m_DeadTree = new MeshObjectFromObj("data/models/DeadTrees", "deadtree1.obj", "deadtree1.mtl", 3.0);

    // créer le matériau du rectangle
    m_Texture = new Texture2D("data/textures/Multiples/feu.png");
    m_Material = new MultipleTextureMaterial(m_Texture, 16, 4);

    // créer un billboard à partir de la texture du feu
    m_Fire = new Billboard(m_Material, 0.5, 1.0);

    // définir une lampe positionnelle
    m_Light0 = new OmniLight();
    m_Light0->setPosition(vec4::fromValues(0.0, 0.6, 0.0, 1.0));
    m_Light0->setColor(vec3::fromValues(0.5,0.5,0.5));
    addLight(m_Light0);

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);      // laisser voir le dos des billboards

    // formule de blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // couleur du fond
    glClearColor(0.3, 0.4, 0.5, 0.0);

    // paramètres de la vue
    m_CameraAngleAzimut = 40.0;
    m_CameraAngleElevation = 0.5;
    m_CameraDistance = 10;
    vec3::set(m_CameraPivot, -0.2, -0.6, -0.2);
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
    mat4::perspective(m_Mat4Projection, Utils::radians(10.0), (float)width / height, 0.1, 40.0);
}


/**
 * dessine les objets de la scène
 * @param mat4Projection : matrice de projection
 * @param mat4View : matrice de vue
 */
void Scene::onDraw(mat4& mat4Projection, mat4& mat4View)
{
    // modifier l'intensité de la lampe d'une manière irrégulière
    float k = 0.1*sin(Utils::Time*23.0)*sin(Utils::Time*17.0)+0.7;
    m_Light0->setColor(vec3::fromValues(k,k,k));

    // transformation des objets
    mat4 mat4ModelView = mat4::create();

    // dessiner le terrain
    m_Ground->onDraw(mat4Projection, mat4View);

    // dessiner l'arbre
    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(1.5, 0.1, -1.0));
    m_DeadTree->onDraw(mat4Projection, mat4ModelView);
}


/**
 * dessin de la scène sur l'écran
 */
void Scene::onDrawFrame()
{
    // appeler la méthode de la superclasse, elle dessine tout
    TurnTableScene::onDrawFrame();

    // construire la matrice MV
    mat4 mat4ModelView = getModelView();

    // dessiner le feu par dessus le tout
    m_Fire->onDraw(m_Mat4Projection, mat4ModelView);
}

/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_Light0;
    delete m_Fire;
    delete m_Material;
    delete m_Texture;
    delete m_Ground;
}
