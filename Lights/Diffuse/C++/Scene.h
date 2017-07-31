#ifndef SCENE_H
#define SCENE_H

// Définition de la classe Scene

#include <gl-matrix.h>
#include <utils.h>
#include <TurnTableScene.h>
#include <MeshObject.h>

#include "LambertMaterial.h"
#include "MinnaertMaterial.h"
#include "OrenNayarMaterial.h"


class Scene: public TurnTableScene
{
private:

    /** objets */
    MeshObject* m_LambertApple;
    MeshObject* m_OrenNayarApple;
    MeshObject* m_MinnaertApple;

    /** matériaux */
    LambertMaterial* m_LambertMaterial;
    OrenNayarMaterial* m_OrenNayarMaterial;
    MinnaertMaterial* m_MinnaertMaterial;

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
