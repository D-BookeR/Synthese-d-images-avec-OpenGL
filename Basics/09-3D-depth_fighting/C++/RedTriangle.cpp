// Définition de la classe Triangle

#include <iostream>

#include <GL/glew.h>
#include <GL/gl.h>
#include <math.h>

#include <utils.h>

#include <RedTriangle.h>



/** constructeur */
RedTriangle::RedTriangle() throw (std::string)
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
        "    glFragColor = vec4(0.84, 0.15, 0.51, 1.0);\n"
        "}";

    // compiler le shader de dessin
    m_ShaderId = Utils::makeShaderProgram(srcVertexShader, srcFragmentShader, "RedTriangle");

    // déterminer où sont les variables attribute et uniform
    m_MatrixLoc = glGetUniformLocation(m_ShaderId, "matrix");
    m_VertexLoc = glGetAttribLocation(m_ShaderId, "glVertex");

    /** VBOs */

    // créer et remplir le buffer des coordonnées
    std::vector<GLfloat> vertices {
        +0.14, +0.95, 0.0,
        -0.88, +0.52, 0.0,
        +0.63, -0.79, 0.0,
    };
    m_VertexBufferId = Utils::makeFloatVBO(vertices, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
}


/**
 * dessiner l'objet
 * @param matrix : matrice à appliquer sur l'objet
 */
void RedTriangle::onDraw(mat4 matrix)
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
RedTriangle::~RedTriangle()
{
    // supprimer le shader et le VBO
    Utils::deleteShaderProgram(m_ShaderId);
    Utils::deleteVBO(m_VertexBufferId);
}
