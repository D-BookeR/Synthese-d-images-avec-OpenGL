#ifndef TRAMAGE_H
#define TRAMAGE_H

// Définition de la classe Dithering

#include <sstream>

#include <gl-matrix.h>
#include <utils.h>

#include <Texture2D.h>
#include <FrameBufferObject.h>
#include <Process.h>

// Cette classe permet d'appliquer un tramage ordonné type Bayer sur un FBO


class Dithering: public Process
{
public:

    Dithering();

    virtual ~Dithering();


    /**
     * applique un tramage de Bayer à l'image du FBO
     * @param fbo : FBO contenant l'image à traiter
     */
    void process(FrameBufferObject* fbo);

protected:

    virtual std::string getFragmentShader();
    virtual void findUniformLocations();

protected:

    Texture2D* m_BayerTexture;

    GLint m_BayerMapLoc;
    GLint m_WindowSizeLoc;

};

#endif
