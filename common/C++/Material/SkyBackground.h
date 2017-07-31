#ifndef MATERIAL_SKYBACKGROUND_H
#define MATERIAL_SKYBACKGROUND_H

// Définition de la classe SkyBackground : se comporte comme une lampe pour ajouter une couleur sur le fond

#include <gl-matrix.h>
#include <utils.h>

#include <Light.h>


// Cette classe permet d'appliquer une couleur sur le fond
class SkyBackground: public Light
{
public:

    SkyBackground();

    virtual ~SkyBackground();


protected:

    virtual std::string getFragmentShader();


protected:

    GLint m_ClearColorLoc;
};


#endif
