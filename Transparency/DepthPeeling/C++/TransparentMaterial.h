#ifndef TRANSPARENTMATERIAL_H
#define TRANSPARENTMATERIAL_H

// Définition de la classe TransparentMaterial

#include <sstream>

#include <gl-matrix.h>
#include <utils.h>
#include <DeferredShadingMaterial.h>
#include <Texture2D.h>


class TransparentMaterial: public DeferredShadingMaterial
{
public:

    /**
     * Constructeur de la classe TransparentMaterial. Ce matériau mémorise dans un FBO MRT les informations
     * nécessaires pour appliquer le modèle de Phong ultérieurement : position, normale, couleur.
     * @param Kd : un vec4(r,g,b,a) ou une texture donnant la couleur diffuse et la transparence
     */
    TransparentMaterial(vec4 Kd);

    /**
     * constructeur
     * @param Ks : un vec3(r,g,b) donnant la couleur spéculaire
     * @param Ns : poli du matériau
     */
    TransparentMaterial(vec3 Ks, float Ns);

    /**
     * constructeur
     * @param Kd : un vec4(r,g,b,a) ou une texture donnant la couleur diffuse et la transparence
     * @param Ks : un vec3(r,g,b) donnant la couleur spéculaire
     * @param Ns : poli du matériau
     */
    TransparentMaterial(vec4 Kd, vec3 Ks, float Ns);

    /**
     * constructeur
     * @param diffuse : nom d'une texture
     * @param Ks : un vec3(r,g,b) donnant la couleur spéculaire
     * @param Ns : poli du matériau
     */
    TransparentMaterial(std::string diffuse, vec3 Ks, float Ns);


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

    /** initialise les variables membres */
    void init();

    /** recompile le shader du matériau */
    virtual void compileShader();

    virtual std::string getFragmentShader();


protected:

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
