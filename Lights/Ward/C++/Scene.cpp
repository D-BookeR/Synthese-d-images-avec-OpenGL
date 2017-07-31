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
    // coordonnées des sources de lumières dans le repère global
    m_LightsPositions.push_back(vec4::fromValues(10.0, 5.0, 0.0, 1.0));
    int lights_count = m_LightsPositions.size();

    // couleurs des lampes
    m_LightsColors.push_back(vec3::fromValues(1.5, 1.5, 1.5));


    // créer le matériau de la théière
    vec3 Kd = vec3::fromValues(0.4, 0.1, 0.0);
    vec3 Ks = vec3::fromValues(0.6, 0.3, 0.1);
    vec2 alpha = vec2::fromValues(0.1, 0.8);
    m_Material = new WardMaterial(lights_count, Kd, Ks, alpha);

    // créer l'objet
    m_Object = new Teapot(m_Material);

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // couleur du fond
    glClearColor(0.4, 0.4, 0.4, 0.0);

    // paramètres de la vue
    m_CameraAngleAzimut = 30.0;
    m_CameraAngleElevation = 50.0;
    m_CameraDistance = 12;
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
 * @param mat4ModelView : matrice de vue
 */
void Scene::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
{
    // fournir la position non transformée des lampes au matériau
    int lights_count = m_LightsPositions.size();
    for (int i=0; i<lights_count; i++) {
        m_Material->setLightPositionColor(i, m_LightsPositions[i], m_LightsColors[i]);
    }

    // dessiner l'objet
    m_Object->onDraw(mat4Projection, mat4ModelView);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_Object;
    delete m_Material;
}
