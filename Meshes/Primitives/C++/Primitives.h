#ifndef PRIMITIVES_H
#define PRIMITIVES_H

// Définition de la classe Primitives

#include <Mesh.h>


class Primitives
{
public:

    /**
     * Constructeur
     */
    Primitives();

    /**
     * fonction de dessin
     */
     void onDraw();

    /**
     * Destructeur
     */
    ~Primitives();

private:

    GLint m_ShaderId;

    GLint m_VertexLoc;
    GLint m_VertexBufferId;

    GLint m_ColorLoc;
    GLint m_ColorBufferId;
};

#endif
