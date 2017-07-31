// Définition de la classe TurnTableScene

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <ios>

#include <utils.h>
#include <TurnTableScene.h>



/**
 * Définit une base pour dessiner une scène en mode table tournante
 * @param deferredShading : true si cette classe gère le FBO m_GBuffer pour un dessin en deux passes
 */
TurnTableScene::TurnTableScene(bool deferredShading) :
    SceneBase(deferredShading)
{
    // paramètres de la vue
    m_CameraAngleAzimut = 0.0;
    m_CameraAngleElevation = 30.0;
    m_CameraPivot = vec3::create();
    m_CameraDistance = 15;
    m_Mat4TMP = mat4::create();
}


/** destructeur */
TurnTableScene::~TurnTableScene()
{
}


/** affiche les informations sur la caméra **/
void TurnTableScene::updateInfos()
{
    std::streamsize ss = std::cout.precision();
    std::cout << std::fixed;
    std::cout.precision(1);
    std::cout << "azimut = " << m_CameraAngleAzimut;
    std::cout << " height = " << m_CameraAngleElevation;
    std::cout.precision(2);
    std::cout << " pivot = (" << m_CameraPivot[0] << ", " << m_CameraPivot[1] << ", " << m_CameraPivot[2] << ")";
    std::cout << " distance = " << m_CameraDistance << "      \r";
    std::cout.precision(ss);
}


/**
 * appelée quand on enfonce le bouton du milieu et qu'on déplace la souris
 * @param dz : nombre de pixels parcourus
 */
void TurnTableScene::onZoom(float dz)
{
    m_CameraDistance *= exp(dz * -0.001);
    updateInfos();
}


/**
 * appelée quand on enfonce le bouton droit et qu'on déplace la souris
 * @param dx : nombre de pixels parcourus horizontalement
 * @param dy : nombre de pixels parcourus verticalement
 */
void TurnTableScene::onPan(float dx, float dy)
{
    // NB : le décalage se fait dans le plan de l'image

    // construire la matrice inverse de l'orientation de la vue à la souris
    mat4::identity(m_Mat4TMP);
    mat4::rotate(m_Mat4TMP, m_Mat4TMP, Utils::radians(m_CameraAngleAzimut), vec3::fromValues(0, -1, 0));
    mat4::rotate(m_Mat4TMP, m_Mat4TMP, Utils::radians(m_CameraAngleElevation), vec3::fromValues(-1, 0, 0));

    // transformer le mouvement de la souris dans l'espace de la scène
    vec3 offset = vec3::create();
    vec3::transformMat4(offset, vec3::fromValues(dx*0.01, dy*0.01, 0), m_Mat4TMP);
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
void TurnTableScene::onRotate(float oldX, float oldY, float x, float y, int width, int height)
{
    // différences de position
    float dx = x - oldX;
    float dy = y - oldY;

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
void TurnTableScene::onKeyDown(unsigned char code)
{
    // construire la matrice inverse de l'orientation de la vue à la souris
    mat4::identity(m_Mat4TMP);
    mat4::rotate(m_Mat4TMP, m_Mat4TMP, Utils::radians(m_CameraAngleAzimut), vec3::fromValues(0, -1, 0));
    mat4::rotate(m_Mat4TMP, m_Mat4TMP, Utils::radians(m_CameraAngleElevation), vec3::fromValues(-1, 0, 0));
    vec3 offset = vec3::create();

    switch (code) {
    case 'W':
        // avancer
        m_CameraDistance *= exp(-0.01);
        break;
    case 'S':
        // reculer
        m_CameraDistance *= exp(+0.01);
        break;
    case 'A':
        // aller à gauche
        vec3::transformMat4(offset, vec3::fromValues(+0.1, 0, 0), m_Mat4TMP);
        vec3::add(m_CameraPivot, m_CameraPivot, offset);
        break;
    case 'D':
        // aller à droite
        vec3::transformMat4(offset, vec3::fromValues(-0.1, 0, 0), m_Mat4TMP);
        vec3::add(m_CameraPivot, m_CameraPivot, offset);
        break;
    case 'Q':
        // monter
        vec3::transformMat4(offset, vec3::fromValues(0, -0.1, 0), m_Mat4TMP);
        vec3::add(m_CameraPivot, m_CameraPivot, offset);
        break;
    case 'Z':
        // descendre
        vec3::transformMat4(offset, vec3::fromValues(0, +0.1, 0), m_Mat4TMP);
        vec3::add(m_CameraPivot, m_CameraPivot, offset);
        break;
    default:
        // appeler la méthode de la superclasse
        SceneBase::onKeyDown(code);
    }
    updateInfos();
}


/**
 * retourne la matrice ModelView pour dessiner la scène
 * @return matrice ModelView
 */
mat4 TurnTableScene::getModelView()
{
    // initialiser la matrice ModelView
    mat4::identity(m_Mat4ModelView);

    // éloignement de la scène
    mat4::translate(m_Mat4ModelView, m_Mat4ModelView, vec3::fromValues(0.0, 0.0, -m_CameraDistance));

    // orientation de la scène par rapport à la caméra
    mat4::rotateX(m_Mat4ModelView, m_Mat4ModelView, Utils::radians(m_CameraAngleElevation));
    mat4::rotateY(m_Mat4ModelView, m_Mat4ModelView, Utils::radians(m_CameraAngleAzimut));

    // définir le pivot des rotations
    mat4::translate(m_Mat4ModelView, m_Mat4ModelView, m_CameraPivot);

    return m_Mat4ModelView;
}
