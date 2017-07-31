#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

#include <utils.h>
#include "Scene.h"



/**
 * Constructeur
 */
Scene::Scene() : SceneBase(true)
{
    // créer les objets à dessiner
    m_Earth = new Earth(24, 24);
    m_Axes = new AxesXYZ(1.0, 1.0, 6.0);

    // définir une lampe omnidirectionnelle
    m_Light0 = new OmniLight();
    m_Light0->setPosition(vec4::fromValues(5, 5, 3, 0));
    m_Light0->setColor(vec3::fromValues(1.0,1.0,1.0));

    // définir une lampe ambiante
    m_Light1 = new Light();
    m_Light1->setColor(vec3::fromValues(0.7,0.7,0.7));

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // couleur du fond
    glClearColor(0.4, 0.4, 0.4, 0.0);

    // paramètres de la vue
    m_CameraZ = 25;
    m_QuatOrientation = quat::create();
}


/** affiche les informations sur la caméra **/
void Scene::updateInfos()
{
    // mode trackball, difficile d'afficher quoi que ce soit
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
    mat4::perspective(m_Mat4Projection, Utils::radians(10.0), (float)width / height, 1.0, 100.0);
}


/**
 * appelée quand on enfonce le bouton du milieu et qu'on déplace la souris
 * @param dz : nombre de pixels parcourus
 */
void Scene::onZoom(float dz)
{
    m_CameraZ *= exp(dz * -0.001);
    updateInfos();
}


/**
 * appelée quand on enfonce le bouton droit et qu'on déplace la souris
 * @param dx : nombre de pixels parcourus horizontalement
 * @param dy : nombre de pixels parcourus verticalement
 */
void Scene::onPan(float dx, float dy)
{
}


/**
 * appelée quand on enfonce le bouton gauche et qu'on déplace la souris
 * @param oldX : ancienne absisse de la souris
 * @param oldY : ancienne ordonnée de la souris
 * @param x : nouvelle absisse de la souris
 * @param y : nouvelle ordonnée de la souris
 * @param width : largeur de la vue OpenGL
 * @param height : hauteur de la vue OpenGL
 */
void Scene::onRotate(float oldX, float oldY, float x, float y, int width, int height)
{
    // normalisation de la fenêtre
    float half_width = width * 0.5;
    float half_height = height * 0.5;
    float half_size = (half_width + half_height) * 0.5;

    // construire p1
    float x1 = (oldX-half_width)/half_size;
    x1 = fmin(fmax(x1, -1.0), +1.0);
    float y1 = (half_height-oldY)/half_size;
    y1 = fmin(fmax(y1, -1.0), +1.0);
    float z1 = sqrt(fmax(1.0 - x1*x1 - y1*y1, 0.0));
    vec3 P1 = vec3::fromValues(x1,y1,z1);

    // construire p2
    float x2 = (x-half_width)/half_size;
    x2 = fmin(fmax(x2, -1.0), +1.0);
    float y2 = (half_height-y)/half_size;
    y2 = fmin(fmax(y2, -1.0), +1.0);
    float z2 = sqrt(fmax(1.0 - x2*x2 - y2*y2, 0.0));
    vec3 P2 = vec3::fromValues(x2,y2,z2);

    // construire le quaternion du mouvement P1->P2
    vec3 axis = vec3::create();
    vec3::cross(axis, P1, P2);
    float angle = vec3::length(axis);
    vec3::normalize(axis, axis);
    quat quatP1P2 = quat::create();
    quat::setAxisAngle(quatP1P2, axis, angle);

    // appliquer ce quaternion au quaternion global (attention à l'ordre)
    quat::multiply(m_QuatOrientation, quatP1P2, m_QuatOrientation);
    updateInfos();
}


/**
 * appelée quand on appuie sur une touche du clavier
 * @param code : touche enfoncée
 */
void Scene::onKeyDown(unsigned char code)
{
}


/**
 * dessine les objets de la scène
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Scene::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
{
    // dessiner la terre
    m_Earth->onDraw(mat4Projection, mat4ModelView);

    // dessiner le trièdre agrandi 2.5x
    mat4 mat4ModelViewTmp = mat4::create();
    mat4::scale(mat4ModelViewTmp, mat4ModelView, vec3::fromValues(2.5, 2.5, 2.5));
    m_Axes->onDraw(mat4Projection, mat4ModelViewTmp);
}


/**
 * Dessine l'image courante
 */
void Scene::onDrawFrame()
{
    // effacer l'écran
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // transformer le quaternion en rotation et rajouter la translation d'éloignement
    mat4 mat4ModelViewTmp = mat4::create();
    mat4::fromRotationTranslation(mat4ModelViewTmp, m_QuatOrientation, vec3::fromValues(0, 0, -m_CameraZ));

    // dessiner les objets dans cette transformation
    drawDeferredShading(m_Mat4Projection, mat4ModelViewTmp);

    // dessiner l'éclairement de la première lampe
    addLighting(m_Light0, true);

    // rajouter les éclairements des lampes suivantes
    addLighting(m_Light1);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_Light1;
    delete m_Light0;
    delete m_Axes;
    delete m_Earth;
}
