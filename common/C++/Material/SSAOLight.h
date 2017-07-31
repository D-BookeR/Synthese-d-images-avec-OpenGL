#ifndef MATERIAL_SSAOLIGHT_H
#define MATERIAL_SSAOLIGHT_H

/**
 * Définition de la classe SSAOLight, une lampe ambiante avec ombrage causé par les occlusions locales
 */

#include <gl-matrix.h>
#include <utils.h>

#include <Light.h>


class SSAOLight: public Light
{
public:

    /**
     * constructeur
     * C'est une lampe ambiante avec ombrage causé par les occlusions locales
     * @param rayon : distance relative à [0.0, 1.0] d'exploration des points, mettre 0.04
     * @param mindistance : distance en unités du monde où un objet est considéré comme proche
     * @param maxdistance : distance en unités du monde à partir de laquelle un objet est trop loin pour occulter
     */
    SSAOLight(float radius, float mindistance, float maxdistance);

    /**
     * destructeur
     */
    virtual ~SSAOLight();


protected:

    /** construit le Fragment Shader qui calcule l'éclairement de cette lampe */
    virtual std::string getFragmentShader();

    /** détermine où sont les variables uniform spécifiques de cette lampe */
    virtual void findUniformLocations();

    /** active le shader, les VBO et les textures pour appliquer l'éclairement défini par la lampe */
    virtual void startProcess(FrameBufferObject* gbuffer);


protected:

    /** paramètres de la lampe */
    float m_Radius;
    float m_MinDistance;
    float m_MaxDistance;

    /** emplacement des variables uniform du shader */
    GLint m_RadiusLoc;
    GLint m_MinDistanceLoc;
    GLint m_MaxDistanceLoc;

};

#endif
