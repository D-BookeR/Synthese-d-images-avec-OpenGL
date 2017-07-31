#ifndef LIGHTMATERIAL1_H1_H
#define LIGHTMATERIAL1_H1_H

// Définition de la classe LightMaterial1

#include <string>
#include <gl-matrix.h>
#include <utils.h>

#include <Material.h>
#include <VBOset.h>
#include <Texture2D.h>


class LightMaterial1: public Material
{
public:

    /**
     * constructeur
     * @param color : fournir un vec3 définissant la couleur diffuse du matériau
     */
    LightMaterial1(vec3 color);

    /** destructeur */
    ~LightMaterial1();

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

    /** couleur du matériau */
    vec3 m_Color;

};

#endif
