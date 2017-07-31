#ifndef PROCESS_PROCESS_H
#define PROCESS_PROCESS_H

// Définition de la classe Process

#include <gl-matrix.h>
#include <utils.h>

#include <FrameBufferObject.h>


// Cette classe permet d'appliquer un traitement sur un FBO
class Process
{
public:

    Process(std::string nom);

    virtual ~Process();

    /**
     *
     * @param fbo : FBO contenant l'image à traiter
     */
    //virtual void process(FrameBufferObject* fbo);

    virtual void setTextureUnit(GLint unit, GLint locSampler=-1, GLuint bufferId=0);

    /** initialise les VBO et shaders pour dessiner les FBO */
    static void staticinit();

    /** supprime les VBO et shaders pour dessiner les FBO */
    static void staticdestroy();

protected:

    virtual std::string getVertexShader();
    virtual std::string getFragmentShader() = 0;

    virtual void compileShader();
    virtual void findUniformLocations();

    /** active le shader et les VBO pour appliquer le traitement défini dans le shader */
    virtual void startProcess();
    /** désactive le shader et les VBO */
    virtual void endProcess();


protected:

    std::string m_Name;
    GLuint m_ShaderId;
    GLint m_VertexLoc;
    GLint m_TexCoordLoc;
    GLint m_ColorMapLoc;


    static GLint m_VertexBufferId;
    static GLint m_TexCoordBufferId;
};


#endif
