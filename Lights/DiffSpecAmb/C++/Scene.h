#ifndef SCENE_H
#define SCENE_H

// Définition de la classe Scene

#include <gl-matrix.h>
#include <utils.h>
#include <TurnTableScene.h>
#include <Texture2D.h>
#include <MeshObject.h>

#include "AmbiantMaterial.h"
#include "DiffuseMaterial.h"
#include "SpecularMaterial.h"
#include "BlinnMaterial.h"


class Scene: public TurnTableScene
{
private:

    /** objets */
    MeshObject* m_AppleAmbiant;
    MeshObject* m_AppleDiffuse;
    MeshObject* m_SpecularApple;
    MeshObject* m_AppleBlinn;

    /** matériaux */
    AmbiantMaterial*  m_AmbiantMaterial;
    DiffuseMaterial*  m_DiffuseMaterial;
    SpecularMaterial* m_SpecularMaterial;
    BlinnMaterial*    m_BlinnMaterial;
    Texture2D*        m_Texture;

    /** position et couleur des lampes */
    std::vector<vec4> m_LightsPositions;
    std::vector<vec3> m_LightsColors;

    // taille des sous-fenêtres : 1/2 de la fenêtre principale
    GLint m_WindowWidth2;
    GLint m_WindowHeight2;

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
