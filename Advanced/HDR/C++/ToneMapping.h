#ifndef TONEMAPPING_H
#define TONEMAPPING_H

// Définition de la classe ToneMapping

#include <sstream>

#include <gl-matrix.h>
#include <utils.h>

#include <FrameBufferObject.h>
#include <Process.h>

// Cette classe permet d'appliquer un tone mapping sur un FBO
// biblio : http://www.nutty.ca/?page_id=352&link=hdr

class ToneMapping: public Process
{
public:

    ToneMapping();

    virtual ~ToneMapping();


    /**
     *
     * @param fbo : FBO contenant l'image à flouter
     * @param maxlum : luminosité maximale de la scène
     * @param avglum : luminosité moyenne de la scène
     * @param alpha : coefficient alpha, luminance d'un gris moyen, 0.18 par défaut
     */
    void process(FrameBufferObject* fbo, float maxlum, float avglum, float alpha=0.18);

protected:

    virtual std::string getFragmentShader();
    virtual void findUniformLocations();

protected:

    GLint m_MaxLuminanceLoc;
    GLint m_AvgLuminanceLoc;
    GLint m_AlphaLoc;

};

#endif
