#ifndef TRIANGLE_H
#define TRIANGLE_H

// Définition de la classe Triangle

#include <gl-matrix.h>

class Triangle
{
private:

    /** tableau des coordonnées, x,y,z à la suite */
    GLuint m_VertexBufferId;
    GLuint m_ColorBufferId;

    /** identifiants liés au shader */
    GLuint m_ShaderId;
    GLuint m_VertexLoc;
    GLuint m_ColorLoc;

public:

    /** constructeur, crée le VBO et le shader */
    Triangle() throw (std::string);

    /** destructeur, libère le VBO et le shader */
    ~Triangle();

    /** dessin du triangle sur l'écran */
    void onDraw();
};

#endif
