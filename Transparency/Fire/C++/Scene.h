#ifndef SCENE_H
#define SCENE_H

// Définition de la classe Scene

#include <vector>

#include <gl-matrix.h>
#include <utils.h>
#include <OmniLight.h>
#include <TurnTableScene.h>
#include <MeshObject.h>
#include <Texture2D.h>

#include "MultipleTextureMaterial.h"
#include "Billboard.h"


class Scene: public TurnTableScene
{
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

    /**
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    virtual void onDrawFrame();


private:

    MeshObject* m_Ground;
    MeshObject* m_DeadTree;
    Texture2D* m_Texture;
    MultipleTextureMaterial* m_Material;
    Billboard* m_Fire;
    OmniLight* m_Light0;

};

#endif
