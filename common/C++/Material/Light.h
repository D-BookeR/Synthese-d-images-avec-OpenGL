#ifndef MATERIAL_LIGHT_H
#define MATERIAL_LIGHT_H

/**
 * Définition de la classe Light, une base pour construire des éclairages en mode Deferred Shading
 */

#include <gl-matrix.h>
#include <utils.h>

#include <Process.h>
#include <SceneElement.h>

class SceneBase;


class Light: public Process, public Drawable
{
public:

    /**
     * constructeur
     * C'est une lampe ambiante toute simple, sans ombre
     */
    Light();

    /**
     * destructeur
     */
    virtual ~Light();

    /**
     * indique si cette lampe a une position (true) ou est ambiante (false)
     * @return false pour une lampe ambiante
     */
    virtual bool hasPosition();

    /**
     * retourne la position caméra de la lampe
     * NB: cette méthode ne fait rien dans le cas d'une lampe générique
     */
    virtual vec4 getPositionCamera();

    /**
     * applique une matrice sur la position de la lampe
     * @param matrix : mat4
     */
    virtual void transform(mat4& matrix);

    /**
     * Cette méthode définit la couleur de la lampe
     * @param color : vec3 donnant la couleur (intensité) de la lampe
     */
    void setColor(vec3 color);

    /**
     * retourne la couleur de la lampe
     */
    vec3 getColor();

    /**
     * applique l'éclairement défini par cette lampe
     * @param gbuffer : FBO MRT contenant toutes les informations de la scène à éclairer
     */
    virtual void process(FrameBufferObject* gbuffer);

    /** "dessine" cette lampe dans le cas où elle est un SceneElement d'une scène */
    virtual void onDraw(mat4& mat4Projection, mat4& mat4View);

    /**
     * dessine la scène dans la shadowmap de cette lampe
     * NB: cette méthode ne fait rien dans le cas d'une lampe omnidirectionnelle
     * @param scene
     * @param mat4ViewScene : matrice de transformation dans laquelle sont dessinés les objets
     */
    virtual void makeShadowMap(SceneBase* scene, mat4& mat4ViewScene);


protected:

    /** construit le Fragment Shader qui calcule l'éclairement de cette lampe */
    virtual std::string getFragmentShader();

    /** détermine où sont les variables uniform spécifiques de cette lampe */
    virtual void findUniformLocations();

    /** active le shader, les VBO et les textures pour appliquer l'éclairement défini par la lampe */
    virtual void startProcess(FrameBufferObject* gbuffer);

    /** désactive shader, VBO et textures */
    virtual void endProcess();

protected:

    /** définitions spécifiques à ce type de lampe **/
    vec3 m_Color;

    /** emplacement des uniform communs à toutes les lampes **/
    GLint m_MapPositionLoc;
    GLint m_MapNormalLoc;
    GLint m_MapDiffuseLoc;
    GLint m_MapSpecularLoc;
    GLint m_MapDepthLoc;
    GLint m_LightColorLoc;
};

#endif
