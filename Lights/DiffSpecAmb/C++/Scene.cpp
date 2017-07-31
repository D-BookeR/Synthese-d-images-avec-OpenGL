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
    m_LightsPositions.push_back(vec4::fromValues(-6.0, 6.0, 4.0, 1.0));
    m_LightsPositions.push_back(vec4::fromValues( 7.0, 5.0,-5.0, 1.0));
    int lights_count = m_LightsPositions.size();

    // couleurs des lampes
    m_LightsColors.push_back(vec3::fromValues(0.9, 0.9, 0.9));
    m_LightsColors.push_back(vec3::fromValues(0.7, 0.7, 0.7));

    // créer les matériaux pour les pommes
    m_Texture = new Texture2D("data/models/Apple/skin.jpg", GL_LINEAR, GL_CLAMP_TO_EDGE);
    vec3 Ka = vec3::fromValues(0.2, 0.2, 0.2);
    vec3 Ks = vec3::fromValues(0.7, 0.7, 0.7);
    float ns = 64;
    m_AmbiantMaterial  = new AmbiantMaterial(lights_count, Ka, m_Texture);
    m_DiffuseMaterial  = new DiffuseMaterial(lights_count, m_Texture);
    m_SpecularMaterial = new SpecularMaterial(lights_count, Ks, ns);
    m_BlinnMaterial    = new BlinnMaterial(lights_count, Ka, m_Texture, Ks, ns);

    // créer les objets
    m_AppleAmbiant    = new MeshObjectFromObj("data/models/Apple", "apple.obj", m_AmbiantMaterial, 0.01);
    m_AppleDiffuse     = new MeshObjectFromObj("data/models/Apple", "apple.obj", m_DiffuseMaterial, 0.01);
    m_SpecularApple = new MeshObjectFromObj("data/models/Apple", "apple.obj", m_SpecularMaterial, 0.01);
    m_AppleBlinn      = new MeshObjectFromObj("data/models/Apple", "apple.obj", m_BlinnMaterial, 0.01);

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // couleur du fond
    glClearColor(0.4, 0.4, 0.4, 0.0);

    // paramètres de la vue
    m_CameraAngleAzimut = 20.0;
    m_CameraAngleElevation = 20.0;
    vec3::set(m_CameraPivot, 0.0, -0.4, 0.0);
    m_CameraDistance = 5.0;

    // taille des sous-fenêtres : 1/2 de la fenêtre principale
    m_WindowWidth2 = 0;
    m_WindowHeight2 = 0;
}


/**
 * appelée quand la taille de la vue OpenGL change
 * @param width : largeur en nombre de pixels de la fenêtre
 * @param height : hauteur en nombre de pixels de la fenêtre
 */
void Scene::onSurfaceChanged(int width, int height)
{
    // taille des sous-fenêtres : 1/2 de la fenêtre principale
    m_WindowWidth2 = width/2.0;
    m_WindowHeight2 = height/2.0;

    // matrice de projection (champ de vision)
    mat4::perspective(m_Mat4Projection, Utils::radians(10.0), (float)m_WindowWidth2 / m_WindowHeight2, 1.0, 15.0);
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
        m_AmbiantMaterial->setLightPositionColor(i, position, m_LightsColors[i]);
        m_DiffuseMaterial->setLightPositionColor(i, position, m_LightsColors[i]);
        m_SpecularMaterial->setLightPositionColor(i, position, m_LightsColors[i]);
        m_BlinnMaterial->setLightPositionColor(i, position, m_LightsColors[i]);
    }

    // matrice temporaire pour faire tourner la pomme
    mat4 mat4ModelViewApple = mat4::create();
    mat4::rotateY(mat4ModelViewApple, mat4ModelView, Utils::radians(Utils::Time * 15.0 + 100.0));

    // dessiner l'éclairement ambiant dans le coin haut gauche
    glViewport(0, m_WindowHeight2, m_WindowWidth2, m_WindowHeight2);
    m_AppleAmbiant->onDraw(mat4Projection, mat4ModelViewApple);

    // dessiner l'éclairement diffus dans le coin haut droit
    glViewport(m_WindowWidth2, m_WindowHeight2, m_WindowWidth2, m_WindowHeight2);
    m_AppleDiffuse->onDraw(mat4Projection, mat4ModelViewApple);

    // dessiner l'éclairement spéculaire dans le coin bas gauche
    glViewport(0, 0, m_WindowWidth2, m_WindowHeight2);
    m_SpecularApple->onDraw(mat4Projection, mat4ModelViewApple);

    // dessiner les éclairements superposés dans le coin bas droit
    glViewport(m_WindowWidth2, 0, m_WindowWidth2, m_WindowHeight2);
    m_AppleBlinn->onDraw(mat4Projection, mat4ModelViewApple);
}


/** supprime tous les objets alloués */
Scene::~Scene()
{
    delete m_AppleBlinn;
    delete m_SpecularApple;
    delete m_AppleDiffuse;
    delete m_AppleAmbiant;
    delete m_AmbiantMaterial;
    delete m_DiffuseMaterial;
    delete m_SpecularMaterial;
    delete m_BlinnMaterial;
    delete m_Texture;
}
