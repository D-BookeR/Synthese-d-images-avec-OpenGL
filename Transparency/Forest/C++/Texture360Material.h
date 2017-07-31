#ifndef TEXTURE360MATERIAL_H360_H
#define TEXTURE360MATERIAL_H360_H

// Définition de la classe Texture360Material

#include <string>
#include <gl-matrix.h>
#include <utils.h>

#include <Texture2D.h>
#include <Material.h>
#include <VBOset.h>

#include "Texture360.h"



class Texture360Material: public Material
{
public:

    /**
     * Constructeur
     * @param texture360 sur laquelle est basé le matériau
     */
    Texture360Material(Texture360* texture360);

    /** destructeur */
    ~Texture360Material();

    /**
     * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
     * @return le VBOset du matériau
     */
    VBOset* createVBOset();

    /**
     * modifie les coefficients de couleur et de transparence
     * @param colorCoefficient : float de 0 à 1 multiplié par la couleur
     * @param alphaCoefficient : float de 0 à 1 multiplié par le coefficient alpha
     */
    void setCoefficients(float colorCoefficient=1.0, float alphaCoefficient=1.0);

    /**
     * sélectionne la Texture2D correspondant à l'angle
     * @param angle : float entre 0 et 1
     */
    void select(float angle);

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
    GLint m_ColorCoefficientLoc;
    GLint m_AlphaCoefficientLoc;

    // textures
    Texture360* m_Texture360;
    Texture2D* m_Texture;

    // modification de la couleur et transparence
    float m_ColorCoefficient;
    float m_AlphaCoefficient;

};

#endif
