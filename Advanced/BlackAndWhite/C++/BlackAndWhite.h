#ifndef NOIRETBLANC_H
#define NOIRETBLANC_H

// Définition de la classe BlackAndWhite

#include <sstream>

#include <gl-matrix.h>
#include <utils.h>

#include <Texture2D.h>
#include <Process.h>

// Cette classe permet de mettre une image en noir&blanc

class BlackAndWhite: public Process
{
public:

    BlackAndWhite();

    virtual ~BlackAndWhite();


    /**
     *
     * @param texture : Texture2D contenant l'image à mettre en noir&blanc
     */
    void process(Texture2D* texture);

protected:

    virtual std::string getFragmentShader();
    virtual void findUniformLocations();

protected:

    GLint m_ColorPictureLoc;

};

#endif
