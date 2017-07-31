#ifndef SCENE_H
#define SCENE_H

// Définition de la classe Scene

#include <gl-matrix.h>
#include <utils.h>
#include <MeshObject.h>
#include <TurnTableScene.h>

#include "PhongMaterial.h"


class Scene: public TurnTableScene
{
private:

    /** objets */
    MeshObject* m_AppleB;
    MeshObject* m_RedApple;
    MeshObject* m_YellowApple;

    /** matériaux */
    PhongMaterial* m_BlinnMaterial;
    PhongMaterial* m_RedMaterial;
    PhongMaterial* m_YellowMaterial;

    /** position et couleur des lampes */
    std::vector<vec4> m_LightsPositions;
    std::vector<vec3> m_LightsColors;

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
