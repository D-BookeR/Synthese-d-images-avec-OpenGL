#ifndef SCENE_H
#define SCENE_H

// Définition de la classe Scene

#include <gl-matrix.h>
#include <utils.h>
#include <TurnTableScene.h>
#include <OmniLight.h>
#include <MeshObject.h>



class Scene: public TurnTableScene
{
private:

    MeshObject* m_Lorry;
    MeshObject* m_PalmTree;
    MeshObject* m_Ground;

    OmniLight* m_Light0;
    OmniLight* m_Light1;
    OmniLight* m_Light2;

    // dimensions de la fenêtre
    int m_WindowWidth2;
    int m_WindowHeight;

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
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4View : matrice de vue
     */
    void onDraw(mat4& mat4Projection, mat4& mat4View);

    /** Dessine l'image courante */
    void onDrawFrame();

};

#endif
