#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

#include <utils.h>
#include "Scene.h"



/**
 * Constructeur
 */
Scene::Scene() : TurnTableScene(false)
{
    // créer les rectangles qui représentent les couches à fusionner
    m_Background = new Rectangle("data/textures/hill-meadow-tree-green-2048.jpg", 1.33, 1.0);
    m_Clouds = new Rectangle("data/textures/nuages_pre.png", 1.33, 1.0);
    m_PalmTree = new Rectangle("data/textures/Arbres/treeM044_pre.png", 1.0, 2.0);
    m_SmallTree = new Rectangle("data/textures/Arbres/plantM060_pre.png", 1.0, 1.33);

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

    // FBO pour accumuler la deuxième passe du dessin des objets transparents
    m_FBOlayers = new FrameBufferObject(width, height, GL_TEXTURE_2D, GL_NONE);
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

    // activer le FBO
    m_FBOlayers->enable();

    // effacer le FBO rgb=0, alpha=1
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // activer le mode blending permettant de fusionner d'avant en arrière
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_DST_ALPHA, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);

    // dessiner l'arbuste
    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(-0.2,-0.3,0.5));
    mat4::scale(mat4ModelView, mat4ModelView, vec3::fromValues(0.5,0.5,0.5));
    m_SmallTree->onDraw(mat4Projection, mat4ModelView);

    // dessiner le palmier
    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(0.2,0.2, 0.0));
    mat4::scale(mat4ModelView, mat4ModelView, vec3::fromValues(0.5,0.5,0.5));
    m_PalmTree->onDraw(mat4Projection, mat4ModelView);

    // dessiner les nuages
    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(-0.2,0.7,-0.5));
    m_Clouds->onDraw(mat4Projection, mat4ModelView);

    // dessiner le fond
    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(0.0,0.0,-1.0));
    m_Background->onDraw(mat4Projection, mat4ModelView);

    // fin du dessin dans le FBO
    m_FBOlayers->disable();

    // changer l'équation de mélange
    glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);

    // dessiner le résultat
    m_FBOlayers->onDraw(GL_COLOR_ATTACHMENT0);

    // désactiver le mode blending
    glDisable(GL_BLEND);

    // remettre la couleur d'effacement
    glClearColor(0.4, 0.4, 0.4, 0.0);
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
    delete m_FBOlayers;
}
