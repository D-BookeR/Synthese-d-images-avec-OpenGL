#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <vector>

#include <utils.h>
#include <DeferredShadingMaterial.h>
#include "Scene.h"



/**
 * Constructeur
 */
Scene::Scene() : TurnTableScene(true)
{
    // créer le matériau de l'objet
    vec4 Kd = vec4::fromValues(0.4, 0.8, 0.1, 1.0);
    vec3 Ks = vec3::fromValues(0.9, 0.9, 0.9);
    float Ns = 128;
    m_Material = new DeferredShadingMaterial(Kd, Ks, Ns);

    // définir une lampe directionnelle
    m_Light0 = new OmniLight();
    m_Light0->setPosition(vec4::fromValues(2, 3, 2, 0));
    m_Light0->setColor(vec3::fromValues(0.8,0.8,0.8));
    addLight(m_Light0);

    // définir une lampe positionnelle
    m_Light1 = new OmniLight();
    m_Light1->setPosition(vec4::fromValues(-10, 5, 5, 1));
    m_Light1->setColor(vec3::fromValues(70,70,70));
    addLight(m_Light1);

    // définir une lampe directionnelle
    m_Light2 = new OmniLight();
    m_Light2->setPosition(vec4::fromValues(-2, 1, -2, 0));
    m_Light2->setColor(vec3::fromValues(0.4,0.4,0.4));
    addLight(m_Light2);

    // créer le contour à faire balayer
    std::vector<vec2> borderXY;
    borderXY.push_back(vec2::fromValues(-1.0,  0.0));
    borderXY.push_back(vec2::fromValues(-0.8,  0.1));
    borderXY.push_back(vec2::fromValues(-0.5,  0.0));
    borderXY.push_back(vec2::fromValues(-0.4,  0.2));
    borderXY.push_back(vec2::fromValues(-0.3,  0.2));
    borderXY.push_back(vec2::fromValues( 0.0,  0.1));
    borderXY.push_back(vec2::fromValues( 0.2,  0.1));
    borderXY.push_back(vec2::fromValues(+0.5,  0.2));
    borderXY.push_back(vec2::fromValues(+0.8,  0.1));
    borderXY.push_back(vec2::fromValues(+1.0,  0.0));

    // créer la trajectoire de balayage
    std::vector<vec2> borderZY;
    borderZY.push_back(vec2::fromValues(-1.5,  0.0));
    borderZY.push_back(vec2::fromValues(-1.3,  0.0));
    borderZY.push_back(vec2::fromValues(-1.0, -0.1));
    borderZY.push_back(vec2::fromValues(-0.5, -0.2));
    borderZY.push_back(vec2::fromValues( 0.0, -0.1));
    borderZY.push_back(vec2::fromValues(+0.5, -0.2));
    borderZY.push_back(vec2::fromValues(+1.0,  0.0));
    borderZY.push_back(vec2::fromValues(+1.2,  0.1));
    borderZY.push_back(vec2::fromValues(+1.5,  0.0));

    // créer l'objet
    m_Object = new PathExtrusion(borderXY, borderZY);

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // couleur du fond
    glClearColor(0.4, 0.4, 0.4, 0.0);

    // paramètres de la vue
    m_CameraAngleAzimut = 135.0;
    m_CameraAngleElevation = 30.0;
    m_CameraDistance = 10;

    // modes de dessin
    m_MeshStructureDraw = new MeshStructureDraw(m_Object->getMesh(), m_Material, nullptr, nullptr, nullptr, 0.2);
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
    mat4::perspective(m_Mat4Projection, Utils::radians(15.0), (float)width / height, 0.1, 20.0);
}


/**
 * appelée quand on appuie sur une touche du clavier
 * @param code : touche enfoncée
 */
void Scene::onKeyDown(unsigned char code)
{
    switch (code) {
    case 'T':
        m_MeshStructureDraw->nextTrianglesMode();
        break;
    case 'Y':
        m_MeshStructureDraw->nextEdgesMode();
        break;
    case 'U':
        // NB: les normales des faces ne sont pas calculées, donc elles n'apparaitront pas, void PathExtrusion.cpp
        m_MeshStructureDraw->nextNormalsMode();
        break;
    default:
        // appeler la méthode de la superclasse
        TurnTableScene::onKeyDown(code);
    }
}


/**
 * dessine les objets de la scène
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Scene::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
{
    // dessiner l'objet selon les modes demandés
    m_MeshStructureDraw->onDraw(mat4Projection, mat4ModelView);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_MeshStructureDraw;
    delete m_Object;
    delete m_Light2;
    delete m_Light1;
    delete m_Light0;
    delete m_Material;
}