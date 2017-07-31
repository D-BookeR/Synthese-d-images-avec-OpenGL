#ifndef SHADOWMATERIAL_H
#define SHADOWMATERIAL_H

// Définition de la classe ShadowMaterial

#include <string>
#include <gl-matrix.h>
#include <utils.h>

#include <Material.h>
#include <ShadowMap.h>
#include <Texture2D.h>
#include <VBOset.h>



class ShadowMaterial: public Material
{
public:

    /**
     * constructeur
     * @param color du matériau
     */
    ShadowMaterial(vec4 color);

    /** destructeur */
    ~ShadowMaterial();

    /**
     * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
     * @return le VBOset du matériau
     */
    VBOset* createVBOset();

    /**
     * fournit la shadow map et sa matrice
     * @param shadowmatrix à utiliser
     * @param positioncamera position de la lampe dans le repère caméra
     * @param shadowmap à utiliser pour les ombres, null si désactivée (pour le dessin dans elle-même)
     */
    void setShadowMap(mat4& shadowmatrix, vec3& positioncamera, ShadowMap* shadowmap=nullptr);

    /**
     * Cette méthode active le matériau
     * @param mat4Projection : fournir la matrice de projection
     * @param mat4ModelView : fournir la matrice de vue
     */
    void enable(mat4 mat4Projection, mat4 mat4ModelView);

    /**
     * Cette méthode désactive le matériau
     */
    void disable();


protected:

    void compileShader();

    std::string getVertexShader();
    std::string getFragmentShader();


private:

    // variables membres
    vec4 m_Color;
    ShadowMap* m_ShadowMap;
    mat4 m_ShadowMatrix;
    vec3 m_PositionCamera;

    // identifiants liés au shader
    int m_ShadowMapLoc;
    int m_ShadowMatrixLoc;
    int m_LightPositionLoc;

};

#endif
