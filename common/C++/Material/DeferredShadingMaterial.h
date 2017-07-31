#ifndef MATERIAL_DEFERREDSHADINGMATERIAL_H
#define MATERIAL_DEFERREDSHADINGMATERIAL_H

// Définition de la classe DeferredShadingMaterial

#include <sstream>

#include <gl-matrix.h>
#include <utils.h>
#include <VBOset.h>
#include <Material.h>
#include <Texture2D.h>


class DeferredShadingMaterial: public Material
{
public:

    /**
     * constructeur
     * @param Kd : vec3
     */
    DeferredShadingMaterial(vec4 Kd);

    /**
     * constructeur
     * @param Ks : vec3
     * @param Ns : poli du matériau
     */
    DeferredShadingMaterial(vec3 Ks, float Ns);

    /**
     * constructeur
     * @param Kd : vec3
     * @param Ks : vec3
     * @param Ns : poli du matériau
     */
    DeferredShadingMaterial(vec4 Kd, vec3 Ks, float Ns);

    /**
     * constructeur
     * @param diffuse : nom d'une texture
     * @param Ks : vec3
     * @param Ns : poli du matériau
     */
    DeferredShadingMaterial(std::string diffuse, vec3 Ks, float Ns);


    /** destructeur */
    virtual ~DeferredShadingMaterial();

    /**
     * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
     * @return le VBOset du matériau
     */
    virtual VBOset* createVBOset();

    /**
     * Cette méthode active le matériau
     * @param mat4Projection : fournir la matrice de projection
     * @param mat4ModelView : fournir la matrice de vue
     */
    virtual void enable(mat4 mat4Projection, mat4 mat4ModelView);

    /**
     * Cette méthode désactive le matériau
     */
    virtual void disable();


protected:

    /** initialise les variables membres */
    void init();

    /** recompile le shader du matériau */
    virtual void compileShader();

    std::string getVertexShader();
    virtual std::string getFragmentShader();

    /// paramètres de construction des shaders
    bool m_KdIsInterpolated;
    vec4 m_Kd;
    vec3 m_Ks;
    float m_Ns;


protected:

    /** textures utilisées par le matériau **/
    Texture2D* m_TxDiffuse;
    Texture2D* m_TxSpecular;

    /** identifiants liés au shader */
    GLint m_TxDiffuseLoc;
    GLint m_TxSpecularLoc;
};

#endif
