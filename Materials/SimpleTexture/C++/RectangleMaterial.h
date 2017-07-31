#ifndef RECTANGLEMATERIAL_H
#define RECTANGLEMATERIAL_H

// Définition de la classe RectangleMaterial

#include <gl-matrix.h>
#include <utils.h>

#include <Material.h>
#include <Texture2D.h>
#include <VBOset.h>



class RectangleMaterial: public Material
{
public:

    /**
     * constructeur
     */
    RectangleMaterial();

    /** destructeur */
    virtual ~RectangleMaterial();

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

    /**
     * recompile le shader du matériau
     */
    virtual void compileShader();

    std::string getVertexShader();
    std::string getFragmentShader();

protected:

    /** identifiants liés au shader */
    GLint m_MatPloc;
    GLint m_MatMVloc;
    GLint m_TextureLoc;

    // textures
    Texture2D* m_Texture;

};

#endif
