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
    static const int TRIANGLE_COUNT = 4;
    GLuint m_TrianglesIndexBufferId;
    static const int LINE_COUNT = 6;
    GLuint m_LinesIndexBufferId;

    /** identifiants liés au shader n°1 */
    GLuint m_TrianglesShaderId;
    GLuint m_TrianglesMatrixLoc;
    GLuint m_Triangles_glVertexLoc;
    GLuint m_Triangles_glColorLoc;

    /** identifiants liés au shader n°2 */
    GLuint m_LinesShaderId;
    GLuint m_LinesMatrixLoc;
    GLuint m_LinesVertexLoc;

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
