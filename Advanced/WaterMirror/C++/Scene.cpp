#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

#include <utils.h>
#include <Stencil.h>

#include "Scene.h"
#include <SoftSpotLight.h>


/**
 * Constructeur
 */
Scene::Scene() : TurnTableScene(true)
{
    // créer le matériau de l'eau
    m_WaterMaterial = new WaterMaterial();

    // créer les objets à dessiner
    const float depth = -0.03;
    const float scale = 4.0;
    m_Island = new Island(120, scale, 0.6, scale * depth);
    m_Ground = new Ground(scale * (depth * 0.9));
    m_Water = new Water(m_WaterMaterial);

    // définir une lampe ambiante
    m_Light0 = new Light();
    m_Light0->setColor(vec3::fromValues(0.5, 0.5, 0.5));
    addLight(m_Light0);

    // définir une lampe directionnelle
    m_Light1 = new OmniLight();
    m_Light1->setPosition(vec4::fromValues(1.0, 2.0, 1.0, 0.0));
    m_Light1->setColor(vec3::fromValues(1.0, 1.0, 1.0));
    addLight(m_Light1);

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // couleur du fond
    glClearColor(0.7, 1.0, 1.0, 0.0);

    // paramètres de la vue
    m_CameraAngleAzimut = -10.0;
    m_CameraAngleElevation = 5.5;
    vec3::set(m_CameraPivot, 0.0, -0.1, 0.0);
    m_CameraDistance = 6;

    // FBOs pour mémoriser le reflet et la vue du fond et les combiner dans le WaterMaterial
    m_FBOreflection = nullptr;
    m_FBObackground = nullptr;

    // application de la brume de distance
    m_FBOfog = nullptr;
    m_Fog = new Fog(vec4::fromValues(0.7, 0.9, 1.0, 1.0), 40.0);
}


/**
 * appelée quand la taille de la vue OpenGL change
 * @param width : largeur en nombre de pixels de la fenêtre
 * @param height : hauteur en nombre de pixels de la fenêtre
 */
void Scene::onSurfaceChanged(int width, int height)
{
    // échelle des FBO par rapport à l'écran
    const int antialias = 2;

    // appeler la méthode de la superclasse
    TurnTableScene::onSurfaceChanged(width, height, antialias);

    // matrice de projection (champ de vision)
    mat4::perspective(m_Mat4Projection, Utils::radians(12.0), (float)width / height, 1.0, 100.0);

    // créer un FBO pour stocker la vue du reflet
    if (m_FBOreflection != nullptr) delete m_FBOreflection;
    m_FBOreflection = new FrameBufferObject(width*antialias, height*antialias, GL_TEXTURE_2D, GL_RENDERBUFFER);

    // créer un FBO pour stocker la vue du fond (vue à travers l'eau) avec un buffer de plus pour la position
    if (m_FBObackground != nullptr) delete m_FBObackground;
    m_FBObackground = new FrameBufferObject(width*antialias, height*antialias, GL_TEXTURE_2D, GL_RENDERBUFFER, 1);

    // créer un FBO pour stocker la vue finale à superposer avec de la brume de distance
    if (m_FBOfog != nullptr) delete m_FBObackground;
    m_FBOfog = new FrameBufferObject(width*antialias, height*antialias, GL_TEXTURE_2D, GL_TEXTURE_2D);

    // fournir ces FBO au matériau de l'eau
    m_WaterMaterial->setGBuffers(m_FBOreflection, m_FBObackground, width*antialias, height*antialias);
}


/**
 * appelée quand on enfonce le bouton gauche et qu'on déplace la souris
 * @param oldX : ancienne absisse de la souris
 * @param oldY : ancienne ordonnée de la souris
 * @param x : nouvelle absisse de la souris
 * @param y : nouvelle ordonnée de la souris
 * @param width largeur de la vue OpenGL
 * @param height hauteur de la vue OpenGL
 */
void Scene::onRotate(float oldX, float oldY, float x, float y, int width, int height)
{
    // appeler la méthode de la superclasse
    TurnTableScene::onRotate(oldX, oldY, x, y, width, height);

    // empêcher que l'angle de hauteur devienne trop faible
    if (m_CameraAngleElevation < 0.0) m_CameraAngleElevation = 0.0;
    updateInfos();
}


/**
 * appelée quand on enfonce le bouton droit et qu'on déplace la souris
 * @param dx : nombre de pixels parcourus horizontalement
 * @param dy : nombre de pixels parcourus verticalement
 */
void Scene::onPan(float dx, float dy)
{
    // appeler la méthode de la superclasse
    TurnTableScene::onPan(dx, dy);

    // empêcher que la hauteur devienne négative
    if (m_CameraPivot[1] > 0.0) m_CameraPivot[1] = 0.0;
    updateInfos();
}


/**
 * appelée quand on appuie sur une touche du clavier
 * @param code : touche enfoncée
 */
void Scene::onKeyDown(unsigned char code)
{
    // appeler la méthode de la superclasse
    TurnTableScene::onKeyDown(code);

    // empêcher que la hauteur devienne négative
    if (m_CameraPivot[1] > 0.0) m_CameraPivot[1] = 0.0;
    updateInfos();
}


/**
 * dessine les objets de la scène
 * @param mat4Projection : matrice de projection
 * @param mat4View : matrice de vue
 */
void Scene::onDraw(mat4& mat4Projection, mat4& mat4View)
{
    // dessiner le terrain
    m_Island->onDraw(mat4Projection, mat4View);

    // NB: le fond n'est pas dessiné ici car il n'a pas de plan de coupe
}


/**
 * Dessine l'image courante
 */
void Scene::onDrawFrame()
{
    /*** PREPARATION DES MATRICES ***/

    // construire la matrice caméra <- scène (fait passer de la scène à la caméra)
    mat4 mat4View = getModelView();

    // transformation du miroir par rapport à la scène : aucune
    mat4 mat4ModelMirror = mat4::create();

    // inverse de la matrice qui positionne le miroir dans la scène
    mat4 mat4ModelMirrorInv = mat4::create();
    mat4::invert(mat4ModelMirrorInv, mat4ModelMirror);

    // matrice pour positionner le miroir par rapport à la caméra
    mat4 mat4ModelViewMirror = mat4::create();
    mat4::multiply(mat4ModelViewMirror, mat4View, mat4ModelMirror);

    // matrice pour dessiner le reflet de la scène par rapport à la caméra
    mat4 mat4ViewReflected = mat4::create();
    mat4::scale(mat4ViewReflected, mat4ViewReflected, vec3::fromValues(1,-1,1));
    mat4::multiply(mat4ViewReflected, mat4ViewReflected, mat4ModelMirrorInv);
    mat4::multiply(mat4ViewReflected, mat4ModelViewMirror, mat4ViewReflected);

    // transformation des plans de coupe
    mat4 mat4ModelViewMirrorInverse = mat4::create();
    mat4::transpose(mat4ModelViewMirrorInverse, mat4ModelViewMirror);
    mat4::invert(mat4ModelViewMirrorInverse, mat4ModelViewMirrorInverse);

    // plan de coupe pour ne voir que le dessus
    vec4 MirrorPlaneOver = vec4::fromValues(0, +1, 0, 0);
    vec4::transformMat4(MirrorPlaneOver, MirrorPlaneOver, mat4ModelViewMirrorInverse);

    // plan de coupe pour ne voir que le fond et un tout petit peu au dessus de l'eau
    vec4 MirrorPlaneUnder = vec4::fromValues(0, -1, 0, 0.01);
    vec4::transformMat4(MirrorPlaneUnder, MirrorPlaneUnder, mat4ModelViewMirrorInverse);


    /*** DESSIN ***/

    /** étape 1 : dessiner le reflet du paysage dans le FBO reflet **/

    // préparer les lampes pour la vue inversée
    prepareLights(mat4ViewReflected);

    // fournir le plan de coupe ne dessinant que le fond à tous les objets dessinés
    m_Island->setClipPlane(true, MirrorPlaneUnder);

    // rediriger les dessins vers le g-buffer
    m_GBuffer->enable();
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // dessiner les objets inversés, mais seulement ceux qui sont au dessus de l'eau
    glFrontFace(GL_CW);
    onDraw(m_Mat4Projection, mat4ViewReflected);
    glFrontFace(GL_CCW);

    // revenir au dessin sur l'écran
    m_GBuffer->disable();

    // enlever le plan de coupe sans recompiler le shader
    m_Island->setClipPlane(true);

    // activer le FBO pour la brume
    m_FBOfog->enable();

    // ajouter les éclairements dans le FBO de la brume
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    addLightings();

    // activer le FBO pour la brume
    m_FBOfog->disable();

    // (debug) dessiner le FBO sur l'écran
    //m_FBOfog->onDraw(GL_COLOR_ATTACHMENT0);return;
    //m_FBOfog->onDraw(GL_DEPTH_ATTACHMENT);return;

    // rediriger les dessins vers le FBO des reflets
    m_FBOreflection->enable();

    // appliquer la brume
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_Fog->process(m_FBOfog);

    // revenir au dessin sur l'écran
    m_FBOreflection->disable();

    // (debug) dessiner le FBO sur l'écran
    //m_FBOreflection->onDraw(GL_COLOR_ATTACHMENT0+0);return;
    //m_FBOreflection->onDraw(GL_COLOR_ATTACHMENT0+1);return;


    /** étape 2 : dessiner le fond du paysage dans le FBO fond **/

    // préparer les lampes pour la vue normale
    prepareLights(mat4View);

    // fournir le plan de coupe ne dessinant que le fond à tous les objets dessinés
    m_Island->setClipPlane(true, MirrorPlaneUnder);

    // échanger le buffer des positions du g-buffer avec le 2e buffer du FBO du fond
    m_GBuffer->swapColorBuffers(2, m_FBObackground, 1);

    // rediriger les dessins vers le g-buffer
    m_GBuffer->enable();
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // dessiner les objets, mais seulement ce qui est sous l'eau
    onDraw(m_Mat4Projection, mat4View);
    m_Ground->onDraw(m_Mat4Projection, mat4View);

    // revenir au dessin sur l'écran
    m_GBuffer->disable();

    // enlever le plan de coupe sans recompiler le shader
    m_Island->setClipPlane(true);

    // rediriger les dessins vers le FBO du fond
    m_FBObackground->enable();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // ajouter les éclairements
    addLightings();

    // revenir au dessin sur l'écran
    m_FBObackground->disable();

    // défaire l'échange des buffers
    m_GBuffer->swapColorBuffers(2, m_FBObackground, 1);

    // (debug) dessiner le FBO sur l'écran
    //m_FBObackground->onDraw(GL_COLOR_ATTACHMENT0+0);return;
    //m_FBObackground->onDraw(GL_COLOR_ATTACHMENT0+1);return;


    /** étape 3 : dessiner la partie aérienne et la surface de l'eau avec son matériau spécial **/

    // fournir le plan de coupe à tous les objets dessinés
    m_Island->setClipPlane(true, MirrorPlaneOver);

    // rediriger les dessins vers le g-buffer
    m_GBuffer->enable();
    glClearColor(0.7, 0.9, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // dessiner le terrain
    m_Island->onDraw(m_Mat4Projection, mat4View);

    // dessiner l'eau => fusion reflet et fond
    m_Water->onDraw(m_Mat4Projection, mat4View);

    // revenir au dessin sur l'écran
    m_GBuffer->disable();

    // enlever le plan de coupe sans recompiler le shader
    m_Island->setClipPlane(true);

    // activer le FBO pour la brume
    m_FBOfog->enable();

    // ajouter les éclairements dans le FBO de la brume
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    addLightings();

    // désactiver le FBO pour la brume
    m_FBOfog->disable();

    // (debug) dessiner le FBO sur l'écran
    //m_FBOfog->onDraw(GL_COLOR_ATTACHMENT0);return;
    //m_FBOfog->onDraw(GL_DEPTH_ATTACHMENT);return;

    // appliquer la brume
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_Fog->process(m_FBOfog);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_Fog;
    delete m_FBOfog;
    delete m_FBOreflection;
    delete m_FBObackground;
    delete m_Light1;
    delete m_Light0;
    delete m_Water;
    delete m_Ground;
    delete m_Island;
    delete m_WaterMaterial;
}
