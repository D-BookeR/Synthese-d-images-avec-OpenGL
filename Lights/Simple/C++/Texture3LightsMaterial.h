#ifndef TEXTURE3LIGHTSMATERIAL_H3LAMPES_H
#define TEXTURE3LIGHTSMATERIAL_H3LAMPES_H

// Définition de la classe Texture3LightsMaterial

#include <string>
#include <gl-matrix.h>
#include <utils.h>

#include <Material.h>
#include <VBOset.h>
#include <Texture2D.h>


class Texture3LightsMaterial: public Material
{
public:

    /**
     * constructeur
     * @param texture : fournir un vec3 définissant la couleur diffuse du matériau
     * @param specular : fournir un vec3 définissant la couleur spéculaire du matériau
     * @param shininess : aspect rugueux ou lisse
     */
    Texture3LightsMaterial(Texture2D* texture, vec3 specular, float shininess);

    /** destructeur */
    ~Texture3LightsMaterial();

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
    Texture2D* m_Texture;
    vec3 m_SpecularColor;
    float m_Shininess;

    /** position et couleur des lampes */
    std::vector<vec3> m_LightsPositions;
    std::vector<vec3> m_LightsColors;

    /** identifiants liés au shader */
    GLint m_LightsPositionsLoc;
    GLint m_LightsColorsLoc;
    GLint m_TextureLoc;

};

#endif
