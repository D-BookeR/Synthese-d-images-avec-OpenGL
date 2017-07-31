#ifndef MATERIAL_SPOTLIGHT_H
#define MATERIAL_SPOTLIGHT_H

/**
 * Définition de la classe SpotLight, un type de lampe plus sophistiqué
 */

#include <gl-matrix.h>
#include <utils.h>

class SpotLight;

#include <OmniLight.h>
#include <ShadowMap.h>


class SpotLight: public OmniLight
{
public:

    /**
     * constructeur
     * @param maxangle : angle en degrés d'ouverture total de la lampe
     * @param minangle : angle dans lequel l'intensité est 100%, il y a un dégradé entre minangle et maxangle
     * @param shadowmapsize : taille de la shadowmap en pixels si la lampe gère des ombres
     * @param near : distance la plus proche dans la shadowmap
     * @param far : distance la plus lointaine dans la shadowmap
     * @param offsetfill : true si la shadow map doit activer le décalage de polygones
     * @param cullface : mettre GL_FRONT ou GL_BACK pour éliminer les faces avant ou de dos, GL_NONE pour ne rien configurer
     */
    SpotLight(float maxangle, float minangle, int shadowmapsize, float near, float far, bool offsetfill, GLenum cullface);
    SpotLight(float maxangle, float minangle, int shadowmapsize, float near, float far);
    SpotLight(float maxangle, float minangle);

    /**
     * destructeur
     */
    virtual ~SpotLight();

    /**
     * Cette méthode définit la position visée par la lampe
     * @param target : vec4 indiquant le point ou l'axe que vise la lampe
     * NB: l'une des deux informations position et cible doivent être des points
     */
    void setTarget(vec4 target);

    /**
     * applique une matrice sur la position de la lampe afin d'obtenir la position caméra
     * @param matrix : mat4
     */
    virtual void transformPosition(mat4& matrix);

    /**
     * applique une matrice sur la cible de la lampe afin d'obtenir la position caméra
     * @param matrix : mat4
     */
    void transformTarget(mat4& matrix);

    /**
     * applique une matrice sur la position et la cible de la lampe
     * @param matrix : mat4
     */
    virtual void transform(mat4& matrix);

    /**
     * dessine la scène dans la shadowmap de cette lampe
     * @param scene à dessiner vue de la lampe this
     * @param mat4ViewCamera : matrice de transformation dans laquelle sont dessinés les objets
     */
    void makeShadowMap(SceneBase* scene, mat4& mat4ViewCamera);


protected:

    /** construit le Fragment Shader qui calcule l'éclairement de cette lampe */
    virtual std::string getFragmentShader();

    /** détermine où sont les variables uniform spécifiques de cette lampe */
    virtual void findUniformLocations();

    /** active le shader, les VBO et les textures pour appliquer l'éclairement défini par la lampe */
    virtual void startProcess(FrameBufferObject* gbuffer);

    /** désactive shader, VBO et textures */
    virtual void endProcess();


private:

    /**
     * initialisations communes à tous les constructeurs
     * @param maxangle : angle en degrés d'ouverture total de la lampe
     * @param minangle : angle dans lequel l'intensité est 100%, il y a un dégradé entre minangle et maxangle
     * @param shadowmapsize : taille de la shadowmap en pixels si la lampe gère des ombres
     * @param near : distance la plus proche dans la shadowmap
     * @param far : distance la plus lointaine dans la shadowmap
     * @param offsetfill : true si la shadow map doit activer le décalage de polygones
     * @param cullface : mettre GL_FRONT ou GL_BACK pour éliminer les faces avant ou de dos, GL_NONE pour ne rien configurer
     */
    void init(float maxangle, float minangle, int shadowmapsize, float near, float far, bool offsetfill, GLenum cullface);


protected:

    float m_MaxAngle;
    float m_CosMaxAngle;
    float m_MinAngle;
    float m_CosMinAngle;
    vec3 m_Direction;
    vec4 m_TargetScene;
    vec4 m_TargetCamera;
    float m_Near;
    float m_Far;

    /** gestion des ombres portées */
    ShadowMap* m_ShadowMap;
    mat4 m_ShadowMatrix;

    /** variables uniform du shader */
    GLint m_ShadowMapLoc;
    GLint m_ShadowMatrixLoc;
    GLint m_CosMaxAngleLoc;
    GLint m_CosMinAngleLoc;
    GLint m_DirectionLoc;

};

#endif
