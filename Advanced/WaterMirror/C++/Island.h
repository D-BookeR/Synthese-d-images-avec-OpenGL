#ifndef ISLAND_H
#define ISLAND_H

// Définition de la classe Island

#include <vector>
#include <map>
#include <list>

#include <gl-matrix.h>
#include <Mesh.h>
#include <VBOset.h>

#include "IslandMaterial.h"


/**
 * Classe Island.
 */
class Island
{
public:

    /**
     * constructeur
     * @param points_count : nombre de points à créer en X et en Z
     * @param scale facteur de taille du maillage
     * @param hauteur position verticale du maillage
     * @param profondeur position verticale du maillage
     */
    Island(int points_count, float scale, float height, float depth);

    /** destructeur, libère le VBOset, le matériau et le maillage */
    ~Island();

    /**
     * définit un plan de coupe pour les fragments. Ce plan est en coordonnées caméra
     * @param actif : true s'il faut compiler un shader gérant le plan de coupe
     * @param plane vec4 en coordonnées caméra ou null pour supprimer le plan de coupe
     */
    void setClipPlane(bool actif, vec4 plane);

    /**
     * enlève le plan de coupe mais sans recompiler le shader (le plan est mis à l'infini)
     * @param actif : true s'il faut garder le code gérant le plan de coupe dans le shader
     */
    void setClipPlane(bool actif);

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

    // matériau
    IslandMaterial* m_Material;

    // VBOset
    VBOset* m_VBOset;

    // dimensions
    float m_Scale;
    float m_Depth;

};

#endif
