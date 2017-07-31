#ifndef HEIGHTMAPMATERIAL_H
#define HEIGHTMAPMATERIAL_H

// Définition de la classe DisplacementMaterial

#include <string>
#include <gl-matrix.h>
#include <utils.h>

#include <Material.h>
#include <Texture2D.h>
#include <VBOset.h>



class DisplacementMaterial: public Material
{
public:

    /**
     * constructeur
     */
    DisplacementMaterial();

    /** destructeur */
    ~DisplacementMaterial();

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

    /** textures */
    Texture2D* m_DiffuseTexture;
    Texture2D* m_NormalTexture;
    Texture2D* m_ElevationTexture;

    /** identifiants liés au shader */
    GLint m_DiffuseTextureLoc;
    GLint m_NormalTextureLoc;
    GLint m_ElevationTextureLoc;

};

#endif
