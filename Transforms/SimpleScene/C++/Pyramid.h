#ifndef PYRAMIDE_H
#define PYRAMIDE_H

// Définition de la classe Pyramid

#include <gl-matrix.h>


class Pyramid
{
private:

    /** tableau des coordonnées, x,y,z à la suite */
    GLuint m_VertexBufferId;
    GLuint m_ColorBufferId;
    static const int TRIANGLE_COUNT = 6;
    GLuint m_IndexBufferId;

    /** identifiants liés au shader */
    GLuint m_ShaderId;
    GLuint m_MatrixLoc;
    GLuint m_VertexLoc;
    GLuint m_ColorLoc;

public:

    /**
     * Constructeur
     */
    Pyramid();

    /** Destructeur */
    ~Pyramid();

    /**
     * dessine l'objet
     * @param matProjection : matrice de projection
     * @param matView : matrice de vue
     * @param matModel : matrice de transformation du modèle
     */
    void onDraw(mat4& matProjection, mat4& matView, mat4& matModel);

public:
};

#endif
