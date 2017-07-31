#ifndef BRUME_H
#define BRUME_H

// Définition de la classe Fog

#include <gl-matrix.h>
#include <utils.h>

#include <FrameBufferObject.h>
#include <Process.h>


// Cette classe permet de modifier la luminosité dans un FBO
class Fog: public Process
{
public:

    /**
     * constructeur
     * @param color : vec4 contenant la couleur de saturation de la brume
     * @param exposant pour élever la distance
     */
    Fog(vec4 color, float exponent);

    virtual ~Fog();

    /**
     * effectue le traitement
     * @param fbo : FBO contenant l'image à traiter
     */
    virtual void process(FrameBufferObject* fbo);


protected:

    virtual std::string getFragmentShader();

    virtual void findUniformLocations();

protected:

    GLint m_DepthMapLoc;
    GLint m_BackgroundColorLoc;
    GLint m_ExponentLoc;
};


#endif
