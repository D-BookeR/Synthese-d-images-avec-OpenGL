#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

#include <utils.h>
#include <DeferredShadingMaterial.h>
#include "Scene.h"



/**
 * Constructeur
 */
Scene::Scene() : TurnTableScene(true)
{
    // créer le matériau de l'objet
    std::string Kd = "data/models/TerrainHM/terrain_tx.jpg";
    vec3 Ks = vec3::fromValues(0.1, 0.1, 0.1);
    float Ns = 64;
    m_Material = new GroundMaterial("data/models/TerrainHM/terrain_hm.png", 0.3, 0.005, Kd, Ks, Ns);

    // créer l'objet
    m_Ground = new Ground(120, m_Material);

    // définir une lampe directionnelle
    m_Light0 = new OmniLight();
    m_Light0->setPosition(vec4::fromValues(1, 1, 1, 0));
    m_Light0->setColor(vec3::fromValues(1.5,1.5,1.5));
    addLight(m_Light0);

    // définir une lampe directionnelle
    m_Light1 = new OmniLight();
    m_Light1->setPosition(vec4::fromValues(-2, 1, -2, 0));
    m_Light1->setColor(vec3::fromValues(0.9,0.9,0.9));
    addLight(m_Light1);

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // couleur du fond
    glClearColor(0.4, 0.4, 0.4, 0.0);

    // paramètres de la vue
    m_CameraAngleAzimut = 30.0;
    m_CameraAngleElevation = 30.0;
    vec3::set(m_CameraPivot, 0.0, -0.05, 0.0);
    m_CameraDistance = 4.2;
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
    // dessiner l'objet
    m_Ground->onDraw(mat4Projection, mat4ModelView);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_Ground;
    delete m_Light1;
    delete m_Light0;
    delete m_Material;
}
