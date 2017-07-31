#ifndef SUBPICTURE_H
#define SUBPICTURE_H

// Définition de la classe SubPicture

#include <sstream>

#include <gl-matrix.h>
#include <utils.h>

#include <FrameBufferObject.h>
#include <Process.h>

// Cette classe permet d'afficher une partie d'un FBO avec un zoom


class SubPicture: public Process
{
public:

    SubPicture(int samplesnb = 0);

    virtual ~SubPicture();

    /**
     * effectue le traitement
     * @param fbo : FBO contenant l'image à extraire
     * @param x0 : absisse du coin haut gauche de l'image
     * @param y0 : ordonnée du coin haut gauche de l'image
     * @param width : largeur de l'image à extraire
     * @param height : hauteur de l'image à extraire
     */
    void process(FrameBufferObject* fbo, int x0, int y0, int width, int height);

protected:

    virtual std::string getFragmentShader();
    virtual void findUniformLocations();
    virtual void setTextureUnit(GLint unit, GLint locSampler=0, GLuint buffer=0);

protected:

    /** nombre d'échantillons */
    int m_SamplesNumber;

    GLint m_TopLeftCornerLoc;
    GLint m_SizeLoc;

};

#endif
