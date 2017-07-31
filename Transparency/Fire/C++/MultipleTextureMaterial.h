#ifndef MULTIPLETEXTUREMATERIAL_H
#define MULTIPLETEXTUREMATERIAL_H

// Définition de la classe MultipleTextureMaterial

#include <string>
#include <gl-matrix.h>
#include <utils.h>

#include <Texture2D.h>
#include <Material.h>
#include <VBOset.h>



class MultipleTextureMaterial: public Material
{
public:

    /**
     * @param texture sur laquelle est basé le matériau
     * @param nbcols nombre de sous-images horizontalement = nombre de colonnes
     * @param nbligs nombre de sous-images verticalement = nombre de lignes
     * @param delai temps entre deux images pour l'animation
     */
    MultipleTextureMaterial(Texture2D* texture, int nbcols=1, int nbligs=1, float delai=0.05f);

    /** destructeur */
    ~MultipleTextureMaterial();

    /**
     * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
     * @return le VBOset du matériau
     */
    VBOset* createVBOset();

    /**
     * sélectionne la partie de la Texture2D correspondant au temps
     */
    void update();

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
    GLint m_OffsetLoc;
    GLint m_SizeLoc;

    // textures
    Texture2D* m_Texture;

    // caractéristiques de l'image
    int m_LineCount;
    int m_ColumnCount;
    int m_TotalNumber;
    vec2 m_Size;
    vec2 m_Offset;
    float m_Delay;

};

#endif
