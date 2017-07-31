#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

#include <utils.h>
#include <MeshObjectFromObj.h>

#include "Scene.h"



/**
 * Constructeur
 */
Scene::Scene() : TurnTableScene(false)
{
    // créer le matériau pour les pommes
    m_Texture = new Texture2D("data/models/Apple/skin.jpg", GL_LINEAR, GL_CLAMP_TO_EDGE);
    m_Material = new TextureMaterial(m_Texture);

    // créer les objets
    m_Apple = new MeshObjectFromObj("data/models/Apple", "apple.obj", m_Material, 0.01);

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // couleur du fond
    glClearColor(0.4, 0.4, 0.4, 0.0);

    // paramètres de la vue
    m_CameraAngleAzimut = 15.0;
    m_CameraAngleElevation = 10.0;
    vec3::set(m_CameraPivot, 0.0, -0.4, 0.0);
    m_CameraDistance = 8.0;
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
    mat4::perspective(m_Mat4Projection, Utils::radians(10.0), (float)width / height, 0.1, 15.0);
}


/**
 * dessine les objets de la scène
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Scene::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
{
    mat4 mat4ModelViewApple = mat4::create();

    mat4::translate(mat4ModelViewApple, mat4ModelView, vec3::fromValues(-0.5, 0.0, 0.3));
    m_Apple->onDraw(mat4Projection, mat4ModelViewApple);

    mat4::translate(mat4ModelViewApple, mat4ModelView, vec3::fromValues(+0.5, 0.0, 0.3));
    mat4::rotateY(mat4ModelViewApple, mat4ModelViewApple, Utils::radians(160.0));
    m_Apple->onDraw(mat4Projection, mat4ModelViewApple);

    mat4::translate(mat4ModelViewApple, mat4ModelView, vec3::fromValues( 0.0, 0.0,-0.5));
    mat4::rotateY(mat4ModelViewApple, mat4ModelViewApple, Utils::radians(90.0));
    m_Apple->onDraw(mat4Projection, mat4ModelViewApple);
}


/** supprime tous les objets alloués */
Scene::~Scene()
{
    delete m_Apple;
    delete m_Material;
    delete m_Texture;
}
