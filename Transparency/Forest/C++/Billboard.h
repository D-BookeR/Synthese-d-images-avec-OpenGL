#ifndef BILLBOARD_H
#define BILLBOARD_H

// Définition de la classe Billboard

#include <string>
#include <vector>

#include <gl-matrix.h>
#include <utils.h>
#include <Mesh.h>
#include <VBOset.h>

#include "Texture360.h"
#include "Texture360Material.h"


class Billboard
{
public:

    /**
     * crée un billboard : un rectangle portant une texture360
     * @param texture360
     * @param position : vec3 donnant les coordonnées du bas du panneau
     * @param tx : float donnant la taille horizontale
     * @param ty : float donnant la taille verticale
     */
    Billboard(Texture360* texture360, vec3 position, float tx=1.0, float ty=1.0);

    /** destructeur */
    ~Billboard();

    /**
     * recopie ModelView dans les matrices locales
     * @param mat4ModelView
     */
    void setModelView(mat4& mat4ModelView);

    /**
     * retourne la distance du billboard à l'œil
     * @return distance
     */
    float getDistance() const;

    /**
     * compare la distance entre deux billboard afin de pouvoir classer une liste
     * du plus loin au plus proche
     */
    static bool DistanceCompare(const Billboard* a, const Billboard* b);

    /**
     * dessin du billboard sur l'écran
     * @param mat4Projection : matrice de projection
     */
    void onDraw(mat4& mat4Projection);

    /**
     * dessin du billboard sur l'écran, variante plus simple, sans positionnement préalable
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    void onDraw2(mat4& mat4Projection, mat4& mat4ModelView);


protected:

    // position et dimensions
    vec3 m_Position;
    float m_SizeX;
    float m_SizeY;

    // matrices ModelView associées
    mat4 m_ModelViewOrig;
    mat4 m_ModelViewFixed;

    // angle et distance de vue
    float m_Angle;
    float m_Distance;

    // maillage
    Mesh* m_Mesh;

    // matériau
    Texture360Material* m_Material;

    // VBOset
    VBOset* m_VBOset;
};

#endif
