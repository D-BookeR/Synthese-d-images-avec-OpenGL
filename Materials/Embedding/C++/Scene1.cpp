#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

#include <utils.h>
#include <Scene1.h>



/**
 * Constructeur
 */
Scene1::Scene1()
{
    // créer le tétraèdre qui est dessiné par cette scène
    m_Tetraedre = new Tetraedre();

    // paramètres de la vue
    m_Mat4Projection = mat4::create();
    m_CameraDistance = 13;
}


void Scene1::onSurfaceChanged(int width, int height)
{
    // met en place le viewport
    glViewport(0, 0, width, height);

    // matrice de projection perspective
    mat4::perspective(m_Mat4Projection, Utils::radians(8.0), (float)width / height, 0.1, 20.0);
}


/**
 * dessine les objets de la scène
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Scene1::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
{
    // dessiner l'objet
    m_Tetraedre->onDraw(mat4Projection, mat4ModelView);
}


/**
 * dessine la scène
 */
void Scene1::onDrawFrame()
{
    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // effacer l'écran avec une couleur grise claire
    glClearColor(0.9, 0.9, 0.9, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // construire la matrice MV
    mat4 mat4ModelView = mat4::create();

    // éloignement de la scène
    mat4::translate(mat4ModelView, mat4ModelView, vec3::fromValues(0.0, 0.0, -m_CameraDistance));

    // rotation du tétraèdre
    mat4::rotate(mat4ModelView, mat4ModelView, Utils::radians(25.0 * Utils::Time + 60.0), vec3::fromValues(1,1,0));

    onDraw(m_Mat4Projection, mat4ModelView);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene1::~Scene1()
{
    delete m_Tetraedre;
}
