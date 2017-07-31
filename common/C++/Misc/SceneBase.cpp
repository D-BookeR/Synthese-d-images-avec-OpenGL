// Définition de la classe SceneBase

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <ios>

#include <utils.h>
#include <SceneBase.h>



/**
 * Définit une base pour dessiner une scène
 * @param deferredShading : true si cette classe gère le FBO m_GBuffer pour un dessin en deux passes
 */
SceneBase::SceneBase(bool deferredShading)
{
    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // couleur du fond
    glClearColor(0.4, 0.4, 0.4, 0.0);

    // FBO pour dessiner en mode Multiple Render Targets
    m_DeferredShading = deferredShading;
    m_GBuffer = nullptr;     // sera initialisé dans onSurfaceChanged

    // matrice de projection et de transformation
    m_Mat4Projection = mat4::create();
    m_Mat4ModelView = mat4::create();
}


/** destructeur */
SceneBase::~SceneBase()
{
    if (m_GBuffer != nullptr) delete m_GBuffer;
}


/** affiche les informations sur la caméra **/
void SceneBase::updateInfos()
{
}


/**
 * appelée quand la taille de la vue OpenGL change
 * @param width : largeur en nombre de pixels de la fenêtre
 * @param height : hauteur en nombre de pixels de la fenêtre
 * @param scale : facteur d'agrandissement du FBO pour simuler un antialiasing global
 */
void SceneBase::onSurfaceChanged(int width, int height, int scale)
{
    // met en place le viewport
    glViewport(0, 0, width, height);

    // créer un FBO pour dessiner hors écran avec plusieurs buffers pour stocker toutes les informations nécessaires
    if (m_GBuffer != nullptr) delete m_GBuffer;
    if (m_DeferredShading) {
        m_GBuffer = new FrameBufferObject(width*scale, height*scale, GL_TEXTURE_2D, GL_TEXTURE_2D, 3);
    } else {
        m_GBuffer = nullptr;
    }
}


/**
 * appelée quand on enfonce le bouton du milieu et qu'on déplace la souris
 * @param dz : nombre de pixels parcourus
 */
void SceneBase::onZoom(float dz)
{
}


/**
 * appelée quand on enfonce le bouton droit et qu'on déplace la souris
 * @param dx : nombre de pixels parcourus horizontalement
 * @param dy : nombre de pixels parcourus verticalement
 */
void SceneBase::onPan(float dx, float dy)
{
}


/**
 * appelée quand on enfonce le bouton gauche et qu'on déplace la souris
 * @param oldX : ancienne absisse de la souris
 * @param oldY : ancienne ordonnée de la souris
 * @param x : nouvelle absisse de la souris
 * @param y : nouvelle ordonnée de la souris
 * @param width : largeur de la vue OpenGL
 * @param height: hauteur de la vue OpenGL
 */
void SceneBase::onRotate(float oldX, float oldY, float x, float y, int width, int height)
{
}


/**
 * appelée quand on appuie sur une touche du clavier
 * @param code : touche enfoncée
 */
void SceneBase::onKeyDown(unsigned char code)
{
    switch (code) {
    case 'P':
        // mettre en pause
        Utils::PauseTime();
        break;
    default:
        ;
    }
}


/**
 * retourne la matrice ModelView pour dessiner la scène
 * @return matrice ModelView
 */
mat4 SceneBase::getModelView()
{
    return m_Mat4ModelView;
}


/**
 * ajoute une lampe pour éclairer le matériau du renderer
 * @param light : lampe à ajouter
 */
void SceneBase::addLight(Light* light)
{
    m_Lights.push_back(light);
}


/**
 * transforme toutes les lampes
 * c'est à dire calcule la position relative à la caméra m_PositionCamera
 * à partir de la position relative à la scène m_Position
 * @param mat4View matrice de positionnement de la scène par rapport à la caméra
 */
void SceneBase::transformLights(mat4& mat4View)
{
    for (Light* light: m_Lights) {
        light->transform(mat4View);
    }
}


/**
 * dessine les shadow maps des lampes
 * @param mat4View : matrice de positionnement de la scène par rapport à la caméra
 */
void SceneBase::makeShadowMaps(mat4& mat4View)
{
    for (Light* light: m_Lights) {
        // calculer sa shadow map (si la lampe en gère une)
        light->makeShadowMap(this, mat4View);
    }
}


/**
 * transforme les lampes puis calcule leur shadow map s'il y en a
 * @param mat4View : matrice de positionnement de la scène par rapport à la caméra
 */
void SceneBase::prepareLights(mat4& mat4View)
{
    transformLights(mat4View);
    makeShadowMaps(mat4View);
}


/**
 * appelée pour dessiner en mode MRT
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void SceneBase::drawDeferredShading(mat4& mat4Projection, mat4& mat4ModelView)
{
    // rediriger les dessins vers le FBO
    if (m_GBuffer != nullptr) m_GBuffer->enable();

    // effacer l'écran
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // dessiner les objets de la scène
    onDraw(mat4Projection, mat4ModelView);

    // revenir au dessin sur l'écran
    if (m_GBuffer != nullptr) m_GBuffer->disable();

    // dessiner l'un des buffers sur l'écran
    //m_GBuffer->onDraw(GL_COLOR_ATTACHMENT0);
    //m_GBuffer->onDraw(GL_DEPTH_ATTACHMENT);
}


/**
 * rajoute l'éclairement d'une lampe
 * @param light : lampe à ajouter
 * @param is_first : mettre true si c'est la première qu'on ajoute ainsi, false sinon
 */
void SceneBase::addLighting(Light* light, bool is_first)
{
    if (!m_DeferredShading || m_GBuffer == nullptr) return;

    if (is_first) {

        // dessiner l'éclairement de la première lampe
        light->process(m_GBuffer);

    } else {

        // passer en mode blending, en tenant compte du canal alpha des éclairements ajoutés
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);

        // rajouter l'éclairement de la lampe
        light->process(m_GBuffer);

        // revenir en mode normal
        glDisable(GL_BLEND);
    }
}


/**
 * rajoute les éclairements de toutes les lampes
 */
void SceneBase::addLightings()
{
    if (m_DeferredShading && m_GBuffer != nullptr) {
        bool is_first = true;
        for (Light* light: m_Lights) {
            addLighting(light, is_first);
            is_first = false;
        }
    }
}


/**
 * dessine les objets de la scène
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void SceneBase::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
{
}


/**
 * dessin de la scène sur l'écran
 */
void SceneBase::onDrawFrame()
{
    // effacer l'écran
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // construire la matrice MV
    mat4 mat4ModelView = getModelView();

    // positionner les lampes et dessiner leur shadow map s'il y en a
    prepareLights(mat4ModelView);

    // dessiner les objets dans le FBO mrt (ou sur l'écran)
    drawDeferredShading(m_Mat4Projection, mat4ModelView);

    // ajouter les éclairements des lampes
    addLightings();
}
