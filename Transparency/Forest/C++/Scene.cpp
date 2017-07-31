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
    // crée les objets à dessiner
    m_Ground = new Ground();

    // créer un ensemble de billboards
    m_TxTree = new Texture360("data/textures/Billboards/bb_arbre2", 8);
    const int N = 13;
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.0, 1.0);
    for (int l=0; l<N; l++) {
        for (int c=0; c<N; c++) {
            // générer une taille variant de 20% autour de 1.0
            float sx = 0.8 + distribution(generator)*0.4;
            float sy = 0.8 + distribution(generator)*0.4;

            // la position de l'arbre varie un peu autour de (x=c, y=0, z=l)
            float x = c*(9.0/(N-1)) - 4.5  + (distribution(generator)-0.5)*0.003;
            float y = 0.0;
            float z = l*(9.0/(N-1)) - 4.5 +  (distribution(generator)-0.5)*0.003;

            // créer et mémoriser un billboard à cet emplacement
            Billboard* tree = new Billboard(m_TxTree, vec3::fromValues(x,y,z), sx, sy);

            // ajouter ce nouvel arbre dans la forêt
            m_Forest.push_back(tree);
        }
    }

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);      // laisser voir le dos des billboards

    // formule de blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // couleur du fond
    glClearColor(0.6, 0.7, 1.0, 0.0);

    // paramètres de la vue
    m_CameraAngleAzimut = 40.0;
    m_CameraAngleElevation = 11.0;
    m_CameraDistance = 9;
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
    mat4::perspective(m_Mat4Projection, Utils::radians(50.0), (float)width / height, 0.1, 80.0);
}


/**
 * dessine les objets de la scène
 * @param mat4Projection : matrice de projection
 * @param mat4View : matrice de vue
 */
void Scene::onDraw(mat4& mat4Projection, mat4& mat4View)
{
    // dessiner le terrain
    m_Ground->onDraw(mat4Projection, mat4View);

    // positionner chaque arbre
    for (Billboard* tree: m_Forest) {
        // positionner l'arbre => récupérer sa matrice ModelView pour avoir sa distance
        tree->setModelView(mat4View);
    }

    // classer par distance décroissante à l'oeil
    std::sort(m_Forest.begin(), m_Forest.end(), Billboard::DistanceCompare);

    // dessiner l'ensemble des arbres
    for (Billboard* tree: m_Forest) {
        tree->onDraw(mat4Projection);
    }
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_Ground;
    for (Billboard* tree: m_Forest) {
        delete tree;
    }
    delete m_TxTree;
}
