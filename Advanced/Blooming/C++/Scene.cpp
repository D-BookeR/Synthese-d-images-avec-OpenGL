#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>

#include <utils.h>
#include <DeferredShadingMaterial.h>
#include <SimpleColorMaterial.h>
#include <MeshObjectFromObj.h>

#include "Scene.h"


// http://stackoverflow.com/questions/30419153/hdr-bloom-effect-rendering-pipeline-using-opengl-glsl
// http://prideout.net/archive/bloom/

/**
 * Constructeur
 */
Scene::Scene() : TurnTableScene(true)
{
    // matériau pour la vache
    vec4 cowKd = vec4::fromValues(0.7, 0.3, 0.2, 1.0);
    vec3 cowKs = vec3::fromValues(1.0, 1.0, 1.0);
    float cowNs = 120;
    m_CowMaterial = new DeferredShadingMaterial(cowKd, cowKs, cowNs);

    // matériau pour la sphère
    vec4 sun_color = vec4::fromValues(100.0, 100.0, 100.0, 1.0);
    m_SunLightMateriau = new SimpleColorMaterial(sun_color);

    // créer les objets à dessiner
    m_Ground   = new MeshObjectFromObj("data/models/TerrainSimple", "Terrain.obj", "Terrain.mtl", 4.0);
    m_Cow      = new MeshObjectFromObj("data/models/Cow", "Cow.obj", m_CowMaterial, 0.1);
    m_PalmTree = new MeshObjectFromObj("data/models/Palm_Tree", "Palm_Tree.obj", "Palm_Tree.mtl", 0.3);
    m_SunLight   = new MeshObjectFromObj("data/models/Sphere", "sphere.obj", m_SunLightMateriau, 0.5);

    // définir une lampe spot qui projette de l'ombre
    m_Light = new SoftSpotLight(115.0, 115.0, 0.15, 512, 1.5, 10.0);
    m_Light->setPosition(vec4::fromValues(2, 4, 3, 1));
    m_Light->setTarget(vec4::fromValues(0, 0, 0, 1));
    m_Light->setColor(vec3::fromValues(20,20,20));
    addLight(m_Light);

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // couleur du fond
    glClearColor(0.2, 0.3, 0.4, 0.0);

    // paramètres de la vue
    m_CameraAngleAzimut = -175.0;
    m_CameraAngleElevation = -7.5;
    vec3::set(m_CameraPivot, -0.5, -1.7, 0.1);
    m_CameraDistance = 9.4;

    // ressources
    m_FBOimage = nullptr;
    m_FBO1 = nullptr;
    m_FBO2 = nullptr;
    m_GaussianBlur = nullptr;
    m_LuminosityContrast = new LuminosityContrast();
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
    mat4::perspective(m_Mat4Projection, Utils::radians(25.0), (float)width / height, 1.0, 50.0);

    // créer un FBO de cette taille pour dessiner hors écran
    const int K = 6;
    if (m_FBOimage != nullptr) delete m_FBOimage;
    m_FBOimage = new FrameBufferObject(width, height, GL_TEXTURE_2D, GL_RENDERBUFFER, 0, GL_LINEAR);
    if (m_FBO1 != nullptr) delete m_FBO1;
    m_FBO1   = new FrameBufferObject(width/K, height/K, GL_TEXTURE_2D, GL_NONE, 0, GL_LINEAR);
    if (m_FBO2 != nullptr) delete m_FBO2;
    m_FBO2   = new FrameBufferObject(width/K, height/K, GL_TEXTURE_2D, GL_NONE, 0, GL_LINEAR);

    // traitement d'image nécessaire
    if (m_GaussianBlur != nullptr) delete m_GaussianBlur;
    m_GaussianBlur = new GaussianBlur(width/K, height/K);
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

    // dessiner le globe éclairant
    mat4::translate(mat4ModelView, mat4View, vec3::fromVec(m_Light->getPosition()));
    m_SunLight->onDraw(mat4Projection, mat4ModelView);

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

    // dessiner les vaches
    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(-2.0,0.0,0.5));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(120.0));
    m_Cow->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(1.0,0.0,2.0));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(-20.0));
    m_Cow->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(-1.0,0.0,2.0));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(200.0));
    m_Cow->onDraw(mat4Projection, mat4ModelView);
}


/**
 * Dessine l'image courante
 */
void Scene::onDrawFrame()
{
    // effacer l'écran
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // construire la matrice caméra <- scène (fait passer de la scène à la caméra)
    mat4 mat4CameraScene = getModelView();

    // préparer les lampes (transformation et shadow maps)
    prepareLights(mat4CameraScene);

    // dessiner les objets dans cette transformation
    drawDeferredShading(m_Mat4Projection, mat4CameraScene);

    // rediriger les dessins vers le FBO net
    m_FBOimage->enable();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // dessiner l'éclairement de la lampe
    addLighting(m_Light, true);

    // revenir au dessin sur l'écran
    m_FBOimage->disable();
    //m_FBOimage->onDraw(GL_COLOR_ATTACHMENT0);return;


    /// à ce stade, on a une image nette dans m_FBOimage, on va la seuiller dans m_FBO1


    // rediriger les dessins vers le FBO1
    m_FBO1->enable();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // éclaicir et constraster l'image pour faire ressortir les zones brillantes
    m_LuminosityContrast->process(m_FBOimage, -0.1, 40.0);

    // revenir au dessin sur l'écran
    m_FBO1->disable();
    //m_FBO1->onDraw(GL_COLOR_ATTACHMENT0);return;


    /// à ce stade, on a une image en noir et blanc dans m_FBO1, on va la flouter dans m_FBO2


    // rediriger les dessins vers le FBO2
    m_FBO2->enable();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // appliquer un flou pour étaler les zones brillantes
    m_GaussianBlur->process(m_FBO1, 1.2);

    // revenir au dessin sur l'écran
    m_FBO2->disable();
    //m_FBO2->onDraw(GL_COLOR_ATTACHMENT0);return;


    /// à ce stade, on a une image en noir et blanc dans m_FBO2, on va la superposer à m_FBOimage


    // dessiner le color buffer net sur l'écran
    m_FBOimage->onDraw(GL_COLOR_ATTACHMENT0);

    // superposer l'image floue des zones brillantes
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    m_FBO2->onDraw(GL_COLOR_ATTACHMENT0);
    glDisable(GL_BLEND);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_Light;
    delete m_SunLight;
    delete m_PalmTree;
    delete m_Cow;
    delete m_Ground;
    delete m_CowMaterial;
    delete m_SunLightMateriau;
    delete m_LuminosityContrast;
    delete m_GaussianBlur;
    delete m_FBOimage;
    delete m_FBO1;
    delete m_FBO2;
}
