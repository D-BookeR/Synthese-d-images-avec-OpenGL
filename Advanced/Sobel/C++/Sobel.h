#ifndef SOBEL_H
#define SOBEL_H

// Définition de la classe Sobel

#include <gl-matrix.h>
#include <utils.h>

#include <FrameBufferObject.h>
#include <Process.h>



// Cette classe permet d'appliquer le filtre de Sobel sur un FBO
class Sobel: public Process
{
public:

    Sobel(int width, int height);

    virtual ~Sobel();

    /**
     * Applique le traitement
     * @param fbo : FBO contenant l'image à traiter
     */
    virtual void process(FrameBufferObject* fbo);


protected:

    /** retourne le vertex shader correspondant à la passe */
    virtual std::string getVertexShader(int pass);

    /** retourne le fragment shader correspondant à la passe */
    virtual std::string getFragmentShader(int pass);

    // pour un traitement standard mais ici c'est pas le cas
    std::string getFragmentShader() {return "";}

    /** compile le shader correspondant à la passe */
    virtual void compileShader(int pass);

protected:

    // FBO intermédiaires pour effectuer les passes de traitement
    FrameBufferObject* m_FBO[2];

    // identifiants des shaders et des variables uniform et attribute
    GLint m_ShaderId[4];
    GLuint m_VertexLoc[4];
    GLint  m_TexCoordLoc[4];
    GLint  m_ColorMapLoc[4];
    GLint  m_TexelSizeLoc[4];
};


#endif
