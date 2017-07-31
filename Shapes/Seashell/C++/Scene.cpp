#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

#include <utils.h>
#include <DeferredShadingMaterial.h>
#include "Scene.h"



/**
 * Constructeur
 */
Scene::Scene() : TrackBallScene(true)
{
    // créer le matériau de l'objet
    vec3 Ks = vec3::fromValues(0.9, 0.9, 0.9);
    float Ns = 128;
    m_Material = new DeferredShadingMaterial(Ks, Ns);

    // définir une lampe directionnelle
    m_Light0 = new OmniLight();
    m_Light0->setPosition(vec4::fromValues(2, 3, 2, 0));
    m_Light0->setColor(vec3::fromValues(0.8,0.8,0.8));
    addLight(m_Light0);

    // définir une lampe positionnelle
    m_Light1 = new OmniLight();
    m_Light1->setPosition(vec4::fromValues(-10, 5, 5, 1));
    m_Light1->setColor(vec3::fromValues(100,100,100));
    addLight(m_Light1);

    // définir une lampe directionnelle
    m_Light2 = new OmniLight();
    m_Light2->setPosition(vec4::fromValues(-2, 1, -2, 0));
    m_Light2->setColor(vec3::fromValues(0.4,0.4,0.4));
    addLight(m_Light2);

    // créer l'objet
    m_Object = new Seashell(4, 24);

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // couleur du fond
    glClearColor(0.4, 0.4, 0.4, 0.0);

    // paramètres de la vue
    m_CameraDistance = 18;
    quat::setAxisAngle(m_QuatP1P2, vec3::fromValues(1,0,0), Utils::radians(30.0));
    quat::multiply(m_QuatOrientation, m_QuatP1P2, m_QuatOrientation);

    // modes de dessin
    m_MeshStructureDraw = new MeshStructureDraw(m_Object->getMesh(), m_Material, nullptr, nullptr, nullptr, 0.25);
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
    mat4::perspective(m_Mat4Projection, Utils::radians(15.0), (float)width / height, 1.0, 100.0);
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
        m_MeshStructureDraw->nextNormalsMode();
        break;
    default:
        // appeler la méthode de la superclasse
        TrackBallScene::onKeyDown(code);
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
