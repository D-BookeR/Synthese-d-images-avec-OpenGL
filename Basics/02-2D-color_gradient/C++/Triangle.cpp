// Définition de la classe Triangle

#include <iostream>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>

#include <utils.h>

#include <Triangle.h>



/** constructeur */
Triangle::Triangle() throw (std::string)
{
    /// Shader
    const char* srcVertexShader =
        "#version 300 es\n"
        "in vec2 glVertex;\n"
        "in vec3 glColor;\n"
        "out vec3 frgColor;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(glVertex, 0.0, 1.0);\n"
        "    frgColor = glColor;\n"
        "}";

    const char* srcFragmentShader =
        "#version 300 es\n"
        "precision mediump float;\n"
        "in vec3 frgColor;\n"
        "out vec4 glFragColor;\n"
        "void main()\n"
        "{\n"
        "    glFragColor = vec4(frgColor, 1.0);\n"
        "}";

    // compiler le shader de dessin
    m_ShaderId = Utils::makeShaderProgram(srcVertexShader, srcFragmentShader, "Triangle");

    // déterminer où sont les variables attribute
    m_VertexLoc = glGetAttribLocation(m_ShaderId, "glVertex");
    m_ColorLoc = glGetAttribLocation(m_ShaderId, "glColor");

    /** VBOs */

    // créer et remplir le buffer des coordonnées
    std::vector<GLfloat> vertices {
        -0.88, +0.52,       // P0
        +0.63, -0.79,       // P1
        +0.14, +0.95,       // P2
    };
    m_VertexBufferId = Utils::makeFloatVBO(vertices, GL_ARRAY_BUFFER, GL_STATIC_DRAW);

    // créer et remplir le buffer des couleurs
    std::vector<GLfloat> colors {
        1.0, 0.0, 0.5,      // P0
        0.0, 0.0, 1.0,      // P1
        1.0, 1.0, 0.0,      // P2
    };
    m_ColorBufferId = Utils::makeFloatVBO(colors, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
}


/** dessiner l'objet */
void Triangle::onDraw()
{
    // activer le shader
    glUseProgram(m_ShaderId);

    // activer et lier le buffer contenant les coordonnées
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
    glEnableVertexAttribArray(m_VertexLoc);
    glVertexAttribPointer(m_VertexLoc, Utils::VEC2, GL_FLOAT, GL_FALSE, 0, 0);

    // activer et lier le buffer contenant les couleurs
    glBindBuffer(GL_ARRAY_BUFFER, m_ColorBufferId);
    glEnableVertexAttribArray(m_ColorLoc);
    glVertexAttribPointer(m_ColorLoc, Utils::VEC3, GL_FLOAT, GL_FALSE, 0, 0);

    // dessiner un triangle avec les trois vertices
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // désactiver les buffers
    glDisableVertexAttribArray(m_VertexLoc);
    glDisableVertexAttribArray(m_ColorLoc);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // désactiver le shader
    glUseProgram(0);
}


/** destructeur */
Triangle::~Triangle()
{
    // supprimer le shader et les VBOs
    Utils::deleteShaderProgram(m_ShaderId);
    Utils::deleteVBO(m_VertexBufferId);
    Utils::deleteVBO(m_ColorBufferId);
}
