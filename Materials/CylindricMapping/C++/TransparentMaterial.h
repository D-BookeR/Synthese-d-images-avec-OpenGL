#ifndef TRANSPARENTMATERIAL_H
#define TRANSPARENTMATERIAL_H

// Définition de la classe TransparentMaterial

#include <sstream>

#include <gl-matrix.h>
#include <utils.h>
#include <VBOset.h>
#include <DeferredShadingMaterial.h>
#include <Texture2D.h>
#include <Material.h>


class TransparentMaterial: public Material
{
public:

    /**
     * constructeur
     * @param texture Texture2D à appliquer
     * @param alpha transparence du matériau
     */
    TransparentMaterial(Texture2D* texture, float alpha);

    /** destructeur */
    virtual ~TransparentMaterial();

    /**
     * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
     * @return le VBOset du matériau
     */
    VBOset* createVBOset();

    /**
     * affecte la texture DepthMap du shader
     * @param far : depth buffer qui limite les dessins arrière de ce matériau
     * @param near : depth buffer qui limite les dessins avant de ce matériau
     */
    void setDepthMaps(GLuint far, GLuint near);

    /**
     * spécifie la taille de la fenêtre
     * @param width : largeur en nombre de pixels de la fenêtre
     * @param height : hauteur en nombre de pixels de la fenêtre
     */
    void setWindowDimensions(int width, int height);

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

    /** recompile le shader du matériau */
    virtual void compileShader();

    virtual std::string getFragmentShader();
    virtual std::string getVertexShader();


protected:

    Texture2D* m_Texture;
    float m_Alpha;

    /** identifiants des textures DepthMap */
    GLuint m_DepthMapFar;
    GLuint m_DepthMapNear;

    /** taille de la fenêtre */
    vec2 m_WindowSize;

    /** identifiants liés au shader */
    GLint m_MatPloc;
    GLint m_MatMVloc;
    GLint m_TextureLoc;
    GLint m_DepthMapFarLoc;
    GLint m_DepthMapNearLoc;
    GLint m_WindowSizeLoc;

};

#endif
