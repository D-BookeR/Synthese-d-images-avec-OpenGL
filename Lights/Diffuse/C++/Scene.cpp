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
    // coordonnées des sources de lumières dans le repère global
    m_LightsPositions.push_back(vec4::fromValues(-4.0, 6.0, 6.0, 1.0));
    m_LightsPositions.push_back(vec4::fromValues( 5.0, 5.0,-9.0, 1.0));
    int lights_count = m_LightsPositions.size();

    // couleurs des lampes
    m_LightsColors.push_back(vec3::fromValues(1.0, 1.0, 1.0));
    m_LightsColors.push_back(vec3::fromValues(1.0, 1.0, 1.0));

    // créer les matériaux pour les pommes
    m_LambertMaterial = new LambertMaterial(lights_count, vec3::fromValues(0.1,0.6,1.0));
    m_OrenNayarMaterial = new OrenNayarMaterial(lights_count, vec3::fromValues(0.1,1.0,0.3), 0.8);
    m_MinnaertMaterial = new MinnaertMaterial(lights_count, vec3::fromValues(0.7,0.2,0.1), 1.8);

    // créer les objets
    m_LambertApple   = new MeshObjectFromObj("data/models/Apple", "apple.obj", m_LambertMaterial, 0.01);
    m_OrenNayarApple = new MeshObjectFromObj("data/models/Apple", "apple.obj", m_OrenNayarMaterial, 0.01);
    m_MinnaertApple  = new MeshObjectFromObj("data/models/Apple", "apple.obj", m_MinnaertMaterial, 0.01);

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // couleur du fond
    glClearColor(0.4, 0.4, 0.4, 0.0);

    // paramètres de la vue
    m_CameraAngleAzimut = 7.0;
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
    // transformer la position des lampes et la fournir aux matériaux
    vec4 position = vec4::create();
    int lights_count = m_LightsPositions.size();
    for (int i=0; i<lights_count; i++) {
        vec4::transformMat4(position, m_LightsPositions[i], mat4ModelView);
        m_LambertMaterial->setLightPositionColor(i, position, m_LightsColors[i]);
        m_OrenNayarMaterial->setLightPositionColor(i, position, m_LightsColors[i]);
        m_MinnaertMaterial->setLightPositionColor(i, position, m_LightsColors[i]);
    }

    // matrice temporaire pour positionner chaque objet
    mat4 mat4ModelViewApple = mat4::create();

    // positionner et dessiner le premier objet
    mat4::translate(mat4ModelViewApple, mat4ModelView, vec3::fromValues(-0.5, 0.0, 0.3));
    mat4::rotateY(mat4ModelViewApple, mat4ModelViewApple, Utils::radians(Utils::Time * 15.0));
    m_LambertApple->onDraw(mat4Projection, mat4ModelViewApple);

    // positionner et dessiner le deuxième objet
    mat4::translate(mat4ModelViewApple, mat4ModelView, vec3::fromValues(+0.5, 0.0, 0.3));
    mat4::rotateY(mat4ModelViewApple, mat4ModelViewApple, Utils::radians(Utils::Time * 15.0));
    m_OrenNayarApple->onDraw(mat4Projection, mat4ModelViewApple);

    // positionner et dessiner le troisième objet
    mat4::translate(mat4ModelViewApple, mat4ModelView, vec3::fromValues( 0.0, 0.0,-0.5));
    mat4::rotateY(mat4ModelViewApple, mat4ModelViewApple, Utils::radians(Utils::Time * 15.0));
    m_MinnaertApple->onDraw(mat4Projection, mat4ModelViewApple);
}


/** supprime tous les objets alloués */
Scene::~Scene()
{
    delete m_MinnaertApple;
    delete m_OrenNayarApple;
    delete m_LambertApple;
    delete m_LambertMaterial;
    delete m_OrenNayarMaterial;
    delete m_MinnaertMaterial;

}
