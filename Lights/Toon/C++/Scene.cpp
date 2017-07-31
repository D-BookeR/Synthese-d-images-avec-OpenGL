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
    m_LightsPositions.push_back(vec4::fromValues(-10.0, 20.0, 0.0, 1.0));
    int lights_count = m_LightsPositions.size();

    // couleurs des lampes
    m_LightsColors.push_back(vec3::fromValues(1.0, 1.0, 1.0));


    // créer le matériau de la théière
    vec3 Kd = vec3::fromValues(0.6, 0.1, 0.8);
    vec3 Ks = vec3::fromValues(0.8, 0.8, 0.8);
    m_Material = new ToonMaterial(lights_count, Kd, Ks);

    // créer l'objet
    m_Object = new Cow(m_Material);

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // couleur du fond
    glClearColor(0.4, 0.4, 0.4, 0.0);

    // paramètres de la vue
    m_CameraAngleAzimut = 45.0;
    m_CameraAngleElevation = 8.0;
    vec3::set(m_CameraPivot, 0.0, -0.9, 0.0);
    m_CameraDistance = 7;
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
    mat4::perspective(m_Mat4Projection, Utils::radians(18.0), (float)width / height, 0.1, 20.0);
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

    // dessiner les arêtes en traits épais noirs
    glLineWidth(6.0);
    m_Object->onDrawEdges(mat4Projection, mat4ModelView);

    // dessiner l'objet avec un décalage vers l'avant
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-6.0, -6.0);
    m_Object->onDraw(mat4Projection, mat4ModelView);
    glPolygonOffset(0.0, 0.0);
    glDisable(GL_POLYGON_OFFSET_FILL);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_Object;
    delete m_Material;
}
