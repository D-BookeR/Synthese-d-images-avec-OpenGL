#ifndef MATERIAL_TEXTUREMATERIAL_H
#define MATERIAL_TEXTUREMATERIAL_H

// Définition de la classe TextureMaterial

#include <gl-matrix.h>
#include <utils.h>

#include <VBOset.h>
#include <Material.h>
#include <Texture2D.h>



class TextureMaterial: public Material
{
public:

    /**
     * constructeur
     * @param texture pour colorer les objets
     */
    TextureMaterial(Texture2D* texture);


    /** destructeur */
    virtual ~TextureMaterial();

    /**
     * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
     * @return le VBOset du matériau
     */
    virtual VBOset* createVBOset();

    /**
     * Cette méthode active le matériau
     * @param mat4Projection : fournir la matrice de projection
     * @param mat4ModelView : fournir la matrice de vue
     */
    virtual void enable(mat4 mat4Projection, mat4 mat4ModelView);

    /**
     * Cette méthode désactive le matériau
     */
    virtual void disable();


protected:

    /**
     * recompile le shader du matériau
     */
    virtual void compileShader();

    std::string getVertexShader();
    std::string getFragmentShader();

protected:

    /** identifiants liés au shader */
    GLint m_TextureLoc;

    // textures
    Texture2D* m_Texture;

};

#endif
