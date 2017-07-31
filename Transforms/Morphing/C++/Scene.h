#ifndef SCENE_H
#define SCENE_H

// Définition de la classe Scene

#include <gl-matrix.h>
#include <utils.h>
#include <TurnTableScene.h>
#include <OmniLight.h>
#include <SpotLight.h>
#include <MeshObject.h>
#include <SceneElement.h>

#include "Pumpkin.h"


class Scene: public TurnTableScene
{
private:

    // objets de la scène
    MeshObject* m_Ground;
    MeshObject* m_DeadTree;
    Pumpkin* m_Pumpkin1;
    Pumpkin* m_Pumpkin2;
    Pumpkin* m_Pumpkin3;

    // lampes
    SpotLight* m_PumpkinSpotLight1;
    OmniLight* m_PumpkinOmniLight1;
    SpotLight* m_PumpkinSpotLight2;
    OmniLight* m_PumpkinOmniLight2;
    SpotLight* m_PumpkinSpotLight3;
    OmniLight* m_PumpkinOmniLight3;

    // éléments 3D (hiérarchie)
    SceneElement* m_GroundElement;
    SceneElement* m_DeadTreeElement1;
    SceneElement* m_DeadTreeElement2;
    SceneElement* m_DeadTreeElement3;
    SceneElement* m_PumpkinElement1;
    SceneElement* m_PumpkinSpotLightElement1;
    SceneElement* m_PumpkinOmniLightElement1;
    SceneElement* m_PumpkinElement2;
    SceneElement* m_PumpkinSpotLightElement2;
    SceneElement* m_PumpkinOmniLightElement2;
    SceneElement* m_PumpkinElement3;
    SceneElement* m_PumpkinSpotLightElement3;
    SceneElement* m_PumpkinOmniLightElement3;


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
