#ifndef COLORPERVERTEXMATERIAL_H
#define COLORPERVERTEXMATERIAL_H

// Définition de la classe ColorPerVertexMaterial

#include <string>
#include <gl-matrix.h>
#include <utils.h>

#include <Material.h>
#include <VBOset.h>



class ColorPerVertexMaterial: public Material
{
public:

    /**
     * constructeur
     */
    ColorPerVertexMaterial();

    /** destructeur */
    ~ColorPerVertexMaterial();

    /**
     * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
     * @return le VBOset du matériau
     */
    VBOset* createVBOset();


protected:

    std::string getVertexShader();
    std::string getFragmentShader();


private:
    /** identifiants liés au shader */
    GLint m_MatPloc;
    GLint m_MatMVloc;

};

#endif
