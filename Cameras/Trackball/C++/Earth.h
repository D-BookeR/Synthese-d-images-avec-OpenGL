#ifndef EARTH_H
#define EARTH_H

// Définition de la classe Earth

#include <vector>
#include <gl-matrix.h>

#include <Mesh.h>
#include <Material.h>
#include <VBOset.h>

/**
 * Classe Earth.
 */
class Earth
{
public:

    /**
     * constructeur, crée le maillage
     * @param nbLon : nombre de divisions en longitude
     * @param nbLat : nombre de divisions en latitude
     */
    Earth(int nbLon, int nbLat);

    /** destructeur, libère le maillage */
    ~Earth();

    void onDraw(mat4 mat4Projection, mat4 mat4ModelView);

private:

    // maillage
    Mesh* m_Mesh;

    // matériau
    Material* m_Material;

    // VBOset
    VBOset* m_VBOset;

private:

    /**
     * Cette méthode retourne les coordonnées 3D d'un point identifié par (a,b)
     * @param a : longitude variant entre 0 et 1 en tournant autour du centre
     * @param b : latitude variant entre 0 (pôle sud) et 1 (pôle nord)
     * @return vec3 contenant les coordonnées
     */
    vec3 getVertex(float a, float b);
};

#endif
