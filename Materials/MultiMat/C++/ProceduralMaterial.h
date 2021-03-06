#ifndef PROCEDURALMATERIAL_H
#define PROCEDURALMATERIAL_H

// Définition de la classe ProceduralMaterial

#include <string>
#include <gl-matrix.h>
#include <utils.h>

#include <Material.h>
#include <Texture2D.h>
#include <VBOset.h>



class ProceduralMaterial: public Material
{
public:

    /**
     * constructeur
     */
    ProceduralMaterial();

    /** destructeur */
    ~ProceduralMaterial();

    /**
     * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
     * @return le VBOset du matériau
     */
    VBOset* createVBOset();


protected:

    std::string getVertexShader();
    std::string getFragmentShader();

};

#endif
