#ifndef INVERTCOLORS_H
#define INVERTCOLORS_H

// Définition de la classe InvertColors

#include <sstream>

#include <gl-matrix.h>
#include <utils.h>

#include <Texture2D.h>
#include <Process.h>

// Cette classe permet de calculer l'image négative (couleurs inversées)

class InvertColors: public Process
{
public:

    InvertColors();

    virtual ~InvertColors();


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
