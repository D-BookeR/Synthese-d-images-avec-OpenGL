#ifndef MATERIAL_SOFTSPOTLIGHT_H
#define MATERIAL_SOFTSPOTLIGHT_H

/**
 * Définition de la classe SoftSpotLight, un type de lampe plus sophistiqué
 */

#include <gl-matrix.h>
#include <utils.h>

#include <SpotLight.h>


class SoftSpotLight: public SpotLight
{
public:

    /**
     * constructeur
     * @param anglemax : angle en degrés d'ouverture total de la lampe
     * @param anglemin : angle dans lequel l'intensité est 100%, il y a un dégradé entre anglemin et anglemax
     * @param radius : largeur de la source de lumière en unités à rapporter à far et à la taille de la shadow map
     * @param shadowmapsize : taille de la shadowmap en pixels si la lampe gère des ombres
     * @param near : distance la plus proche dans la shadowmap
     * @param far : distance la plus lointaine dans la shadowmap
     * @param pcss : algorithme employé pour le calcul du flou des ombres true:PCSS, false:PCF
     * @param offsetfill : true si la shadow map doit activer le décalage de polygones
     * @param cullface : mettre GL_FRONT ou GL_BACK pour éliminer les faces avant ou de dos, GL_NONE pour ne rien configurer
     */
    SoftSpotLight(float anglemax, float anglemin, float radius, int shadowmapsize, float near, float far, bool pcss, bool offsetfill, GLenum cullface);
    SoftSpotLight(float anglemax, float anglemin, float radius, int shadowmapsize, float near, float far, bool pcss);
    SoftSpotLight(float anglemax, float anglemin, float radius, int shadowmapsize, float near, float far);

    /**
     * destructeur
     */
    virtual ~SoftSpotLight();

    /**
     * Cette méthode définit l'étendue de la lampe
     * @param radius : float donnant la largeur de la lampe
     */
    void setRadius(float radius);

    /**
     * retourne l'étendue de la lampe
     */
    float getRadius();


protected:

    /** construit le Fragment Shader qui calcule l'éclairement de cette lampe */
    virtual std::string getFragmentShader();

    /** détermine où sont les variables uniform spécifiques de cette lampe */
    virtual void findUniformLocations();

    /** active le shader, les VBO et les textures pour appliquer l'éclairement défini par la lampe */
    virtual void startProcess(FrameBufferObject* gbuffer);


private:

    /**
     * initialisations communes à tous les constructeurs
     * @param radius : largeur de la source de lumière en unités à rapporter à far et à la taille de la shadow map
     * @param pcss : algorithme employé pour le calcul du flou des ombres true:PCSS, false:PCF
     * @param offsetfill : true si la shadow map doit activer le décalage de polygones
     * @param cullface : mettre GL_FRONT ou GL_BACK pour éliminer les faces avant ou de dos, GL_NONE pour ne rien configurer
     */
    void init(float radius, bool pcss, bool offsetfill, GLenum cullface);


protected:

    /** gestion des ombres floues */
    float m_TanMaxAngle;
    float m_LightRadius;
    bool m_PCSS;

    /** variables uniform du shader */
    GLint m_TanMaxAngleLoc;
    GLint m_LightRadiusLoc;

};

#endif
