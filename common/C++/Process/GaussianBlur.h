#ifndef PROCESS_GAUSSIANBLUR_H
#define PROCESS_GAUSSIANBLUR_H

// Définition de la classe GaussianBlur

#include <gl-matrix.h>
#include <utils.h>

#include <FrameBufferObject.h>
#include <Process.h>



// Cette classe permet d'appliquer un flou gaussien sur un FBO
class GaussianBlur: public Process
{
public:

    GaussianBlur(int width, int height);

    virtual ~GaussianBlur();

    /**
     * Applique le traitement
     * @param fbo : FBO contenant l'image à traiter
     */
    virtual void process(FrameBufferObject* fbo, float radius);


protected:

    virtual std::string getHorizVertexShader();
    virtual std::string getVertiVertexShader();

    virtual std::string getFragmentShader();

    virtual void compileHorizShader();
    virtual void compileVertiShader();

protected:

    FrameBufferObject* m_FBO;

    GLint m_HorizShaderId;
    GLint m_HorizVertexLoc;
    GLint m_HorizTexCoordLoc;
    GLint m_HorizColorMapLoc;
    GLint m_HorizTexelSizeLoc;

    GLint m_VertiShaderId;
    GLint m_VertiVertexLoc;
    GLint m_VertiTexCoordLoc;
    GLint m_VertiColorMapLoc;
    GLint m_VertiTexelSizeLoc;
};


#endif
