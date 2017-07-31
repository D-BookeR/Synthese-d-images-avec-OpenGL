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
    /** identifiants liés au shader */
    GLint m_TimeLoc;

};

#endif
