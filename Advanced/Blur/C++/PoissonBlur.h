#ifndef FLOUPOISSON_H
#define FLOUPOISSON_H

// Définition de la classe PoissonBlur

#include <sstream>

#include <gl-matrix.h>
#include <utils.h>

#include <FrameBufferObject.h>
#include <Process.h>

// Cette classe permet d'appliquer un flou de Poisson sur un FBO
// voir https://github.com/spite/Wagner/blob/master/fragment-shaders/poisson-disc-blur-fs.glsl

class PoissonBlur: public Process
{
public:

    PoissonBlur();

    virtual ~PoissonBlur();


    /**
     * applique le traitement sur l'image fournie dans le FBO
     * @param fbo : FBO contenant l'image à flouter
     * @param radius : rayon du flou
     */
    void process(FrameBufferObject* fbo, float radius);

protected:

    virtual std::string getFragmentShader();
    virtual void findUniformLocations();

protected:

    GLint m_TexelSizeLoc;
    GLint m_TimeLoc;

};

#endif
