#ifndef SCENE_H
#define SCENE_H

// Définition de la classe Scene

#include <gl-matrix.h>
#include <utils.h>
#include <TurnTableScene.h>
#include <MeshObject.h>

#include "LightMaterial1.h"
#include "LightMaterial2.h"
#include "LightMaterial3.h"


class Scene: public TurnTableScene
{
private:

    MeshObject* m_Apple1;
    MeshObject* m_Apple2;
    MeshObject* m_Apple3;

    LightMaterial1* m_LightMaterial1;
    LightMaterial2* m_LightMaterial2;
    LightMaterial3* m_LightMaterial3;


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
