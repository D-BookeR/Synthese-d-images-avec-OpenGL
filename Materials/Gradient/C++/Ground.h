#ifndef TERRAIN_H
#define TERRAIN_H

// Définition de la classe Ground

#include <vector>
#include <map>
#include <list>

#include <gl-matrix.h>
#include <Mesh.h>
#include <VBOset.h>

#include "GroundMaterial.h"


/**
 * Classe Ground.
 */
class Ground
{
public:

    /**
     * constructeur
     * @param points_count : nombre de points à créer en X et en Z
     * @param materiau : matériau déformant à employer
     */
    Ground(int points_count, GroundMaterial* material);

    /** destructeur, libère le VBOset et le maillage */
    ~Ground();

    /**
     * dessine l'objet sur l'écran
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    void onDraw(mat4 mat4Projection, mat4 mat4ModelView);


private:

    /**
     * Cette méthode retourne les coordonnées 3D d'un point identifié par (ix, iz)
     * @param points_count : nombre de points en X et en Z
     * @param ix : numéro du point en X
     * @param iz : numéro du point en Z
     * @return vec3 contenant les coordonnées
     */
    vec3 getCoord(int points_count, int ix, int iz);

    /**
     * Cette méthode retourne les coordonnées de texture d'un point identifié par (ix, iz)
     * @param points_count : nombre de points en X et en Z
     * @param ix : numéro du point en X
     * @param iz : numéro du point en Z
     * @return vec3 contenant les coordonnées
     */
    vec2 getTexCoord(int points_count, int ix, int iz);

private:

    // maillage
    Mesh* m_Mesh;

    // VBOset
    VBOset* m_VBOset;


};

#endif
