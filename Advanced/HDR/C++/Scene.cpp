#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>

#include <utils.h>
#include <DeferredShadingMaterial.h>
#include <SimpleColorMaterial.h>
#include <MeshObjectFromObj.h>

#include "Scene.h"


/**
 * Constructeur
 */
Scene::Scene() : TurnTableScene(true)
{
    // matériau pour la vache
    vec4 cowKd = vec4::fromValues(0.9, 0.1, 0.8, 1.0);
    vec3 cowKs = vec3::fromValues(1.0, 1.0, 1.0);
    float cowNs = 120;
    m_CowMaterial = new DeferredShadingMaterial(cowKd, cowKs, cowNs);

    // matériau pour la sphère
    vec4 sun_color = vec4::fromValues(100.0, 100.0, 100.0, 1.0);
    m_SunLightMateriau = new SimpleColorMaterial(sun_color);

    // créer les objets à dessiner
    m_Ground   = new MeshObjectFromObj("data/models/TerrainSimple", "Terrain.obj", "Terrain.mtl", 4.0);
    m_Cow      = new MeshObjectFromObj("data/models/Cow", "Cow.obj", m_CowMaterial, 0.1);
    m_PalmTree = new MeshObjectFromObj("data/models/Palm_Tree", "Palm_Tree.obj", "Palm_Tree.mtl", 0.5);
    m_SunLight   = new MeshObjectFromObj("data/models/Sphere", "sphere.obj", m_SunLightMateriau, 0.5);

    // définir une lampe spot très puissante qui projette de l'ombre
    m_Light0 = new SoftSpotLight(100.0, 100.0, 0.15, 1024, 3.0, 11.0);
    m_Light0->setPosition(vec4::fromValues(3, 6, 3, 1));
    m_Light0->setTarget(vec4::fromValues(0, 0, 0, 1));
    m_Light0->setColor(vec3::fromValues(3000.0, 3000.0, 3000.0));
    addLight(m_Light0);

    // définir une lampe omni peu intense à l'opposé du soleil
    m_Light1 = new OmniLight();
    m_Light1->setPosition(vec4::fromValues(-3, 6, -3, 1));
    m_Light1->setColor(vec3::fromValues(500.0, 500.0, 500.0));
    addLight(m_Light1);

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // couleur du fond
    glClearColor(20.0, 30.0, 40.0, 0.0);

    // paramètres de la vue
    m_CameraAngleAzimut = -200.0;
    m_CameraAngleElevation = 4.0;
    vec3::set(m_CameraPivot, 0.0, -1.0, 0.0);
    m_CameraDistance = 11.0;

    // ressources
    m_FBOhdr = nullptr;
    m_ToneMapping = new ToneMapping();
}


/**
 * appelée quand la taille de la vue OpenGL change
 * @param width : largeur en nombre de pixels de la fenêtre
 * @param height : hauteur en nombre de pixels de la fenêtre
 */
void Scene::onSurfaceChanged(int width, int height)
{
    // appeler la méthode de la superclasse
    TurnTableScene::onSurfaceChanged(width, height, 1);

    // matrice de projection (champ de vision)
    mat4::perspective(m_Mat4Projection, Utils::radians(25.0), (float)width / height, 1.0, 50.0);

    // créer un FBO de cette taille pour dessiner hors écran
    if (m_FBOhdr != nullptr) delete m_FBOhdr;
    m_FBOhdr = new FrameBufferObject(width, height, GL_TEXTURE_2D, GL_RENDERBUFFER, 0, GL_LINEAR);
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
    mat4::translate(mat4ModelView, mat4View, vec3::fromVec(m_Light0->getPosition()));
    m_SunLight->onDraw(mat4Projection, mat4ModelView);

    // dessiner les palmiers
    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(0.5,0,0.5));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(120.0));
    m_PalmTree->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(3.0,0,1.5));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(20.0));
    m_PalmTree->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(2.0,0,-1.0));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(20.0));
    m_PalmTree->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(-1.0,0.0,-1.5));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(80.0));
    m_PalmTree->onDraw(mat4Projection, mat4ModelView);

    // dessiner les vaches
    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(0.0,0.0,-0.5));
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

    // rediriger les dessins vers le FBO hdr
    m_FBOhdr->enable();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // dessiner l'éclairement des lampes
    addLightings();

    // revenir au dessin sur l'écran
    m_FBOhdr->disable();

    // (DEBUG) dessiner le color buffer net sur l'écran
    //m_FBOnet->onDraw(GL_COLOR_ATTACHMENT0);/*

    // à ce stade, on a une image hdr dans m_FBOhdr, on va réduire sa dynamique
    m_ToneMapping->process(m_FBOhdr, 5, 3);


    //*/
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_Light0;
    delete m_Light1;
    delete m_SunLight;
    delete m_PalmTree;
    delete m_Cow;
    delete m_Ground;
    delete m_CowMaterial;
    delete m_SunLightMateriau;

    delete m_ToneMapping;
    delete m_FBOhdr;
}
