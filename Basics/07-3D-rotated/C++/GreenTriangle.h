#ifndef TRIANGLEVERT_H
#define TRIANGLEVERT_H

// Définition de la classe GreenTriangle

#include <gl-matrix.h>

class GreenTriangle
{
private:

    /** tableau des coordonnées, x,y,z à la suite */
    GLuint m_VertexBufferId;

    /** identifiants liés au shader */
    GLuint m_ShaderId;
    GLuint m_MatrixLoc;
    GLuint m_VertexLoc;

public:

    /** constructeur, crée le VBO et le shader */
    GreenTriangle() throw (std::string);

    /** destructeur, libère le VBO et le shader */
    ~GreenTriangle();

    /**
     * dessiner l'objet
     * @param matrix : matrice à appliquer sur l'objet
     */
    void onDraw(mat4 matrix);
};

#endif
