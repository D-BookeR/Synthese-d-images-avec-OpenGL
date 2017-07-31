#ifndef TWISTMATERIAL_H
#define TWISTMATERIAL_H

// Définition de la classe TwistMaterial

#include <string>
#include <gl-matrix.h>
#include <utils.h>

#include <Material.h>
#include <Texture2D.h>
#include <VBOset.h>



class TwistMaterial: public Material
{
public:

    /**
     * constructeur
     */
    TwistMaterial();

    /** destructeur */
    ~TwistMaterial();

    /**
     * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
     * @return le VBOset du matériau
     */
    VBOset* createVBOset();

    /**
     * modifie l'angle maximal de la torsion
     * @param anglemax en degrés
     */
    void setMaxAngle(float anglemax);

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
    GLint m_AngleMaxLoc;

    // textures
    Texture2D* m_Texture;

};

#endif
