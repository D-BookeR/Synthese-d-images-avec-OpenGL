#ifndef SCENE_H
#define SCENE_H

// Définition de la classe Scene

#include <gl-matrix.h>
#include <utils.h>
#include <TurnTableScene.h>
#include <SceneElement.h>
#include <MeshObject.h>
#include <OmniLight.h>


class Scene: public TurnTableScene
{
private:

    // maillages de la scène
    MeshObject* m_Bee;
    MeshObject* m_Lorry;
    MeshObject* m_PalmTree;
    MeshObject* m_Ground;

    // lampes
    OmniLight* m_Light0;
    OmniLight* m_Light1;

    // éléments de l'arbre de scène
    SceneElement* m_ElementBee;
    SceneElement* m_ElementLorry;
    SceneElement* m_ElementPalmTree1;
    SceneElement* m_ElementPalmTree2;
    SceneElement* m_ElementPalmTree3;
    SceneElement* m_ElementPalmTree4;
    SceneElement* m_ElementPalmTree5;
    SceneElement* m_ElementGround;
    SceneElement* m_ElementLight0;
    SceneElement* m_ElementLight1;

    // élément attaché à la caméra
    SceneElement* ELEMENT_CAMERA;


public:

    /** constructeur, crée les objets 3D à dessiner */
    Scene();

    /** destructeur, libère les ressources */
    ~Scene();

    /**
     * appelée quand la taille de la vue OpenGL change
     * @param width : largeur en nombre de pixels de la fenêtre
     * @param height : hauteur en nombre de pixels de la fenêtre
     */
    void onSurfaceChanged(int width, int height);

    /**
     * dessin des objets de la scène sur l'écran
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    void onDraw(mat4& mat4Projection, mat4& mat4ModelView);

    /** Dessine l'image courante */
    void onDrawFrame();
};

#endif
