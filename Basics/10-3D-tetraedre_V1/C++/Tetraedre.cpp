// Définition de la classe Tetraedre

#include <iostream>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>

#include <utils.h>

#include <Tetraedre.h>



/** constructeur */
Tetraedre::Tetraedre() throw (std::string)
{
    /// Shader
    const char* srcVertexShader =
        "#version 300 es\n"
        "uniform mat4 matrix;\n"
        "in vec3 glVertex;\n"
        "in vec3 glColor;\n"
        "out vec3 frgColor;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = matrix * vec4(glVertex, 1.0);\n"
        "    frgColor = glColor;\n"
        "}";

    const char* srcFragmentShader =
        "#version 300 es\n"
        "precision mediump float;\n"
        "in vec3 frgColor;\n"
        "out vec4 glFragColor;\n"
        "void main()\n"
        "{\n"
        "  glFragColor = vec4(frgColor, 1.0);\n"
        "}";

    // compiler le shader de dessin
    m_ShaderId = Utils::makeShaderProgram(srcVertexShader, srcFragmentShader, "Tetraedre");

    // déterminer où sont les variables attribute et uniform
    m_MatrixLoc = glGetUniformLocation(m_ShaderId, "matrix");
    m_VertexLoc = glGetAttribLocation(m_ShaderId, "glVertex");
    m_ColorLoc = glGetAttribLocation(m_ShaderId, "glColor");

    /** VBOs */

    // créer et remplir le buffer des coordonnées
    const float b = 0.5;
    std::vector<GLfloat> vertices {
        -b, +b, +b,     // P0 bleu
        +b, -b, +b,     // P1 orange
        +b, +b, -b,     // P2 vert

        -b, +b, +b,     // P0 bleu
        +b, -b, +b,     // P1 orange
        -b, -b, -b,     // P3 violet

        +b, -b, +b,     // P1 orange
        +b, +b, -b,     // P2 vert
        -b, -b, -b,     // P3 violet

        -b, +b, +b,     // P0 bleu
        +b, +b, -b,     // P2 vert
        -b, -b, -b,     // P3 violet
    };
    m_VertexBufferId = Utils::makeFloatVBO(vertices, GL_ARRAY_BUFFER, GL_STATIC_DRAW);

    // créer et remplir le buffer des couleurs
    std::vector<GLfloat> colors {
        0.0, 0.5, 1.0,  // P0 bleu
        1.0, 0.5, 0.0,  // P1 orange
        0.0, 1.0, 0.0,  // P2 vert

        0.0, 0.5, 1.0,  // P0 bleu
        1.0, 0.5, 0.0,  // P1 orange
        0.7, 0.0, 0.7,  // P3 violet

        1.0, 0.5, 0.0,  // P1 orange
        0.0, 1.0, 0.0,  // P2 vert
        0.7, 0.0, 0.7,  // P3 violet

        0.0, 0.5, 1.0,  // P0 bleu
        0.0, 1.0, 0.0,  // P2 vert
        0.7, 0.0, 0.7,  // P3 violet
    };
    m_ColorBufferId = Utils::makeFloatVBO(colors, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
}


/**
 * dessiner l'objet
 * @param matrix : matrice à appliquer sur l'objet
 */
void Tetraedre::onDraw(mat4 matrix)
{
    // activer le shader
    glUseProgram(m_ShaderId);

    // fournir la matrice au shader
    mat4::glUniformMatrix(m_MatrixLoc, matrix);

    // activer et lier le buffer contenant les coordonnées
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
    glEnableVertexAttribArray(m_VertexLoc);
    glVertexAttribPointer(m_VertexLoc, Utils::VEC3, GL_FLOAT, GL_FALSE, 0, 0);

    // activer et lier le buffer contenant les couleurs
    glBindBuffer(GL_ARRAY_BUFFER, m_ColorBufferId);
    glEnableVertexAttribArray(m_ColorLoc);
    glVertexAttribPointer(m_ColorLoc, Utils::VEC3, GL_FLOAT, GL_FALSE, 0, 0);

    // dessiner les 4 triangles
    glDrawArrays(GL_TRIANGLES, 0, TRIANGLE_COUNT * 3);

    // désactiver les buffers
    glDisableVertexAttribArray(m_VertexLoc);
    glDisableVertexAttribArray(m_ColorLoc);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // désactiver le shader
    glUseProgram(0);
}


/** destructeur */
Tetraedre::~Tetraedre()
{
    // supprimer le shader et les VBOs
    Utils::deleteShaderProgram(m_ShaderId);
    Utils::deleteVBO(m_VertexBufferId);
    Utils::deleteVBO(m_ColorBufferId);
}
