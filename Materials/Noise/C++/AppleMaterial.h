#ifndef APPLEMATERIAL_H
#define APPLEMATERIAL_H

// Définition de la classe AppleMaterial

#include <string>

#include <gl-matrix.h>
#include <utils.h>
#include <Material.h>
#include <Texture2D.h>
#include <VBOset.h>

#include "NoiseValue2D.h"


class AppleMaterial: public Material
{
public:

    /**
     * constructeur
     */
    AppleMaterial();

    /** destructeur */
    ~AppleMaterial();

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
    Texture2D* m_TextureGradient;

    /** identifiants liés au shader */
    GLint m_MatPloc;
    GLint m_MatMVloc;
    GLint m_NoiseTextureLoc;
    GLint m_TextureGradientLoc;

};

#endif
