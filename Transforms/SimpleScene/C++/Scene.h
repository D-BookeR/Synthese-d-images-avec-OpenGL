#ifndef SCENE_H
#define SCENE_H

// Définition de la classe Scene

#include <gl-matrix.h>
#include <utils.h>
#include <SceneBase.h>
#include <SceneElement.h>
#include <GridXZ.h>

#include "Pyramid.h"


class Scene: public SceneBase
{
private:

    // objets de la scène
    Pyramid* m_Pyramid;
    GridXZ*  m_Grid;

    // matrices
    mat4 m_matModel1;
    mat4 m_matModel2;
    mat4 m_matView;
    mat4 m_matProjection;

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

};

#endif
