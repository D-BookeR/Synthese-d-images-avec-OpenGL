#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

#include <utils.h>
#include <Rectangle.h>
#include "Scene.h"



/**
 * Constructeur
 */
Scene::Scene() : TurnTableScene(false)
{
    // créer les deux textures, l'une avec mipmaps, l'autre sans
    // pour filtrage, essayer
    //      GL_NEAREST
    //      GL_LINEAR
    //      GL_NEAREST_MIPMAP_NEAREST
    //      GL_LINEAR_MIPMAP_NEAREST
    //      GL_NEAREST_MIPMAP_LINEAR
    //      GL_LINEAR_MIPMAP_LINEAR
    // voir https://www.openGL_org/wiki/GLAPI/glTexParameter à GL_TEXTURE_MIN_FILTER
    m_TextureAvec = new Texture2D("data/textures/sols/4016.jpg", GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT);
    m_TextureWithout = new Texture2D("data/textures/sols/4016.jpg", GL_LINEAR, GL_REPEAT);

    m_MaterialWith = new TextureMaterial(m_TextureAvec);
    m_MaterialWithout = new TextureMaterial(m_TextureWithout);

    // créer deux rectangles, chacun son matériau
    m_ObjectWith = new Rectangle(m_MaterialWith);
    m_ObjectWithout = new Rectangle(m_MaterialWithout);

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);      // laisser voir le dos du rectangle

    // couleur du fond
    glClearColor(0.4, 0.4, 0.4, 0.0);

    // paramètres de la vue
    m_CameraAngleAzimut = 0.0;
    m_CameraAngleElevation = 6.0;
    m_CameraDistance = 15;
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
    mat4::perspective(m_Mat4Projection, Utils::radians(10.0), (float)width / height, 0.1, 200.0);
}


/**
 * dessine les objets de la scène
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Scene::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
{
    // dessiner les rectangles côte à côte
    mat4 mat4ModelViewTmp = mat4::create();

    mat4::translate(mat4ModelViewTmp, mat4ModelView, vec3::fromValues(-100.1,-2,0));
    m_ObjectWith->onDraw(mat4Projection, mat4ModelViewTmp);

    mat4::translate(mat4ModelViewTmp, mat4ModelView, vec3::fromValues(+100.1,-2,0));
    m_ObjectWithout->onDraw(mat4Projection, mat4ModelViewTmp);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_ObjectWithout;
    delete m_ObjectWith;
    delete m_MaterialWithout;
    delete m_MaterialWith;
    delete m_TextureWithout;
    delete m_TextureAvec;
}
