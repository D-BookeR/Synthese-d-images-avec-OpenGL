#ifndef TETRAEDRE_H
#define TETRAEDRE_H

// Définition de la classe Tetraedre

#include <gl-matrix.h>

class Tetraedre
{
private:

    /** tableau des coordonnées, x,y,z à la suite */
    GLuint m_VertexBufferId;
    GLuint m_ColorBufferId;
    static const int LINE_COUNT = 6;
    GLuint m_IndexBufferId;

    /** identifiants liés au shader */
    GLuint m_ShaderId;
    GLuint m_MatrixLoc;
    GLuint m_VertexLoc;
    GLuint m_ColorLoc;

public:

    /** constructeur, crée le VBO et le shader */
    Tetraedre() throw (std::string);

    /** destructeur, libère le VBO et le shader */
    ~Tetraedre();

    /**
     * dessiner le tétraèdre
     * @param matrix : matrice à appliquer sur l'objet
     */
    void onDraw(mat4 matrix);
};

#endif
