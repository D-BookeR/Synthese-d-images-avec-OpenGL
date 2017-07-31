#ifndef MATERIAUCOULEUR_H
#define MATERIAL_SIMPLECOLORMATERIAL_H

// Définition de la classe SimpleColorMaterial

#include <sstream>

#include <gl-matrix.h>
#include <utils.h>

class SimpleColorMaterial;

#include <Material.h>
#include <Texture2D.h>


class SimpleColorMaterial: public Material
{
public:

    /**
     * constructeur
     */
    SimpleColorMaterial();

    /**
     * constructeur
     * @param color du matériau
     */
    SimpleColorMaterial(vec4 color);


    /** destructeur */
    virtual ~SimpleColorMaterial();

protected:

    virtual std::string getVertexShader();
    virtual std::string getFragmentShader();


protected:

    vec4 m_Color;

};

#endif
