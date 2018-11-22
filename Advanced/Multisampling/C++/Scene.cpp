#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

#include <utils.h>
#include <MeshObjectFromObj.h>

#include <FrameBufferObjectMS.h>
#include "Scene.h"


/**
 * Constructeur
 */
Scene::Scene() : TurnTableScene(false)
{
    // créer les objets à dessiner
    m_Ground   = new MeshObjectFromObj("data/models/TerrainSimple", "Terrain.obj", "Terrain.mtl", 4.0);
    m_Lorry    = new MeshObjectFromObj("data/models/Camion", "camion.obj", "camion.mtl", 2.0);
    m_PalmTree = new MeshObjectFromObj("data/models/Palm_Tree", "Palm_Tree.obj", "Palm_Tree.mtl", 0.3);

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // nombre maximal d'échantillons
    int maxsamples = 1;
    glGetIntegerv(GL_MAX_SAMPLES, &maxsamples);
    std::cout << "Max number of samples = " << maxsamples << std::endl;
    /// REMARQUE : il semble que le FBO MS ne puisse pas toujours être créé avec le nombre maximal

    // nombre d'échantillons initialisé à 2
    m_SamplesNumber = std::min(2, maxsamples);

    // couleur du fond
    glClearColor(0.4, 0.5, 0.6, 0.0);

    // paramètres de la vue
    m_CameraAngleAzimut = -47.5;
    m_CameraAngleElevation = 12.0;
    vec3::set(m_CameraPivot, 0.0, -1.5, 0.8);
    m_CameraDistance = 20;

    // FBO
    m_Width = 0;
    m_Height = 0;
    m_GBuffer = nullptr;
    m_SubPicture = nullptr;
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_Ground;
    delete m_Lorry;
    delete m_PalmTree;
    delete m_SubPicture;
}


/**
 * appelée quand la taille de la vue OpenGL change
 * @param width : largeur en nombre de pixels de la fenêtre
 * @param height : hauteur en nombre de pixels de la fenêtre
 */
void Scene::onSurfaceChanged(int width, int height)
{
    // matrice de projection (champ de vision)
    mat4::perspective(m_Mat4Projection, Utils::radians(12.0), (float)width / height, 1.0, 100.0);

    // créer un FBO multisampling
    m_Width = width;
    m_Height = height;
    recreateFBO();
}


/**
 * recrée les FBO utilisés en fonction de la taille de la fenêtre OpenGL
 */
void Scene::recreateFBO()
{
    std::cout << "Current samples number = " << m_SamplesNumber << std::endl;
    if (m_GBuffer != nullptr) delete m_GBuffer;
    if (m_SamplesNumber > 1) {
        glEnable(GL_MULTISAMPLE);
        m_GBuffer = new FrameBufferObjectMS(m_Width, m_Height, m_SamplesNumber, GL_TEXTURE_2D, GL_RENDERBUFFER, 3);
    } else {
        glDisable(GL_MULTISAMPLE);
        m_GBuffer = new FrameBufferObject(m_Width, m_Height, GL_TEXTURE_2D, GL_RENDERBUFFER, 3);
    }

    if (m_SubPicture != nullptr) delete m_SubPicture;
    m_SubPicture = new SubPicture(m_SamplesNumber);
}


/**
 * appelée quand on appuie sur une touche du clavier
 * @param code : touche enfoncée
 */
void Scene::onKeyDown(unsigned char code)
{
    int maxsamples = 1;
    glGetIntegerv(GL_MAX_SAMPLES, &maxsamples);
    if (code == 'F') {
        if (m_SamplesNumber < maxsamples) m_SamplesNumber++;
        recreateFBO();
    } else if (code == 'G') {
        if (m_SamplesNumber > 1) m_SamplesNumber--;
        recreateFBO();
    } else {
        // appeler la méthode de la superclasse
        SceneBase::onKeyDown(code);
    }
}


/**
 * dessine les objets de la scène
 * @param mat4Projection : matrice de projection
 * @param mat4View : matrice de vue
 */
void Scene::onDraw(mat4& mat4Projection, mat4& mat4View)
{
    // matrice temporaire
    mat4 mat4ModelView = mat4::create();

    // dessiner le terrain
    m_Ground->onDraw(mat4Projection, mat4View);

    // dessiner les palmiers
    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(0.5,0,0.5));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(120.0));
    m_PalmTree->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(3.0,0,1.5));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(20.0));
    m_PalmTree->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(2.0,0,-3.5));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(20.0));
    m_PalmTree->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(-1.0,0.0,-2.0));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(80.0));
    m_PalmTree->onDraw(mat4Projection, mat4ModelView);

    // dessiner les camions
    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(-2.0,0.0,0.5));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(120.0));
    m_Lorry->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(1.0,0.0,2.0));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(-20.0));
    m_Lorry->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(-1.0,0.0,2.0));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(200.0));
    m_Lorry->onDraw(mat4Projection, mat4ModelView);
}


/**
 * Dessine l'image courante
 */
void Scene::onDrawFrame()
{
    // construire la matrice caméra <- scène (fait passer de la scène à la caméra)
    mat4 mat4CameraScene = getModelView();

    // dessiner les objets dans cette transformation (pas de lampes, donc pas de addLightings)
    drawDeferredShading(m_Mat4Projection, mat4CameraScene);

    // afficher le FBO complet
    glViewport(0, 0, m_Width, m_Height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // variante en utilisant le traitement SubPicture
    //m_SubPicture->process(m_GBuffer, 0, 0, m_Width, m_Height);

    // variante en utilisant glBlitFramebuffer
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_GBuffer->getId());
    glReadBuffer(GL_COLOR_ATTACHMENT0);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);

    glBlitFramebuffer(
        0, 0, m_GBuffer->getWidth(), m_GBuffer->getHeight(),
        0, 0, m_GBuffer->getWidth(), m_GBuffer->getHeight(),
        GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // afficher une vignette avec un zoom sur la partie centrale du FBO
    glViewport(0, m_Height*3/5, m_Width*2/5, m_Height*2/5);
    glClear(GL_DEPTH_BUFFER_BIT);
    m_SubPicture->process(m_GBuffer, m_Width/2 - m_Width/8, m_Height/2 - m_Height/8, m_Width/8, m_Height/8);
}
