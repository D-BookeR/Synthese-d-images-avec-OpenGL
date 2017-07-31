#ifndef TRIANGLEROUGE_H
#define TRIANGLEROUGE_H

// Définition de la classe RedTriangle

#include <gl-matrix.h>

class RedTriangle
{
private:

    /** tableau des coordonnées, x,y,z à la suite */
    GLuint m_VertexBufferId;

    /** identifiants liés au shader */
    GLuint m_ShaderId;
    GLuint m_VertexLoc;

public:

    /** constructeur, crée le VBO et le shader */
    RedTriangle() throw (std::string);

    /** destructeur, libère le VBO et le shader */
    ~RedTriangle();

    /** dessin du triangle sur l'écran */
    void onDraw();
};

#endif
