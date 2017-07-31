#ifndef GROUNDMATERIAL_H
#define GROUNDMATERIAL_H

// Définition de la classe Material

#include <string>
#include <gl-matrix.h>
#include <utils.h>

#include <Material.h>
#include <Texture2D.h>
#include <VBOset.h>



class GroundMaterial: public Material
{
public:

    /**
     * constructeur
     * @param heightmap : nom d'un fichier image contenant le relief
     * @param hmax : float qui donne la hauteur relative du terrain, ex: 0.4
     * @param delta : float qui indique la distance pour calculer la normale
     */
    GroundMaterial(std::string heightmap, float hmax, float delta);

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
    /** identifiants liés au shader */
    GLuint m_TxHeightmapLoc;

    // textures
    Texture2D* m_TxHeightmap;

    float m_HMax;
    float m_Delta;

};

#endif
