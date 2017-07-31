#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

#include <utils.h>
#include <MeshObjectFromObj.h>

#include "Scene.h"


/**
 * Constructeur
 */
Scene::Scene() : SceneBase(true)
{
    // créer les objets à dessiner
    m_Ground   = new MeshObjectFromObj("data/models/TerrainSimple", "Terrain.obj", "Terrain.mtl", 2.0);
    m_Lorry    = new MeshObjectFromObj("data/models/Camion", "camion.obj", "camion.mtl", 2.0);
    m_PalmTree = new MeshObjectFromObj("data/models/Palm_Tree", "Palm_Tree.obj", "Palm_Tree.mtl", 2.0);
    m_Axes = new AxesXYZ(2.0, 1.0, 3.0);

    // définir une lampe directionnelle
    m_Light0 = new OmniLight();
    m_Light0->setPosition(vec4::fromValues(10, 5, 10, 0));
    m_Light0->setColor(vec3::fromValues(0.9,0.9,0.9));
    addLight(m_Light0);

    // définir une lampe positionnelle
    m_Light1 = new OmniLight();
    m_Light1->setPosition(vec4::fromValues(-10, 5, 5, 1));
    m_Light1->setColor(vec3::fromValues(100,100,100));
    addLight(m_Light1);

    // définir une lampe directionnelle
    m_Light2 = new OmniLight();
    m_Light2->setPosition(vec4::fromValues(-5, 5, -10, 0));
    m_Light2->setColor(vec3::fromValues(1.0,1.0,1.0));
    addLight(m_Light2);

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // couleur du fond
    glClearColor(0.4, 0.5, 0.6, 0.0);

    // paramètres de la vue
    m_CameraAngleAzimut = 60.0;
    m_CameraAngleElevation = 30.0;
    m_CameraPivot = vec3::create();
    m_CameraDistance = 35;
}


/** affiche les informations sur la caméra **/
void Scene::updateInfos()
{
    std::streamsize ss = std::cout.precision();
    std::cout << std::fixed;
    std::cout.precision(1);
    std::cout << "\razimut = " << m_CameraAngleAzimut;
    std::cout << " height = " << m_CameraAngleElevation;
    std::cout.precision(2);
    std::cout << " pivot = (" << m_CameraPivot[0] << ", " << m_CameraPivot[1] << ", " << m_CameraPivot[2] << ")";
    std::cout << " distance = " << m_CameraDistance << "      ";
    std::cout.precision(ss);
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
    mat4::perspective(m_Mat4Projection, Utils::radians(12.0), (float)width / height, 1.0, 100.0);
}


/**
 * appelée quand on enfonce le bouton du milieu et qu'on déplace la souris
 * @param dd : nombre de pixels parcourus
 */
void Scene::onZoom(float dd)
{
    m_CameraDistance *= exp(dd * -0.001);
    updateInfos();
}


/**
 * appelée quand on enfonce le bouton droit et qu'on déplace la souris
 * @param dx : nombre de pixels parcourus horizontalement
 * @param dy : nombre de pixels parcourus verticalement
 */
void Scene::onPan(float dx, float dy)
{
    // NB : le décalage se fait dans le plan de l'image

    // construire la matrice inverse de l'orientation de la vue à la souris
    mat4 ivMatrix = mat4::create();
    mat4::rotate(ivMatrix, ivMatrix, Utils::radians(m_CameraAngleAzimut), vec3::fromValues(0, -1, 0));
    mat4::rotate(ivMatrix, ivMatrix, Utils::radians(m_CameraAngleElevation), vec3::fromValues(-1, 0, 0));

    // transformer le mouvement de la souris dans l'espace de la scène
    vec3 offset = vec3::create();
    vec3::transformMat4(offset, vec3::fromValues(dx*0.01, dy*0.01, 0), ivMatrix);
    vec3::add(m_CameraPivot, m_CameraPivot, offset);

    updateInfos();
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
    // différences de position
    int dx = x - oldX;
    int dy = y - oldY;

    m_CameraAngleAzimut  += dx * 0.1;
    //m_CameraAngleAzimut = fmod(m_CameraAngleAzimut, 360.0);
    m_CameraAngleElevation += dy * 0.1;
    if (m_CameraAngleElevation >  90.0) m_CameraAngleElevation =  90.0;
    if (m_CameraAngleElevation < -90.0) m_CameraAngleElevation = -90.0;
    updateInfos();
}


/**
 * appelée quand on appuie sur une touche du clavier
 * @param code : touche enfoncée
 */
void Scene::onKeyDown(unsigned char code)
{
    // construire la matrice inverse de l'orientation de la vue à la souris
    mat4 ivMatrix = mat4::create();
    mat4::identity(ivMatrix);
    mat4::rotate(ivMatrix, ivMatrix, Utils::radians(m_CameraAngleAzimut), vec3::fromValues(0, -1, 0));
    mat4::rotate(ivMatrix, ivMatrix, Utils::radians(m_CameraAngleElevation), vec3::fromValues(-1, 0, 0));

    // vecteur indiquant le décalage à appliquer au pivot de la rotation
    vec3 offset = vec3::create();
    switch (code) {
    case 27: exit(0);
    case 'z':
        m_CameraDistance *= exp(-0.01);
        break;
    case 's':
        m_CameraDistance *= exp(+0.01);
        break;
    case 'q':
        vec3::transformMat4(offset, vec3::fromValues(+0.1, 0, 0), ivMatrix);
        break;
    case 'd':
        vec3::transformMat4(offset, vec3::fromValues(-0.1, 0, 0), ivMatrix);
        break;
    case 'a':
        vec3::transformMat4(offset, vec3::fromValues(0, -0.1, 0), ivMatrix);
        break;
    case 'w':
        vec3::transformMat4(offset, vec3::fromValues(0, +0.1, 0), ivMatrix);
        break;
    default:
        // appeler la méthode de la superclasse
        SceneBase::onKeyDown(code);
    }

    // appliquer le décalage au pivot de la rotation
    vec3::add(m_CameraPivot, m_CameraPivot, offset);

    // afficher les informations sur l'écran
    updateInfos();
}


/**
 * dessine les objets de la scène
 * @param mat4Projection : matrice de projection
 * @param mat4View : matrice de vue
 */
void Scene::onDraw(mat4& mat4Projection, mat4& mat4View)
{
    // dessiner le trièdre
    m_Axes->onDraw(mat4Projection, mat4View);

    // définir le pivot des rotations
    mat4::translate(mat4View, mat4View, m_CameraPivot);

    // dessiner le terrain
    mat4 mat4ModelView = mat4::create();
    mat4::scale(mat4ModelView, mat4View, vec3::fromValues(2.0, 2.0, 2.0));
    m_Ground->onDraw(mat4Projection, mat4ModelView);

    // dessiner les palmiers
    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(2.5,0,-1.0));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(120.0));
    mat4::scale(mat4ModelView, mat4ModelView, vec3::fromValues(0.2, 0.2, 0.2));
    m_PalmTree->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(3.0,0,1.5));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(20.0));
    mat4::scale(mat4ModelView, mat4ModelView, vec3::fromValues(0.2, 0.2, 0.2));
    m_PalmTree->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(-1,0,-2.0));
    mat4::rotate(mat4ModelView, mat4ModelView, Utils::radians(80.0), vec3::fromValues(-1,0,-1));
    mat4::scale(mat4ModelView, mat4ModelView, vec3::fromValues(0.2, 0.2, 0.2));
    m_PalmTree->onDraw(mat4Projection, mat4ModelView);

    // dessiner les camions
    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(1.5,0.5,-1.5));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(-70.0));
    mat4::rotate(mat4ModelView, mat4ModelView, Utils::radians(82.0), vec3::fromValues(0,0,1));
    m_Lorry->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(-2,0,0.5));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(120.0));
    m_Lorry->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(-1,0,-1));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(320.0));
    mat4::scale(mat4ModelView, mat4ModelView, vec3::fromValues(1.5,3.0,0.5));
    m_Lorry->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(1.15,1.0,0.7));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(60.0));
    m_Lorry->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(1,0,1));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(-20.0));
    mat4::scale(mat4ModelView, mat4ModelView, vec3::fromValues(2.0, 2.0, 2.0));
    m_Lorry->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(-1,0,2));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(200.0));
    m_Lorry->onDraw(mat4Projection, mat4ModelView);
}


/**
 * Dessine l'image courante
 */
void Scene::onDrawFrame()
{
    // effacer l'écran
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // construire la matrice caméra
    mat4 mat4View = mat4::create();

    // éloignement de la scène
    mat4::translate(mat4View, mat4View, vec3::fromValues(0.0, 0.0, -m_CameraDistance));

    // orientation de la scène par rapport à la caméra
    mat4::rotateX(mat4View, mat4View, Utils::radians(m_CameraAngleElevation));
    mat4::rotateY(mat4View, mat4View, Utils::radians(m_CameraAngleAzimut));

    // transformer les lampes
    prepareLights(mat4View);

    // dessiner les objets dans cette transformation
    drawDeferredShading(m_Mat4Projection, mat4View);

    // dessiner les éclairements des lampes
    addLightings();
}


/** supprime tous les objets créés */
Scene::~Scene()
{
    delete m_Light2;
    delete m_Light1;
    delete m_Light0;
    delete m_Axes;
    delete m_PalmTree;
    delete m_Lorry;
    delete m_Ground;
}
