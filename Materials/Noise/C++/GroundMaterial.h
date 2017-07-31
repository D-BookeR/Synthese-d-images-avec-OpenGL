#ifndef GROUNDMATERIAL_H
#define GROUNDMATERIAL_H

// Définition de la classe GroundMaterial

#include <string>

#include <gl-matrix.h>
#include <utils.h>
#include <Material.h>
#include <Texture2D.h>
#include <VBOset.h>

#include "NoiseValue2D.h"



class GroundMaterial: public Material
{
public:

    /**
     * constructeur
     */
    GroundMaterial();

    /** destructeur */
    ~GroundMaterial();

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

    NoiseValue2D* m_NoiseTexture;

    /** identifiants liés au shader */
    GLint m_MatPloc;
    GLint m_MatMVloc;
    GLint m_NoiseTextureLoc;

};

#endif
