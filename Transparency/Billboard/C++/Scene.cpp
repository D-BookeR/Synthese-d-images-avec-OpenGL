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
    // crée les objets à dessiner
    m_Ground = new Ground(2.0);

    // créer un billboard
    m_TxTree = new Texture360("data/textures/Billboards/bb_arbre2", 8);
    m_Tree = new Billboard(m_TxTree);

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);      // laisser voir le dos des billboards

    // formule de blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // couleur du fond
    glClearColor(0.6, 0.7, 1.0, 0.0);

    // paramètres de la vue
    m_CameraAngleAzimut = 40.0;
    m_CameraAngleElevation = 11.0;
    m_CameraDistance = 6;
    vec3::set(m_CameraPivot, 0.0, -0.4, 0.0);
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
    // dessiner le terrain
    m_Ground->onDraw(mat4Projection, mat4View);

    // dessiner l'arbre
    m_Tree->onDraw(mat4Projection, mat4View);

    // variante pour dessiner (commenter la ligne précédente)
    //m_Tree->onDraw2(mat4Projection, mat4View);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_Ground;
    delete m_Tree;
    delete m_TxTree;
}
