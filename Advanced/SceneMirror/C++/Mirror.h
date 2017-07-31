#ifndef MIROIR_H
#define MIROIR_H

// Définition de la classe Mirror

#include <gl-matrix.h>
#include <Mesh.h>
#include <ColorMaterial.h>
#include <VBOset.h>


/**
 * Classe Mirror = encadrement et surface réfléchissante
 */
class Mirror
{
public:

    Mirror();
    ~Mirror();

    /**
     * dessine le cadre du miroir
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    void onDrawFrame(mat4& mat4Projection, mat4& mat4ModelView);

    /**
     * dessine la surface du miroir
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    void onDrawSurface(mat4& mat4Projection, mat4& mat4ModelView);

    /**
     * définit un plan de coupe pour les fragments. Ce plan est en coordonnées caméra
     * @param active : true s'il faut compiler un shader gérant le plan de coupe
     * @param plane vec4 en coordonnées caméra ou null pour supprimer le plan de coupe
     */
    void setClipPlane(bool active, vec4 plane);

    /**
     * supprime un plan de coupe pour les fragments.
     * @param active : true s'il faut garder le shader gérant le plan de coupe
     */
    void setClipPlane(bool active);


private:

    /**
     * ajout d'une face (a,b,c,d) au maillage du cadre
     * @param name : nom de la face à créer
     * @param normal : vec3 à affecter aux 4 sommets
     * @param a : coordonnées
     * @param b : coordonnées
     * @param c : coordonnées
     * @param d : coordonnées
     */
    void addFrameQuad(std::string name, vec3 normal, vec3& a, vec3& b, vec3& c, vec3& d);


private:

    // partie miroir
    Mesh* m_MirrorMesh;
    VBOset* m_VBOsetMirror;
    ColorMaterial* m_MirrorMaterial;

    // partie cadre
    Mesh* m_FrameMesh;
    VBOset* m_VBOsetFrame;
    ColorMaterial* m_FrameMaterial;

};

#endif
