#ifndef TREMBLEMENT_H
#define TREMBLEMENT_H

// Définition de la classe AnimatedRipples

#include <sstream>

#include <gl-matrix.h>
#include <utils.h>

#include <Texture2D.h>
#include <FrameBufferObject.h>
#include <Process.h>

// Cette classe permet d'appliquer une déformation type tremblement sur un FBO
// voir https://github.com/SFML/SFML/wiki/Source:-HeatHazeShader


class AnimatedRipples: public Process
{
public:

    AnimatedRipples();

    virtual ~AnimatedRipples();


    /**
     * applique une distorsion à l'image du FBO
     * @param fbo : FBO contenant l'image à traiter
     * @param strength : importance de l'effet
     * @param speed : vitesse de l'effet
     */
    void process(FrameBufferObject* fbo, float strength=0.003, float speed=1.0);

protected:

    virtual std::string getFragmentShader();
    virtual void findUniformLocations();

protected:

    Texture2D* m_Texture;

    GLint m_DistorsionMapLoc;
    GLint m_StrengthLoc;
    GLint m_SpeedLoc;
    GLint m_TimeLoc;

};

#endif
