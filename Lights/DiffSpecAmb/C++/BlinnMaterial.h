#ifndef BLINNMATERIAL_H
#define BLINNMATERIAL_H

// Définition de la classe BlinnMaterial

#include <string>
#include <gl-matrix.h>
#include <utils.h>

#include <Material.h>
#include <VBOset.h>
#include <Texture2D.h>


class BlinnMaterial: public Material
{
public:

    /**
     * constructeur
     * @param lights_count : nombre de lampes à gérer
     * @param ambiante : fournir un vec3 définissant la couleur ambiante du matériau
     * @param diffuse : fournir un vec3 définissant la couleur diffuse du matériau
     * @param specular : fournir un vec3 définissant la couleur spéculaire du matériau
     * @param shininess : aspect rugueux ou lisse
     */
    BlinnMaterial(int lights_count, vec3 ambiant, Texture2D* diffuse, vec3 specular, float shininess);

    /** destructeur */
    ~BlinnMaterial();

    /**
     * définit la position d'une lampe
     * @param num : numéro de la lampe
     * @param position : vec4 indiquant où elle se trouve dans l'espace caméra
     * @param color : vec3 indiquant sa couleur et son intensité
     */
    void setLightPositionColor(int num, vec4 position, vec3 color);

    /**
     * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
     * @return le VBOset du matériau
     */
    VBOset* createVBOset();

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

    /** caractéristiques du matériau */
    vec3 m_AmbiantColor;
    Texture2D* m_DiffuseTexture;
    vec3 m_SpecularColor;
    float m_Shininess;

    /** position et couleur des lampes */
    std::vector<vec4> m_LightsPositions;
    std::vector<vec3> m_LightsColors;

    /** identifiants liés au shader */
    GLint m_LightsPositionsLoc;
    GLint m_LightsColorsLoc;
    GLint m_TextureLoc;
};

#endif
