#ifndef FLOUPROFONDEURCHAMP_H
#define FLOUPROFONDEURCHAMP_H

// Définition de la classe DepthOfFieldBlur

#include <sstream>

#include <gl-matrix.h>
#include <utils.h>

#include <FrameBufferObject.h>
#include <Process.h>

// Cette classe permet d'appliquer un flou variable selon la distance
// voir http://www.nutty.ca/?page_id=352&link=depth_of_field
// voir https://github.com/ga-m3dv/ga-worldwind-suite/blob/master/Common/src/main/java/au/gov/ga/worldwind/common/effects/depthoffield/DepthOfFieldFragmentShader.glsl

class DepthOfFieldBlur: public Process
{
public:

    DepthOfFieldBlur();

    virtual ~DepthOfFieldBlur();

    /**
     * change les distances near et far du shader
     * @param near : distance la plus proche dans le depth buffer du FBO à traiter
     * @param far : distance la plus lointaine dans le depth buffer du FBO à traiter
     */
    void setNearFar(float near, float far);

    /**
     * dessine la texture fournie sur l'écran
     * @param fbonet : FBO contenant l'image et son depth buffer
     * @param focalLength : distance focale de l'objectif
     * @param aperture : ouverture de l'objectif
     * @param distance : distance où l'image est parfaitement nette (doit être >> focale)
     */
    void process(FrameBufferObject* fbonet, float focalLength, float aperture, float distance);

protected:

    virtual std::string getFragmentShader();
    virtual void findUniformLocations();

protected:

    float m_Near;
    float m_Far;

    GLint m_DepthMapLoc;
    GLint m_TexelSizeLoc;
    GLint m_NearLoc;
    GLint m_FarLoc;
    GLint m_SharpDistanceLoc;
    GLint m_SharpnessLoc;
    GLint m_TimeLoc;

};

#endif
