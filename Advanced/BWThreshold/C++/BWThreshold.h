#ifndef THRESHOLD_H
#define THRESHOLD_H

// Définition de la classe BWThreshold

#include <sstream>

#include <gl-matrix.h>
#include <utils.h>

#include <Texture2D.h>
#include <Process.h>

// Cette classe permet d'appliquer un seuil sur une image

class BWThreshold: public Process
{
public:

    BWThreshold();

    virtual ~BWThreshold();


    /**
     * applique le traitement sur l'image fournie
     * @param texture : Texture2D contenant l'image à traiter
     * @param threshold : seuil 0.0 à 1.0
     */
    void process(Texture2D* texture, float threshold=0.5);

protected:

    virtual std::string getFragmentShader();
    virtual void findUniformLocations();

protected:

    GLint m_ThresholdLoc;

};

#endif
