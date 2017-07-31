#ifndef SKYBOXMATERIAL_H
#define SKYBOXMATERIAL_H

// Définition de la classe SkyboxMaterial

#include <string>
#include <gl-matrix.h>
#include <utils.h>

#include <Material.h>
#include <TextureCube.h>
#include <VBOset.h>



class SkyboxMaterial: public Material
{
public:

    /**
     * constructeur
     * @param texture : instance de TextureCube
     */
    SkyboxMaterial(TextureCube* texture);

    /** destructeur */
    ~SkyboxMaterial();

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
    /** identifiants liés au shader */
    GLint m_TextureLoc;

    // texture
    TextureCube* m_Texture;

};

#endif
