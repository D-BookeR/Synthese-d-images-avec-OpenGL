// Définition de la classe FirstPersonScene

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <ios>

#include <utils.h>
#include <FirstPersonScene.h>



/**
 * Définit une base pour dessiner une scène en mode vue première personne
 * @param deferredShading : true si cette classe gère le FBO m_GBuffer pour un dessin en deux passes
 */
FirstPersonScene::FirstPersonScene(bool deferredShading) :
    SceneBase(deferredShading)
{
    // paramètres de la vue
    m_CameraAngleAzimut = 0.0;
    m_CameraAngleElevation = 0.0;
    m_CameraInvPosition = vec3::create();
    m_Mat4TMP = mat4::create();
}


/** destructeur */
FirstPersonScene::~FirstPersonScene()
{
}


/** affiche les informations sur la caméra **/
void FirstPersonScene::updateInfos()
{
    std::streamsize ss = std::cout.precision();
    std::cout << std::fixed;
    std::cout.precision(1);
    std::cout << "\razimut = " << m_CameraAngleAzimut;
    std::cout << " height = " << m_CameraAngleElevation;
    std::cout.precision(2);
    std::cout << " position = (" << -m_CameraInvPosition[0] << ", " << -m_CameraInvPosition[1] << ", " << -m_CameraInvPosition[2] << ")";
    std::cout << "      ";
    std::cout.precision(ss);
}


/**
 * appelée quand on enfonce le bouton du milieu et qu'on déplace la souris
 * @param dd : nombre de pixels parcourus
 */
void FirstPersonScene::onZoom(float dd)
{
}


/**
 * appelée quand on enfonce le bouton droit et qu'on déplace la souris
 * @param dx : nombre de pixels parcourus horizontalement
 * @param dy : nombre de pixels parcourus verticalement
 */
void FirstPersonScene::onPan(float dx, float dy)
{
    // on ne peut pas faire grand chose, un tel décalage n'a pas beaucoup de sens
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
void FirstPersonScene::onRotate(float oldX, float oldY, float x, float y, int width, int height)
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
void FirstPersonScene::onKeyDown(unsigned char code)
{
    // construire la matrice inverse de l'orientation de la vue à la souris
    mat4::identity(m_Mat4TMP);
    mat4::rotate(m_Mat4TMP, m_Mat4TMP, Utils::radians(m_CameraAngleAzimut), vec3::fromValues(0, -1, 0));
    mat4::rotate(m_Mat4TMP, m_Mat4TMP, Utils::radians(m_CameraAngleElevation), vec3::fromValues(-1, 0, 0));

    // vecteur indiquant le décalage à appliquer au pivot de la rotation
    vec3 offset = vec3::create();
    switch (code) {
    case 'W':
        // avancer
        vec3::transformMat4(offset, vec3::fromValues(0, 0, -0.1), m_Mat4TMP);
        break;
    case 'S':
        // reculer
        vec3::transformMat4(offset, vec3::fromValues(0, 0, +0.1), m_Mat4TMP);
        break;
    case 'A':
        // décaler à gauche
        vec3::transformMat4(offset, vec3::fromValues(+0.1, 0, 0), m_Mat4TMP);
        break;
    case 'D':
        // décaler à droite
        vec3::transformMat4(offset, vec3::fromValues(-0.1, 0, 0), m_Mat4TMP);
        break;
    case 'Q':
        // monter
        vec3::transformMat4(offset, vec3::fromValues(0, -0.1, 0), m_Mat4TMP);
        break;
    case 'Z':
        // descendre
        vec3::transformMat4(offset, vec3::fromValues(0, +0.1, 0), m_Mat4TMP);
        break;
    default:
        // appeler la méthode de la superclasse
        SceneBase::onKeyDown(code);
    }

    // appliquer le décalage à la position
    vec3::add(m_CameraInvPosition, m_CameraInvPosition, offset);

    // (optionnel) bloquer l'altitude à une valeur minimale
    if (m_CameraInvPosition[1] > -0.5) {
        m_CameraInvPosition[1] = -0.5;
    }

    // afficher les informations sur l'écran
    updateInfos();
}


/**
 * retourne la matrice ModelView pour dessiner la scène
 * @return matrice ModelView
 */
mat4 FirstPersonScene::getModelView()
{
    // initialiser la matrice ModelView
    mat4::identity(m_Mat4ModelView);

    // orientation de la scène par rapport à la caméra
    mat4::rotateX(m_Mat4ModelView, m_Mat4ModelView, Utils::radians(m_CameraAngleElevation));
    mat4::rotateY(m_Mat4ModelView, m_Mat4ModelView, Utils::radians(m_CameraAngleAzimut));

    // définir le pivot des rotations
    mat4::translate(m_Mat4ModelView, m_Mat4ModelView, m_CameraInvPosition);

    return m_Mat4ModelView;
}
