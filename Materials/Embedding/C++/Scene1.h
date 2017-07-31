#ifndef SCENE1_H1_H
#define SCENE1_H1_H

// Définition de la classe Scene1

#include <gl-matrix.h>
#include <utils.h>
#include <SceneBase.h>

#include "Tetraedre.h"


class Scene1
{
private:

    Tetraedre* m_Tetraedre;

    float m_CameraDistance;
    mat4 m_Mat4Projection;

public:

    /** constructeur, crée les objets 3D à dessiner */
    Scene1();

    /** destructeur, libère les ressources */
    ~Scene1();

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

    /** dessin de la scène entière sur l'écran */
    void onDrawFrame();

};

#endif
