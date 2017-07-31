#ifndef SPHERE_H
#define SPHERE_H

// Définition de la classe Sphere

#include <vector>

#include <gl-matrix.h>
#include <Texture2D.h>
#include <Mesh.h>
#include <VBOset.h>

#include "TransparentMaterial.h"


/**
 * Classe Sphere.
 */
class Sphere
{
public:

    /**
     * constructeur, crée le maillage
     * @param nbLon : nombre de divisions en longitude
     * @param nbLat : nombre de divisions en latitude
     */
    Sphere(int nbLon, int nbLat, Texture2D* texture);

    /** destructeur, libère le maillage */
    ~Sphere();

    /**
     * affecte la texture DepthMap du shader
     * @param far : depth buffer qui limite les dessins arrière de ce matériau
     * @param near : depth buffer qui limite les dessins avant de ce matériau
     */
    void setDepthMaps(GLint far, GLint near);

    /**
     * spécifie la taille de la fenêtre
     * @param width : largeur en nombre de pixels de la fenêtre
     * @param height : hauteur en nombre de pixels de la fenêtre
     */
    void setWindowDimensions(int width, int height);

    /**
     * dessine l'objet sur l'écran
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    void onDraw(mat4& mat4Projection, mat4& mat4ModelView);


private:

    Mesh* m_Mesh;
    TransparentMaterial* m_Material;
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
