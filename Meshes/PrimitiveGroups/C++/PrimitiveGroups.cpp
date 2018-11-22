#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <math.h>

#include <utils.h>

#include <PrimitiveGroups.h>

PrimitiveGroups::PrimitiveGroups()
{
    /** shader */

    const char* srcVertexShader =
        "#version 300 es\n"
        "in vec2 glVertex;\n"
        "in vec3 glColor;\n"
        "out vec3 frgColor;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(glVertex*0.4, 0.0, 1.0);\n"
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
    m_ShaderId = Utils::makeShaderProgram(srcVertexShader, srcFragmentShader, "PrimitiveGroups");

    // déterminer où sont les variables attribute
    m_VertexLoc = glGetAttribLocation(m_ShaderId, "glVertex");
    m_ColorLoc = glGetAttribLocation(m_ShaderId, "glColor");

    /** VBOs */

    // créer et remplir le buffer des coordonnées
    std::vector<GLfloat> vertices = {
         -1.5, -1.5,
          1.0, -2.0,
         -2.0, -0.5,
          2.0,  0.0,
         -1.0,  1.0,
          1.0,  1.5,
         -1.5,  2.0,
          1.5,  2.0
    };
    m_VertexBufferId = Utils::makeFloatVBO(vertices,GL_ARRAY_BUFFER, GL_STATIC_DRAW);

    // créer et remplir le buffer des couleurs

    std::vector<GLfloat> colors = {
        1.0,  0.11, 0.11,
        1.0,  0.71, 0.11,
        0.71, 1.0,  0.11,
        0.11, 1.0,  0.11,
        0.11, 1.0,  0.72,
        0.11, 0.72, 1.0,
        0.11, 0.11, 1.0,
        0.72, 0.11, 1.0
    };
    m_ColorBufferId = Utils::makeFloatVBO(colors, GL_ARRAY_BUFFER, GL_STATIC_DRAW);

    // créer et remplir le buffer des indices
    std::vector<GLushort> indexlist = {
        0,1,2, 6,2,4,  // triangles
        4,5,6,7,       // ruban
        3,7,5,4,2,1    // éventail
    };
    m_IndexBufferId = Utils::makeShortVBO(indexlist, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);
}


/** Fonction de dessin */
void PrimitiveGroups::onDraw()
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

    // activer et lier le buffer contenant les indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferId);

    // 2 octet par indice dans le VBO m_IndexBufferId
    const int SIZEOF_USHORT = 2;

    // dessiner différentes primitives
    glDrawElements(GL_TRIANGLES,      6, GL_UNSIGNED_SHORT, (const GLvoid*)( 0 * SIZEOF_USHORT));
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, (const GLvoid*)( 6 * SIZEOF_USHORT));
    glDrawElements(GL_TRIANGLE_FAN,   6, GL_UNSIGNED_SHORT, (const GLvoid*)(10 * SIZEOF_USHORT));

    // désactiver les buffers
    glDisableVertexAttribArray(m_VertexLoc);
    glDisableVertexAttribArray(m_ColorLoc);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // désactiver le shader
    glUseProgram(0);
}


/**
 * Cette méthode supprime les ressources allouées
 */
PrimitiveGroups::~PrimitiveGroups()
{
    Utils::deleteVBO(m_IndexBufferId);
    Utils::deleteVBO(m_ColorBufferId);
    Utils::deleteVBO(m_VertexBufferId);
    Utils::deleteShaderProgram(m_ShaderId);
}
