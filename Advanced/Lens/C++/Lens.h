#ifndef LOUPE_H
#define LOUPE_H

// Définition de la classe Lens

#include <sstream>

#include <gl-matrix.h>
#include <utils.h>

#include <FrameBufferObject.h>
#include <Process.h>

// Cette classe permet d'appliquer un flou de Poisson sur un FBO
// voir https://github.com/spite/Wagner/blob/master/fragment-shaders/poisson-disc-blur-fs.glsl

class Lens: public Process
{
public:

    Lens();

    virtual ~Lens();

    /**
     * met à jour le rapport largeur/hauteur de la fenêtre
     * @param ratio largeur/hauteur
     */
    void setRatio(float ratio);

    /**
     * applique une distorsion en forme de loupe à l'image du FBO
     * @param fbo : FBO contenant l'image à traiter
     * @param strength : grossissement de la loupe
     */
    void process(FrameBufferObject* fbo, float strength);

protected:

    virtual std::string getFragmentShader();
    virtual void findUniformLocations();

protected:

    // rapport largeur/hauteur de la fenêtre
    float m_Ratio;

    GLint m_StrengthLoc;
    GLint m_RatioLoc;

};

#endif
