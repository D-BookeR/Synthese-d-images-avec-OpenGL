#ifndef SCENE_H
#define SCENE_H

// Définition de la classe Scene

#include <gl-matrix.h>
#include <utils.h>
#include <TurnTableScene.h>
#include <ShadowMap.h>
#include <Cube.h>
#include <Plane.h>

#include "ShadowMaterial.h"


class Scene: public TurnTableScene
{
private:

    // matériaux pour les objets
    ShadowMaterial* m_PlaneMaterial;
    ShadowMaterial* m_CubeMaterial;

    // objets de la scène
    Plane* m_Plane;
    Cube* m_Cube;

    // shadow map et matrice
    ShadowMap* m_ShadowMap;
    mat4 m_ShadowMatrix;

    // infos sur la lampe
    float m_MaxAngle;
    float m_Near;
    float m_Far;
    vec3 m_Position;
    vec3 m_Target;
    vec3 m_PositionCamera;
    vec3 m_TargetCamera;


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
     * dessine la scène dans la shadowmap de cette lampe
     * @param mat4ViewCamera : matrice de transformation dans laquelle sont dessinés les objets
     */
    void makeShadowMap(mat4& mat4ViewCamera);

    /** Dessine l'image courante */
    void onDrawFrame();

};

#endif
