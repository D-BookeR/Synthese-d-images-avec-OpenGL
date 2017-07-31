#ifndef LIGHTMATERIAL3_H3_H
#define LIGHTMATERIAL3_H3_H

// Définition de la classe LightMaterial3

#include <string>
#include <gl-matrix.h>
#include <utils.h>

#include <Material.h>
#include <VBOset.h>
#include <Texture2D.h>


class LightMaterial3: public Material
{
public:

    /**
     * constructeur
     * @param diffuse : fournir un vec3 définissant la couleur diffuse du matériau
     * @param specular : fournir un vec3 définissant la couleur spéculaire du matériau
     * @param shininess : aspect rugueux ou lisse
     */
    LightMaterial3(vec3 diffuse, vec3 specular, float shininess);

    /** destructeur */
    ~LightMaterial3();

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


protected:

    void compileShader();

    std::string getVertexShader();
    std::string getFragmentShader();


private:

    /** caractéristiques du matériau */
    vec3 m_DiffuseColor;
    vec3 m_SpecularColor;
    float m_Shininess;

    /** position des lampes */
    std::vector<vec3> m_LightsPositions;

    /** identifiants liés au shader */
    GLint m_LightsPositionsLoc;

};

#endif
