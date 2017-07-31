#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

#include <utils.h>
#include <Rectangle.h>
#include "Scene.h"



/**
 * Constructeur
 */
Scene::Scene() : TurnTableScene(false)
{
    // créer les rectangles qui représentent les couches à fusionner
    m_Background = new Rectangle("data/textures/hill-meadow-tree-green-2048.jpg", 1.33, 1.0);
    m_Clouds = new Rectangle("data/textures/nuages.png", 1.33, 1.0);
    m_PalmTree = new Rectangle("data/textures/Arbres/treeM044.png", 1.0, 2.0);
    m_SmallTree = new Rectangle("data/textures/Arbres/plantM060.png", 1.0, 1.33);

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);      // laisser voir le dos du rectangle

    // couleur du fond
    glClearColor(0.4, 0.4, 0.4, 0.0);

    // paramètres de la vue
    m_CameraAngleAzimut = 0.0;
    m_CameraAngleElevation = 0.0;
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
    mat4::perspective(m_Mat4Projection, Utils::radians(8.0), (float)width / height, 0.1, 20.0);
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

    // dessiner le fond
    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(0.0,0.0,-1.0));
    m_Background->onDraw(mat4Projection, mat4ModelView);

    // activer le mode blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // dessiner les nuages
    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(-0.2,0.7,-0.5));
    m_Clouds->onDraw(mat4Projection, mat4ModelView);

    // dessiner le palmier
    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(0.2,0.2, 0.0));
    mat4::scale(mat4ModelView, mat4ModelView, vec3::fromValues(0.5,0.5,0.5));
    m_PalmTree->onDraw(mat4Projection, mat4ModelView);

    // dessiner l'arbuste
    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(-0.2,-0.3,0.5));
    mat4::scale(mat4ModelView, mat4ModelView, vec3::fromValues(0.5,0.5,0.5));
    m_SmallTree->onDraw(mat4Projection, mat4ModelView);

    // désactiver le mode blending
    glDisable(GL_BLEND);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_SmallTree;
    delete m_PalmTree;
    delete m_Clouds;
    delete m_Background;
}
