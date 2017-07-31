#ifndef SCENE_H
#define SCENE_H

// Définition de la classe Scene

#include <gl-matrix.h>
#include <utils.h>
#include <TurnTableScene.h>
#include <MeshObject.h>
#include <SkyBackground.h>
#include <OmniLight.h>
#include <FrameBufferObject.h>

#include "TransparentMaterial.h"


class Scene: public TurnTableScene
{
private:

    // objets de la scène
    MeshObject* m_Ground;
    MeshObject* m_PalmTree;
    MeshObject* m_Lorry;
    MeshObject* m_Apple;

    // matériaux spéciaux
    TransparentMaterial* m_TransparentMaterial;

    // FBO nécessaires
    FrameBufferObject* m_PreviousGBuffer;
    FrameBufferObject* m_CurrentGBuffer;
    FrameBufferObject* m_FBOlayers;

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
     * dessine les objets opaques de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4View : matrice de vue
     */
    void onDraw(mat4& mat4Projection, mat4& mat4View);

    /**
     * dessine les objets transparents de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4View : matrice de vue
     */
    void onDrawTranspa(mat4& mat4Projection, mat4& mat4View);

    /** Dessine l'image courante */
    void onDrawFrame();

};

#endif
