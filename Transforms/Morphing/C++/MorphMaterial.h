#ifndef MORPHMATERIAL_H
#define MORPHMATERIAL_H

// Définition de la classe MorphMaterial, une spécialisation de Material
// Ce matériau permet d'appliquer une déformation par cible
// C'est une simplification de DeferredShadingMaterial

#include <sstream>

#include <gl-matrix.h>
#include <utils.h>
#include <Material.h>
#include <Texture2D.h>
#include <VBOset.h>


class MorphMaterial: public Material
{
public:

    /**
     * constructeur
     * @param diffuse : nom d'une texture
     * @param Ks : vec3
     * @param Ns : poli du matériau
     */
    MorphMaterial(std::string diffuse, vec3 Ks, float Ns);


    /** destructeur */
    virtual ~MorphMaterial();

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

    /**
     * définit la valeur du morph pour changer la forme de l'objet
     * @param coef entre 0 et 1
     */
    void setMorphCoef(float coef);


protected:

    /** initialise les variables membres */
    void init();

    /** recompile le shader du matériau */
    virtual void compileShader();

    std::string getVertexShader();
    virtual std::string getFragmentShader();

    /// paramètres de construction des shaders
    vec3 m_Ks;
    float m_Ns;
    float m_MorphCoef;


    /** textures utilisées par le matériau **/
    Texture2D* m_TxDiffuse;

    /** identifiants liés au shader */
    GLint m_TxDiffuseLoc;
    GLint m_MorphCoefLoc;
};

#endif
