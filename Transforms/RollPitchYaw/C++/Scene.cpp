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
    m_Plane = new MeshObjectFromObj("data/models/Avion", "avion.obj", "avion.mtl", 1.0);
    m_Grid  = new GridXZ(5, 1.0, 1.0, 4.0);
    m_Axes = new AxesXYZ(2.0, 1.0, 9.0);

    // définir une lampe directionnelle
    m_Light0 = new OmniLight();
    m_Light0->setPosition(vec4::fromValues(-1, 1, 0, 0));
    m_Light0->setColor(vec3::fromValues(1.0,1.0,1.0));
    addLight(m_Light0);

    // définir une lampe positionnelle
    m_Light1 = new OmniLight();
    m_Light1->setPosition(vec4::fromValues(5, 5, 5, 1));
    m_Light1->setColor(vec3::fromValues(20,20,20));
    addLight(m_Light1);

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // couleur du fond
    glClearColor(0.60, 0.65, 0.70, 0.0);

    // paramètres de la vue
    m_CameraAngleAzimut = 20.0;
    m_CameraAngleElevation = 30.0;
    m_CameraDistance = 22;
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
    mat4::perspective(m_Mat4Projection, Utils::radians(20.0), (float)width / height, 1.0, 100.0);
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

    // dessiner la grille et le trièdre
    m_Grid->onDraw(mat4Projection, mat4View);
    m_Axes->onDraw(mat4Projection, mat4View);

    /// dessin de l'avion de gauche

    // positionnement de l'avion sur la grille
    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(-3,1,0));

    // rotation de l'avion sur l'axe X (tangage)
    mat4::rotateX(mat4ModelView, mat4ModelView, Utils::radians(20.0 * sin(Utils::Time)));

    // dessiner un trièdre non soumis à l'homothétie
    m_Axes->onDraw(mat4Projection, mat4ModelView);

    // agrandissement de l'avion
    mat4::scale(mat4ModelView, mat4ModelView, vec3::fromValues(5,5,5));

    // dessiner l'avion
    m_Plane->onDraw(mat4Projection, mat4ModelView);


    /// dessin de l'avion de droite

    // positionnement de l'avion sur la grille
    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(+3,1,0));

    // rotation de l'avion sur l'axe Y (lacet)
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(45.0));

    // rotation de l'avion sur l'axe X (tangage)
    mat4::rotateX(mat4ModelView, mat4ModelView, Utils::radians(20.0));

    // rotation de l'avion sur l'axe Z (roulis)
    mat4::rotateZ(mat4ModelView, mat4ModelView, Utils::radians(20.0 * sin(Utils::Time)));

    // dessiner un trièdre non soumis à l'homothétie
    m_Axes->onDraw(mat4Projection, mat4ModelView);

    // agrandissement de l'avion
    mat4::scale(mat4ModelView, mat4ModelView, vec3::fromValues(5,5,5));

    // dessiner l'avion
    m_Plane->onDraw(mat4Projection, mat4ModelView);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_Light1;
    delete m_Light0;
    delete m_Axes;
    delete m_Grid;
    delete m_Plane;
}
