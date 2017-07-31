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
    // créer les deux rectangles
    m_Rectangle1 = new Rectangle1();
    m_Rectangle2 = new Rectangle2();
    m_Rectangle3 = new Rectangle3();

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);      // laisser voir le dos du rectangle

    // couleur du fond
    glClearColor(0.4, 0.4, 0.4, 0.0);

    // paramètres de la vue
    m_CameraAngleAzimut = 15.0;
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
    mat4::perspective(m_Mat4Projection, Utils::radians(10.0), (float)width / height, 0.1, 100.0);
}


/**
 * dessine les objets de la scène
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Scene::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
{
    mat4 mat4ModelViewTmp = mat4::create();

    // dessiner le premier rectangle à gauche
    mat4::translate(mat4ModelViewTmp, mat4ModelView, vec3::fromValues(-1.0, 0.0, 0.0));
    mat4::rotateY(mat4ModelViewTmp, mat4ModelViewTmp, Utils::radians(+40.0*cos(Utils::Time)));
    m_Rectangle1->onDraw(mat4Projection, mat4ModelViewTmp);

    // dessiner le deuxième rectangle au milieu
    m_Rectangle2->onDraw(mat4Projection, mat4ModelView);

    // dessiner le troisième rectangle à droite
    mat4::translate(mat4ModelViewTmp, mat4ModelView, vec3::fromValues(+1.0, 0.0, 0.0));
    mat4::rotateY(mat4ModelViewTmp, mat4ModelViewTmp, Utils::radians(-40.0*cos(Utils::Time)));
    m_Rectangle3->onDraw(mat4Projection, mat4ModelViewTmp);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_Rectangle3;
    delete m_Rectangle2;
    delete m_Rectangle1;
}
