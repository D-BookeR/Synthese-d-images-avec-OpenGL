#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <math.h>

#include <utils.h>

#include <Primitives.h>


/**
 * Constructeur
 */
Primitives::Primitives()
{
    /** shader */

    std::string srcVertexShader = "#version 300 es          \n\
        in vec2 glVertex;                                   \n\
        in vec3 glColor;                                    \n\
        out vec3 frgColor;                                  \n\
        void main()                                         \n\
        {                                                   \n\
            gl_Position = vec4(glVertex*0.4, 0.0, 1.0);     \n\
            frgColor = glColor;                             \n\
            gl_PointSize = 10.0;                            \n\
        }";

    std::string srcFragmentShader = "#version 300 es        \n\
        precision mediump float;                            \n\
        in vec3 frgColor;                                   \n\
        out vec4 glFragColor;                               \n\
        void main()                                         \n\
        {                                                   \n\
            glFragColor = vec4(frgColor, 1.0);              \n\
        }";


    // compiler le shader de dessin
    m_ShaderId = Utils::makeShaderProgram(srcVertexShader, srcFragmentShader, "Primitives");

    // déterminer où sont les variables attribute
    m_VertexLoc = glGetAttribLocation(m_ShaderId, "glVertex");
    m_ColorLoc = glGetAttribLocation(m_ShaderId, "glColor");

    /** VBOs */

    // créer et remplir le buffer des coordonnées
    std::vector<GLfloat> vertices = {
          0.5,  0.0,
          2.0, -0.5,
          1.5,  2.0,
         -0.5,  1.5,
         -2.0, -0.5,
         -0.5, -1.5,
          1.0, -2.0,
          1.5, -1.5
    };
    m_VertexBufferId = Utils::makeFloatVBO(vertices,GL_ARRAY_BUFFER, GL_STATIC_DRAW);

    // créer et remplir le buffer des couleurs

    std::vector<GLfloat> colors = {
        0.72, 0.08, 0.08,
        0.72, 0.51, 0.08,
        0.51, 0.72, 0.08,
        0.08, 0.72, 0.08,
        0.08, 0.72, 0.51,
        0.08, 0.51, 0.72,
        0.08, 0.08, 0.72,
        0.51, 0.08, 0.72
    };
    m_ColorBufferId = Utils::makeFloatVBO(colors, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
}

void Primitives::onDraw()
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

    // largeur des lignes (la largeur des points est dans le MeshVertex Shader)
    glLineWidth(5.0);
    glPointSize(10.0);  // pas en OpenGL ES, voir le vertex shader à la place

    // dessiner différentes primitives à tour de rôle
    switch ((int)floor(Utils::Time/2) % 7) {
    case 0:
        glDrawArrays(GL_POINTS,            0, 8);
        break;
    case 1:
        glDrawArrays(GL_LINES,             0, 8);
        break;
    case 2:
        glDrawArrays(GL_LINE_STRIP,        0, 8);
        break;
    case 3:
        glDrawArrays(GL_LINE_LOOP,         0, 8);
        break;
    case 4:
        glDrawArrays(GL_TRIANGLES,         0, 6);
        break;
    case 5:
        glDrawArrays(GL_TRIANGLE_STRIP,    0, 8);
        break;
    case 6:
        glDrawArrays(GL_TRIANGLE_FAN,      0, 8);
        break;
    }

    // désactiver les buffers
    glDisableVertexAttribArray(m_VertexLoc);
    glDisableVertexAttribArray(m_ColorLoc);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // désactiver le shader
    glUseProgram(0);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Primitives::~Primitives()
{
    Utils::deleteVBO(m_ColorBufferId);
    Utils::deleteVBO(m_VertexBufferId);
    Utils::deleteShaderProgram(m_ShaderId);
}
