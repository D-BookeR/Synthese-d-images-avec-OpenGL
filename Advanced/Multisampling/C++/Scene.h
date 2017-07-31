#ifndef SCENE_H
#define SCENE_H

// Définition de la classe Scene

#include <gl-matrix.h>
#include <utils.h>
#include <TurnTableScene.h>
#include <MeshObject.h>

#include "SubPicture.h"


class Scene: public TurnTableScene
{
private:

    // objets de la scène
    MeshObject* m_Lorry;
    MeshObject* m_PalmTree;
    MeshObject* m_Ground;

    // antialiasing actif si >1 ou non si =1
    int m_SamplesNumber;

    // taille de la fenêtre
    int m_Width;
    int m_Height;

    // pour afficher une vignette
    SubPicture* m_SubPicture;

    /** recréer le FBO avec multisampling ou pas */
    void recreateFBO();

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
     * appelée quand on appuie sur une touche du clavier
     * @param code : touche enfoncée
     */
    void onKeyDown(unsigned char code);

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
