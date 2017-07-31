#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>

#include <utils.h>
#include <Stencil.h>
#include <MeshObjectFromObj.h>
#include <SoftSpotLight.h>

#include "Scene.h"


/**
 * Constructeur
 */
Scene::Scene() : TurnTableScene(true)
{
    // initialiser le stencil
    Stencil::init();

    // créer les objets à dessiner
    m_Ground   = new MeshObjectFromObj("data/models/TerrainSimple", "Terrain.obj", "Terrain.mtl", 4.0);
    m_Lorry    = new MeshObjectFromObj("data/models/Camion", "camion.obj", "camion.mtl", 2.0);
    m_PalmTree = new MeshObjectFromObj("data/models/Palm_Tree", "Palm_Tree.obj", "Palm_Tree.mtl", 0.3);
    m_Mirror   = new Mirror();

    // définir une lampe spot qui projette de l'ombre
    m_Light0 = new SoftSpotLight(30.0, 30.0, 0.5, 512, 14, 26, true, true, GL_NONE);
    m_Light0->setPosition(vec4::fromValues(5, 15, 15, 1));
    m_Light0->setTarget(vec4::fromValues(0, 0, 0, 1));
    m_Light0->setColor(vec3::fromValues(400,400,400));
    addLight(m_Light0);

    // définir une lampe spot qui projette de l'ombre
    m_Light1 = new SoftSpotLight(40.0, 40.0, 0.2, 512, 12, 23, true, true, GL_NONE);
    m_Light1->setPosition(vec4::fromValues(8, 15, -6, 1));
    m_Light1->setTarget(vec4::fromValues(0, 0, 0, 1));
    m_Light1->setColor(vec3::fromValues(300,300,300));
    addLight(m_Light1);

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // couleur du fond
    glClearColor(0.4, 0.5, 0.6, 0.0);

    // paramètres de la vue
    m_CameraAngleAzimut = -40.0;
    m_CameraAngleElevation = 35.0;
    vec3::set(m_CameraPivot, 0.0, -0.5, 0.0);
    m_CameraDistance = 30;
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
    mat4::perspective(m_Mat4Projection, Utils::radians(12.0), (float)width / height, 1.0, 100.0);
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

    // dessiner le cadre du miroir
    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(2.0,0.0,-1.5));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(-30.0*sin(Utils::Time*0.5)-15.0));
    m_Mirror->onDrawFrame(mat4Projection, mat4ModelView);
}


/**
 * Dessine l'image courante
 */
void Scene::onDrawFrame()
{
    /*** PREPARATION DES MATRICES ***/

    // construire la matrice caméra <- scène (fait passer de la scène à la caméra)
    mat4 mat4View = getModelView();

    // transformation du miroir par rapport à la scène
    mat4 mat4ModelMirror = mat4::create();
    mat4::translate(mat4ModelMirror, mat4ModelMirror, vec3::fromValues(2.0, 0.0, -1.5));
    mat4::rotateY(mat4ModelMirror, mat4ModelMirror, Utils::radians(-30.0*sin(Utils::Time*0.5)-15.0));

    // inverse de la matrice qui positionne le miroir dans la scène
    mat4 mat4ModelMirrorInv = mat4::create();
    mat4::invert(mat4ModelMirrorInv, mat4ModelMirror);

    // matrice pour positionner le miroir par rapport à la caméra
    mat4 mat4ModelViewMirror = mat4::create();
    mat4::multiply(mat4ModelViewMirror, mat4View, mat4ModelMirror);

    // matrice pour dessiner le reflet de la scène par rapport à la caméra
    mat4 mat4ViewReflected = mat4::create();
    mat4::scale(mat4ViewReflected, mat4ViewReflected, vec3::fromValues(1,1,-1));
    mat4::multiply(mat4ViewReflected, mat4ViewReflected, mat4ModelMirrorInv);
    mat4::multiply(mat4ViewReflected, mat4ModelViewMirror, mat4ViewReflected);

    // plan de coupe qui élimine les fragments devant le miroir (en coordonnées locales du miroir)
    vec4 mirror_plane = vec4::fromValues(0, 0, -1, 0);

    // transformer le plan de coupe en coordonnées caméra
    mat4 mat4ModelViewMirrorInverse = mat4::create();
    mat4::transpose(mat4ModelViewMirrorInverse, mat4ModelViewMirror);
    mat4::invert(mat4ModelViewMirrorInverse, mat4ModelViewMirrorInverse);
    vec4::transformMat4(mirror_plane, mirror_plane, mat4ModelViewMirrorInverse);


    /*** DESSIN ***/

    // préparer les lampes (transformation et shadow maps)
    prepareLights(mat4ViewReflected);

    // effacer l'écran
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /** étape 1 : la découpe du stencil par le miroir **/

    // créer le stencil avec ce qu'on voit du miroir
    Stencil::create();

    // dessiner le miroir
    m_Mirror->onDrawSurface(m_Mat4Projection, mat4ModelViewMirror);

    /** étape 2 : le reflet à travers la découpe **/

    // maintenant, on va dessiner le reflet vu dans le miroir
    Stencil::use();

    // fournir le plan de coupe à tous les objets dessinés
    m_Lorry->setClipPlane(true, mirror_plane);
    m_PalmTree->setClipPlane(true, mirror_plane);
    m_Ground->setClipPlane(true, mirror_plane);
    m_Mirror->setClipPlane(true, mirror_plane);

    // dessiner les objets inversés, avec la matrice de la scène reflétée
    glFrontFace(GL_CW);
    drawDeferredShading(m_Mat4Projection, mat4ViewReflected);
    glFrontFace(GL_CCW);

    // enlever le plan de coupe sans recompiler le shader
    m_Lorry->setClipPlane(true);
    m_PalmTree->setClipPlane(true);
    m_Ground->setClipPlane(true);
    m_Mirror->setClipPlane(true);

    // dessiner les éclairements visibles dans le reflet
    addLightings();

    // ne plus utiliser le stencil
    Stencil::disable();

    /** étape 3 : la surface du miroir par dessus le reflet */

    // dessiner le miroir en mode fusion
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    m_Mirror->onDrawSurface(m_Mat4Projection, mat4ModelViewMirror);
    glDisable(GL_BLEND);

    /** étape 4 : la scène réelle **/

    // dessiner la scène normale sans le miroir
    prepareLights(mat4View);
    drawDeferredShading(m_Mat4Projection, mat4View);
    addLightings();
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_Light1;
    delete m_Light0;
    delete m_Mirror;
    delete m_PalmTree;
    delete m_Lorry;
    delete m_Ground;
}
