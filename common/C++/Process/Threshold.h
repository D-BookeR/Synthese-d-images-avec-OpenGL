#ifndef PROCESS_THRESHOLD_H
#define PROCESS_THRESHOLD_H

// Définition de la classe Threshold

#include <gl-matrix.h>
#include <utils.h>

#include <Process.h>


// Cette classe permet d'appliquer un threshold sur un FBO
class Threshold: public Process
{
public:

    Threshold();

    virtual ~Threshold();

    /**
     * applique le traitement sur le FBO fourni
     * @param fbo : FBO contenant l'image à traiter
     */
    virtual void process(FrameBufferObject* fbo, float threshold);


protected:

    virtual std::string getFragmentShader();

    virtual void findUniformLocations();

protected:

    GLint m_ThresholdLoc;
};


#endif
