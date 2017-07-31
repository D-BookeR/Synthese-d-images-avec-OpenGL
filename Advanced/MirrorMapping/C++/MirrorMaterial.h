#ifndef MIRRORMATERIAL_H
#define MIRRORMATERIAL_H

// Définition de la classe MirrorMaterial

#include <string>
#include <gl-matrix.h>
#include <utils.h>

#include <Material.h>
#include <TextureCube.h>
#include <VBOset.h>



class MirrorMaterial: public Material
{
public:

    /**
     * constructeur
     * @param texture : instance de TextureCube
     */
    MirrorMaterial(TextureCube* texture);

    /** destructeur */
    ~MirrorMaterial();

    /**
     * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
     * @return le VBOset du matériau
     */
    VBOset* createVBOset();

    /**
     * Cette méthode active le matériau : met en place son shader,
     * fournit les variables uniform qu'il demande
     * @param mat4Projection : mat4 contenant la projection
     * @param mat4ModelViewTeapot : mat4 contenant la transformation vers la caméra
     */
    void enable(mat4 mat4Projection, mat4 mat4ModelViewTeapot);

    /**
     * indique au matériau quelle est la transformation de la scène afin
     * de viser le point correct dans la skybox qui se réfléchit sur le matériau
     * @param mat4ModelViewScene : matrice ModelView utilisée pour dessiner la skybox
     */
    void setModelViewScene(mat4 mat4ModelViewScene);


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
    GLint m_MatProjectionLoc;
    GLint m_MatModelViewTeapotLoc;
    GLint m_MatNormalSceneLoc;
    GLint m_MatNormalTeapotLoc;
    GLint m_TextureLoc;

    // textures
    TextureCube* m_Texture;

};

#endif
