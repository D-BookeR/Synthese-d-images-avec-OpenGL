#ifndef SCENE_H
#define SCENE_H

// Définition de la classe Scene

#include <gl-matrix.h>
#include <utils.h>
#include <TrackBallScene.h>
#include <MeshStructureDraw.h>
#include <Material.h>
#include <OmniLight.h>

#include "Wave.h"


class Scene: public TrackBallScene
{
private:

    Wave* m_Object;
    Material* m_Material;
    MeshStructureDraw* m_MeshStructureDraw;
    OmniLight* m_Light0;
    OmniLight* m_Light1;
    OmniLight* m_Light2;


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
     * appelée quand on appuie sur une touche du clavier
     * @param code : touche enfoncée
     */
    void onKeyDown(unsigned char code);

    /**
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4View : matrice de vue
     */
    void onDraw(mat4& mat4Projection, mat4& mat4View);
};

#endif
