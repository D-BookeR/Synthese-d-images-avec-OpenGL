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
    // créer la sous-scène Scene1
    m_Scene1 = new Scene1();

    // lui donner sa taille d'affichage hors écran
    int width = 512;
    int height = 512;
    m_Scene1->onSurfaceChanged(width, height);

    // FBO nécessaire pour le dessin hors écran de la scène imbriquée
    m_FBO = new FrameBufferObject(width, height, GL_TEXTURE_2D);

    // créer le matériau de dessin du cube
    m_Material = new FBOMaterial(m_FBO->getColorBuffer());

    // créer le cube sur lequel on va projeter la sous-scène
    m_Cube = new Cube(m_Material);

    // paramètres de la vue
    m_CameraAngleAzimut = 30.0;
    m_CameraAngleElevation = 20.0;
    m_CameraDistance = 18;
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
    mat4::perspective(m_Mat4Projection, Utils::radians(10.0), (float)width / height, 0.1, 20.0);
}


/**
 * dessine les objets de la scène
 * @param mat4Projection : matrice de projection
 * @param mat4View : matrice de vue
 */
void Scene::onDraw(mat4& mat4Projection, mat4& mat4View)
{
    // dessiner la sous-scène dans le FBO
    m_FBO->enable();
    m_Scene1->onDrawFrame();
    m_FBO->disable();

    // mise au point (décommenter la ligne suivante)
    //m_FBO->onDraw(GL_COLOR_ATTACHMENT0); return;

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // couleur du fond
    glClearColor(0.4, 0.4, 0.4, 0.0);

    // effacer l'écran
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // rotation du cube
    mat4 mat4ModelView = mat4::create();
    mat4::rotateY(mat4ModelView, mat4View, Utils::radians(35.0 * Utils::Time));

    // dessiner le cube
    m_Cube->onDraw(mat4Projection, mat4ModelView);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_Cube;
    delete m_Material;
    delete m_FBO;
    delete m_Scene1;
}
