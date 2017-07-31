// Définition de la classe GreenTriangle

#include <iostream>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>

#include <utils.h>

#include <GreenTriangle.h>


/** contructeur */
GreenTriangle::GreenTriangle() throw (std::string)
{
    /// Shader
    const char* srcVertexShader =
        "#version 300 es\n"
        "uniform mat4 matrix;\n"
        "in vec3 glVertex;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = matrix * vec4(glVertex, 1.0);\n"
        "}";

    const char* srcFragmentShader =
        "#version 300 es\n"
        "precision mediump float;\n"
        "out vec4 glFragColor;\n"
        "void main()\n"
        "{\n"
        "    glFragColor = vec4(0.40, 0.75, 0.11, 1.0);\n"
        "}";

    // compiler le shader de dessin
    m_ShaderId = Utils::makeShaderProgram(srcVertexShader, srcFragmentShader, "GreenTriangle");

    // déterminer où sont les variables attribute et uniform
    m_MatrixLoc = glGetUniformLocation(m_ShaderId, "matrix");
    m_VertexLoc = glGetAttribLocation(m_ShaderId, "glVertex");

    /** VBOs */

    // créer et remplir le buffer des coordonnées
    std::vector<GLfloat> vertices {
        +0.86, +0.65, 0.0,
        -0.04, +0.57, 0.0,
        -0.40, -0.92, 0.0,
    };
    m_VertexBufferId = Utils::makeFloatVBO(vertices, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
}


/**
 * dessiner l'objet
 * @param matrix : matrice à appliquer sur l'objet
 */
void GreenTriangle::onDraw(mat4 matrix)
{
    // activer le shader
    glUseProgram(m_ShaderId);

    // fournir la matrice au shader
    mat4::glUniformMatrix(m_MatrixLoc, matrix);

    // activer et lier le buffer contenant les coordonnées
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
    glEnableVertexAttribArray(m_VertexLoc);
    glVertexAttribPointer(m_VertexLoc, Utils::VEC3, GL_FLOAT, GL_FALSE, 0, 0);

    // dessiner un triangle avec les trois vertices
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // désactiver le buffer
    glDisableVertexAttribArray(m_VertexLoc);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // désactiver le shader
    glUseProgram(0);
}


/** destructeur */
GreenTriangle::~GreenTriangle()
{
    // supprimer le shader et le VBO
    Utils::deleteShaderProgram(m_ShaderId);
    Utils::deleteVBO(m_VertexBufferId);
}
