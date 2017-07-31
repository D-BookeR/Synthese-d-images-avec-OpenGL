#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>

#include <utils.h>
#include <MeshObjectFromObj.h>

#include "Scene.h"
#include <SoftSpotLight.h>


/**
 * Constructeur
 */
Scene::Scene() : TurnTableScene(false)
{
    // créer les objets à dessiner
    m_GridXZ   = new GridXZ(3);
    m_TriedreXYZ = new AxesXYZ(1,1,4);
    m_Lorry  = new MeshObjectFromObj("data/models/Camion", "camion.obj", "camion.mtl", 2.0);
    m_Mirror = new Mirror();

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // couleur du fond
    glClearColor(0.7, 0.7, 0.7, 0.0);

    // paramètres de la vue
    m_CameraAngleAzimut = -17.0;
    m_CameraAngleElevation = 8.0;
    vec3::set(m_CameraPivot, 0.9, -0.6, -0.6);
    m_CameraDistance = 25;
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

    // dessiner le trièdre
    m_TriedreXYZ->onDraw(mat4Projection, mat4View);

    // décaler tous les objets en z positif
    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(0.0, 0.0, 4.0));

    // dessiner la grille
    m_GridXZ->onDraw(mat4Projection, mat4ModelView);

    // dessiner le camion
    mat4::translate(mat4ModelView, mat4ModelView, vec3::fromValues(0.0, 0.0, 0.0));        // ESSAYEZ DE METTRE -7 POUR z
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(120.0 * cos(Utils::Time)));
    m_Lorry->onDraw(mat4Projection, mat4ModelView);
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
    mat4::translate(mat4ModelMirror, mat4ModelMirror, vec3::fromValues(0.0, 0.0, -1.0));  // ESSAYEZ D'AUTRES VALEURS
    //mat4::rotateY(mat4ModelMirror, mat4ModelMirror, Utils::radians(20.0));              // ESSAYEZ D'AUTRES VALEURS

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
    vec4 MirrorPlane = vec4::fromValues(0, 0, -1, 0);

    // transformer le plan de coupe en coordonnées caméra
    mat4 mat4ModelViewMirrorInverse = mat4::create();
    mat4::transpose(mat4ModelViewMirrorInverse, mat4ModelViewMirror);
    mat4::invert(mat4ModelViewMirrorInverse, mat4ModelViewMirrorInverse);
    vec4::transformMat4(MirrorPlane, MirrorPlane, mat4ModelViewMirrorInverse);


    /*** DESSIN ***/

    // effacer l'écran
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /** étape 1 : la découpe du stencil par le miroir **/

    // configurer le stencil en mode "mettre le buffer à 1"
    glEnable(GL_STENCIL_TEST);                             // activation du Stencil Buffer
    glStencilFunc(GL_ALWAYS, 1, 0xFF);                     // accepter tous les fragments, la constante = 1
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);             // mettre la constante, c'est à dire 1 si le depth test passe
    glClearStencil(0);                                     // mettre des 0 partout quand on vide le stencil
    glClear(GL_STENCIL_BUFFER_BIT);                        // vider le stencil

    // bloquer les dessins sur l'écran et le depth buffer
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);   // ne pas dessiner de pixels
    glDepthMask(GL_FALSE);                                 // ne pas écrire sur le depth buffer

    // dessiner le miroir (il n'apparaît pas sur l'écran)
    m_Mirror->onDraw(m_Mat4Projection, mat4ModelViewMirror);

    // réautoriser les dessins sur l'écran et le depth buffer
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);       // réactiver le dessin sur l'écran
    glDepthMask(GL_TRUE);                                  // réactiver l'écriture sur le depth buffer

    /** étape 2 : le reflet à travers la découpe **/

    // configurer le stencil en mode "ne dessiner que si le stencil est égal à 1"
    glStencilFunc(GL_EQUAL, 1, 0xFF);                      // accepter les fragments pour lesquels le stencil vaut 1
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);                // ne pas changer le stencil

    // fournir le plan de coupe au camion
//    m_Lorry->setClipPlane(true, MirrorPlane);

    // dessiner les objets inversés, avec la matrice de la scène reflétée
    glFrontFace(GL_CW);
    onDraw(m_Mat4Projection, mat4ViewReflected);
    glFrontFace(GL_CCW);

    // enlever le plan de coupe sans recompiler le shader
//    m_Lorry->setClipPlane(true);

    // ne plus utiliser le stencil
    glDisable(GL_STENCIL_TEST);

    /** étape 3 : la surface du miroir par dessus le reflet */

    // superposer le miroir quasi-transparent
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    m_Mirror->onDraw(m_Mat4Projection, mat4ModelViewMirror);
    glDisable(GL_BLEND);

    /** étape 4 : la scène réelle **/

    // dessiner la scène normale sans le miroir
    onDraw(m_Mat4Projection, mat4View);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_Mirror;
    delete m_Lorry;
    delete m_TriedreXYZ;
    delete m_GridXZ;
}
