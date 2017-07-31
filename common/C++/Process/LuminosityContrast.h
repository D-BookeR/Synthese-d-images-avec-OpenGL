#ifndef PROCESS_LUMINOSITYCONSTRAST_H
#define PROCESS_LUMINOSITYCONSTRAST_H

// Définition de la classe LuminosityContrast

#include <gl-matrix.h>
#include <utils.h>

#include <Process.h>


// Cette classe permet de modifier la luminosité dans un FBO
class LuminosityContrast: public Process
{
public:

    LuminosityContrast();

    virtual ~LuminosityContrast();

    /**
     *
     * @param fbo : FBO contenant l'image à traiter
     */
    virtual void process(FrameBufferObject* fbo, float luminosite=0.0, float contraste=1.0);


protected:

    virtual std::string getFragmentShader();

    virtual void findUniformLocations();

protected:

    GLint m_LuminosityLoc;
    GLint m_ContrastLoc;
};


#endif
