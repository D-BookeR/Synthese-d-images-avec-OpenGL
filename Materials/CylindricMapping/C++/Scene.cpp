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
    // créer le matériau basé sur une texture (au choix)
    m_Texture = new Texture2D("data/textures/earth/earth_map.jpg", GL_LINEAR, GL_CLAMP_TO_EDGE);

    // créer le cylindre
    m_Cylinder = new Cylinder(48, m_Texture);

    // créer l'objet
    m_Rock = new Rock(m_Texture, m_Cylinder);

    // paramètres de la vue
    m_CameraAngleAzimut = 25.0;
    m_CameraAngleElevation = 15.0;
    m_CameraDistance = 10.0;
    m_DisplayCylinder = true;

    // ressources
    m_FBOpreceeding = nullptr;
    m_FBOcurrent = nullptr;
    m_FBOresult = nullptr;
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
    mat4::perspective(m_Mat4Projection, Utils::radians(10.0), (float)width / height, 0.1, 15.0);

    // supprimer les anciens FBO
    if (m_FBOpreceeding != nullptr) delete m_FBOpreceeding;
    if (m_FBOpreceeding != nullptr) delete m_FBOpreceeding;
    if (m_FBOresult != nullptr) delete m_FBOresult;

    // FBO pour la première passe du dessin des objets transparents (alternances entre les deux)
    m_FBOpreceeding = new FrameBufferObject(width, height, GL_TEXTURE_2D, GL_TEXTURE_2D);
    m_FBOcurrent = new FrameBufferObject(width, height, GL_TEXTURE_2D, GL_TEXTURE_2D);

    // FBO pour accumuler la deuxième passe du dessin des objets transparents
    m_FBOresult = new FrameBufferObject(width, height, GL_TEXTURE_2D, GL_TEXTURE_2D);

    // taille de la fenêtre à l'objet transparent
    m_Cylinder->setWindowDimensions(width, height);
}


/**
 * appelée quand on appuie sur une touche du clavier
 * @param code : touche enfoncée
 */
void Scene::onKeyDown(unsigned char code)
{
    switch (code) {
    case 'T':
        m_DisplayCylinder = !m_DisplayCylinder;
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
    // effacer le depth buffer du 2e FBO mrt transparents
    glClearDepth(0.0);
    m_FBOpreceeding->enable();
    glClear(GL_DEPTH_BUFFER_BIT);
    m_FBOpreceeding->disable();
    glClearDepth(1.0);
    m_FBOcurrent->enable();
    glClear(GL_DEPTH_BUFFER_BIT);
    m_FBOcurrent->disable();


    /// dessiner les objets opaques sur le FBO couches

    // ne dessiner que les faces avant
    glEnable(GL_CULL_FACE);

    // effacer le FBO qui contiendra le résultat
    m_FBOresult->enable();
    glClearColor(0.1, 0.1, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_Rock->onDraw(mat4Projection, mat4ModelView);
    m_FBOresult->disable();

    if (m_DisplayCylinder) {

        /// Cycles d'épluchage des transparences

        // mode de transparence arrière
        glBlendEquation(GL_FUNC_ADD);
        glBlendFuncSeparate(GL_DST_ALPHA, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);

        // dessiner toutes les faces des objets
        glDisable(GL_CULL_FACE);

        const int NbCycles = 2;
        for (int i=0; i<NbCycles; i++) {

            // fournir les depth buffers des FBO au matériau transparent
            m_Cylinder->setDepthMaps(m_FBOresult->getDepthBuffer(), m_FBOpreceeding->getDepthBuffer());

            // dessiner la sphère
            m_FBOcurrent->enable();
            glClearColor(0.0, 0.0, 0.0, 0.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            m_Cylinder->onDraw(mat4Projection, mat4ModelView);
            m_FBOcurrent->disable();

            // reporter cette couche sur le FBO du résultat en mode fusion arrière
            glEnable(GL_BLEND);
            m_FBOresult->enable();
            m_FBOcurrent->onDraw(GL_COLOR_ATTACHMENT0);
            m_FBOresult->disable();
            glDisable(GL_BLEND);

            // échanger les FBO
            FrameBufferObject* tmp = m_FBOcurrent;
            m_FBOcurrent = m_FBOpreceeding;
            m_FBOpreceeding = tmp;
        }
    }

    // recopier le FBO transpa sur l'écran
    m_FBOresult->onDraw(GL_COLOR_ATTACHMENT0);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_Cylinder;
    delete m_Rock;
    delete m_FBOpreceeding;
    delete m_FBOcurrent;
    delete m_FBOresult;
}
