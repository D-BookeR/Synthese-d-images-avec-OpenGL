#ifndef SCENE_H
#define SCENE_H

// Définition de la classe Scene

#include <gl-matrix.h>
#include <utils.h>
#include <TurnTableScene.h>
#include <MeshObject.h>
#include <Light.h>
#include <OmniLight.h>
#include <SSAOLight.h>
#include <FrameBufferObject.h>


class Scene: public TurnTableScene
{
private:

    // objets de la scène
    MeshObject* m_Buildings;
    MeshObject* m_Ground;

    OmniLight* m_Light0;
    SSAOLight* m_Light1;


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
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4View : matrice de vue
     */
    void onDraw(mat4& mat4Projection, mat4& mat4View);

    /**
     * positionne et dessine les shadow maps des lampes
     * @param mat4View : matrice de vue de la lampe
     */
    void prepareLights(mat4& mat4View);

    /** Dessine l'image courante */
    void onDrawFrame();

};

#endif
