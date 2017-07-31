#ifndef REDMATERIAL_H
#define REDMATERIAL_H

// Définition de la classe RedMaterial

#include <string>
#include <gl-matrix.h>
#include <utils.h>

#include <Material.h>
#include <VBOset.h>



class RedMaterial: public Material
{
public:

    /**
     * constructeur
     */
    RedMaterial();

    /** destructeur */
    ~RedMaterial();


protected:

    std::string getVertexShader();
    std::string getFragmentShader();

};

#endif
