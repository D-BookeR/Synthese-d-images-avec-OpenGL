#ifndef MATERIAL_OMNILIGHT_H
#define MATERIAL_OMNILIGHT_H

/**
 * Définition de la classe OmniLight, une base pour construire des éclairages diffus et localisés
 */

#include <gl-matrix.h>
#include <utils.h>

#include <Light.h>


class OmniLight: public Light
{
public:

    /**
     * constructeur
     * C'est une lampe toute simple, positionnelle ou directionnelle, sans ombre
     */
    OmniLight();

    /**
     * destructeur
     */
    virtual ~OmniLight();

    /**
     * indique si cette lampe a une position (true) ou est ambiante (false)
     * @return false pour une lampe ambiante
     */
    virtual bool hasPosition();

    /**
     * Cette méthode définit la position de la lampe
     * @param position : vec4 indiquant où se trouve la lampe
     */
    virtual void setPosition(vec4 position);

    /**
     * retourne la position de la lampe
     */
    vec4 getPosition();

    /**
     * applique une matrice sur la position de la lampe afin d'obtenir la position caméra
     * @param matrix : mat4
     */
    virtual void transformPosition(mat4& matrix);

    /**
     * Cette méthode définit la position visée par la lampe
     * NB: cette méthode ne fait rien dans le cas d'une lampe omnidirectionnelle
     * @param position : vec4 indiquant où se trouve la lampe
     * @note l'une des deux informations position et cible doivent être des points
     */
    virtual void setTarget(vec4 target);

    /**
     * applique une matrice sur la cible de la lampe afin d'obtenir la position caméra
     * NB: cette méthode ne fait rien dans le cas d'une lampe omnidirectionnelle
     * @param matrix : mat4
     */
    virtual void transformTarget(mat4& matrix);

    /**
     * applique une matrice sur la position de la lampe
     * @param matrix : mat4
     */
    virtual void transform(mat4& matrix);

    /**
     * retourne la position caméra de la lampe
     */
    vec4 getPositionCamera();


protected:

    /** construit le Fragment Shader qui calcule l'éclairement de cette lampe */
    virtual std::string getFragmentShader();

    /** détermine où sont les variables uniform spécifiques de cette lampe */
    virtual void findUniformLocations();

    /** active le shader, les VBO et les textures pour appliquer l'éclairement défini par la lampe */
    virtual void startProcess(FrameBufferObject* gbuffer);


protected:

    /** définitions spécifiques à ce type de lampe **/
    vec4 m_PositionScene;

    /** position effective de la lampe : après transformation */
    vec4 m_PositionCamera;

    /** emplacement des uniform communs de ce type de lampe **/
    GLint m_LightPositionLoc;

};

#endif
