#ifndef MATERIAL_COLORMATERIAL_H
#define MATERIAL_COLORMATERIAL_H

// Définition de la classe ColorMaterial

#include <sstream>

#include <gl-matrix.h>
#include <utils.h>

class ColorMaterial;

#include <VBOset.h>
#include <Material.h>
#include <Texture2D.h>


class ColorMaterial: public Material
{
public:

    /**
     * constructeur
     */
    ColorMaterial();

    /**
     * constructeur
     * @param color du matériau
     */
    ColorMaterial(vec4 color);


    /** destructeur */
    virtual ~ColorMaterial();

    /**
     * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
     * @return le VBOset du matériau, voir MeshModuleDrawing
     */
    virtual VBOset* createVBOset();


protected:

    virtual std::string getVertexShader();
    virtual std::string getFragmentShader();


protected:

    vec4 m_Color;

};

#endif
