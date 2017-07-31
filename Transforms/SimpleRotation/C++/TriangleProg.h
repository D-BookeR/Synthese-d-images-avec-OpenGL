#ifndef TRIANGLEPROG_H
#define TRIANGLEPROG_H

// Définition de la classe TriangleProg

#include <gl-matrix.h>

class TriangleProg
{
private:

    /** tableau des coordonnées, x,y,z à la suite */
    GLuint m_VertexBufferId;
    GLuint m_ColorBufferId;

    /** identifiants liés au shader */
    GLuint m_ShaderId;
    GLint m_VertexLoc;
    GLint m_ColorLoc;
    GLint m_RotationLoc;

public:

    /** constructeur, crée le VBO et le shader */
    TriangleProg() throw (std::string);

    /** destructeur, libère le VBO et le shader */
    ~TriangleProg();

    /** dessin du triangle sur l'écran */
    void onDraw();
};

#endif
