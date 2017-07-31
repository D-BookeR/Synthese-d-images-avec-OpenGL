#ifndef SATURATION_H
#define SATURATION_H

// Définition de la classe Saturation

#include <gl-matrix.h>
#include <utils.h>

#include <FrameBufferObject.h>
#include <Process.h>



// Cette classe permet de saturer les couleurs d'un FBO
class Saturation: public Process
{
public:

    Saturation(int width, int height);

    virtual ~Saturation();

    /**
     * Applique le traitement
     * @param fbo : FBO contenant l'image à traiter
     * @param force de l'effet, 1.0 ne change rien
     */
    virtual void process(FrameBufferObject* fbo, float strength);


protected:

    virtual std::string getFragmentShader();
    virtual void findUniformLocations();

protected:

    GLint m_StrengthLoc;

};


#endif
