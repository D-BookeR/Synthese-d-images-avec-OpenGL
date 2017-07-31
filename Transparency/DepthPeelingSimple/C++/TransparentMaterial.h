#ifndef TRANSPARENTMATERIAL_H
#define TRANSPARENTMATERIAL_H

// Définition de la classe TransparentMaterial

#include <sstream>

#include <gl-matrix.h>
#include <utils.h>

#include <Material.h>
#include <Texture2D.h>


class TransparentMaterial: public Material
{
public:

    /**
     * Constructeur de la classe TransparentMaterial.
     * @param Kd : un vec4(r,g,b,a) donnant la couleur diffuse et la transparence
     */
    TransparentMaterial(vec4 Kd);


    /** destructeur */
    virtual ~TransparentMaterial();

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


protected:

    /** couleur du matériau */
    vec4 m_Kd;

    /** identifiants des textures DepthMap */
    GLuint m_DepthMapFar;
    GLuint m_DepthMapNear;

    /** taille de la fenêtre */
    vec2 m_WindowSize;

    /** identifiants liés au shader */
    GLint m_DepthMapFarLoc;
    GLint m_DepthMapNearLoc;
    GLint m_WindowSizeLoc;

};

#endif
