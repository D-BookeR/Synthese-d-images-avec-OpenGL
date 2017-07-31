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
     * @param diffuse : nom d'un fichier image contenant la texture diffuse
     * @param Ks : vec3
     * @param Ns : poli du matériau
     */
    GroundMaterial(std::string heightmap, float hmax, float delta, std::string diffuse, vec3 Ks, float Ns);

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
    GLuint m_TxDiffuseLoc;

    // textures
    Texture2D* m_TxHeightmap;
    Texture2D* m_TxDiffuse;

    float m_HMax;
    float m_Delta;
    vec3 m_Ks;
    float m_Ns;

};

#endif
