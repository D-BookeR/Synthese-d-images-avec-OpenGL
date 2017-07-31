#ifndef CONVOLUTION_H
#define CONVOLUTION_H

// Définition de la classe Convolution

#include <sstream>

#include <gl-matrix.h>
#include <utils.h>

#include <FrameBufferObject.h>
#include <Process.h>

// Cette classe permet d'appliquer une convolution sur un FBO

class Convolution: public Process
{
public:

    Convolution();

    virtual ~Convolution();


    /**
     *
     * @param fbo : FBO contenant l'image à traiter
     */
    void process(FrameBufferObject* fbo);

protected:

    virtual std::string getFragmentShader();
    virtual void findUniformLocations();

protected:

    GLint m_TexelSizeLoc;

};

#endif
