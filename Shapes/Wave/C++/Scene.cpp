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
Scene::Scene() : TrackBallScene(true)
{
    // créer le matériau de l'objet
    vec4 Kd = vec4::fromValues(0.2, 0.4, 1.0, 1.0);
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
    m_Light1->setColor(vec3::fromValues(150,150,150));
    addLight(m_Light1);

    // définir une lampe directionnelle
    m_Light2 = new OmniLight();
    m_Light2->setPosition(vec4::fromValues(-2, 1, -2, 0));
    m_Light2->setColor(vec3::fromValues(0.4,0.4,0.4));
    addLight(m_Light2);

    // créer le contour de la fonction
    const int segments_count = 96;
    std::vector<vec2> border;
    for (int segment=segments_count; segment>=0; segment--) {
        // aller de l'extérieur vers l'intérieur
        float r = 40.0 * segment/segments_count;
        // calculer y = f(r)
        float y = -5.0;
        if (segment > 0) {
            y = -5.0 * sin(r) / (r*r);
        }
        r = r / 10.0;
        border.push_back(vec2::fromValues(r,y));
    }

    // créer l'objet
    m_Object = new Wave(border, 48, segments_count);

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // couleur du fond
    glClearColor(0.4, 0.4, 0.4, 0.0);

    // paramètres de la vue
    m_CameraDistance = 10;
    quat::setAxisAngle(m_QuatP1P2, vec3::fromValues(1,0,0), Utils::radians(30.0));
    quat::multiply(m_QuatOrientation, m_QuatP1P2, m_QuatOrientation);

    // modes de dessin
    m_MeshStructureDraw = new MeshStructureDraw(m_Object->getMesh(), m_Material, nullptr, nullptr, nullptr, 0.1);
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
