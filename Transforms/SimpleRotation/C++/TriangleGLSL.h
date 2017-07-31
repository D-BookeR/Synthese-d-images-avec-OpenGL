#ifndef TRIANGLEGLSL_H
#define TRIANGLEGLSL_H

// Définition de la classe TriangleGLSL

#include <gl-matrix.h>

class TriangleGLSL
{
private:

    /** tableau des coordonnées, x,y,z à la suite */
    GLuint m_VertexBufferId;
    GLuint m_ColorBufferId;

    /** identifiants liés au shader */
    GLuint m_ShaderId;
    GLint m_VertexLoc;
    GLint m_ColorLoc;
    GLint m_AlphaLoc;

public:

    /** constructeur, crée le VBO et le shader */
    TriangleGLSL() throw (std::string);

    /** destructeur, libère le VBO et le shader */
    ~TriangleGLSL();

    /** dessin du triangle sur l'écran */
    void onDraw();
};

#endif
