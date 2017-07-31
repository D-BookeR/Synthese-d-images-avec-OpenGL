#ifndef GREENMATERIAL_H
#define GREENMATERIAL_H

// Définition de la classe GreenMaterial

#include <string>
#include <gl-matrix.h>
#include <utils.h>

#include <Material.h>
#include <VBOset.h>



class GreenMaterial: public Material
{
public:

    /**
     * constructeur
     */
    GreenMaterial();

    /** destructeur */
    ~GreenMaterial();

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
