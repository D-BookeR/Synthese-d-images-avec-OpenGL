#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

#include <utils.h>

#include "Scene.h"


/**
 * Constructeur
 */
Scene::Scene() : SceneBase(false)
{
    // créer les objets à dessiner
    m_Pyramid = new Pyramid();
    m_Grid = new GridXZ();

    // définir la transformation M1
    m_matModel1 = mat4::create();
    mat4::translate(m_matModel1, m_matModel1, vec3::fromValues(-1.5, 1.0, 2.0));

    // définir la transformation M2
    m_matModel2 = mat4::create();
    mat4::translate(m_matModel2, m_matModel2, vec3::fromValues(2.0, 1.0, 1.0));
    mat4::rotateY(m_matModel2, m_matModel2, Utils::radians(120));

    // définir la transformation V
    m_matView = mat4::create();
    mat4::translate(m_matView, m_matView, vec3::fromValues(0,0,-10));
    mat4::rotateX(m_matView, m_matView, Utils::radians(32));

    // la transformation P est définie dans onSurfaceChanged
    m_matProjection = mat4::create();

    // couleur du fond
    glClearColor(0.9, 0.9, 0.9, 0.0);
}


/**
 * appelée quand la taille de la vue OpenGL change
 * @param width : largeur en nombre de pixels de la fenêtre
 * @param height : hauteur en nombre de pixels de la fenêtre
 */
void Scene::onSurfaceChanged(int width, int height)
{
    // appeler la méthode de la superclasse
    SceneBase::onSurfaceChanged(width, height);

    // matrice de projection (champ de vision)
    mat4::perspective(m_matProjection, Utils::radians(40.0), (float)width / height, 4.0, 16.0);
}


/**
 * dessine les objets de la scène
 * @param mat4Projection : matrice de projection (égale à m_matP)
 * @param mat4View : matrice de vue (ignorée)
 */
void Scene::onDraw(mat4& mat4Projection, mat4& mat4View)
{
    // dessiner la grille
    m_Grid->onDraw(m_matProjection, m_matView);

    // dessiner la pyramide n°1 (devant et à gauche)
    m_Pyramid->onDraw(m_matProjection, m_matView, m_matModel1);

    // dessiner la pyramide n°2 (à droite)
    m_Pyramid->onDraw(m_matProjection, m_matView, m_matModel2);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_Grid;
    delete m_Pyramid;
}
