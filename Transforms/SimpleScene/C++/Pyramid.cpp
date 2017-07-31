// Définition de la classe Pyramid

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>
#include <math.h>

#include <utils.h>

#include <Pyramid.h>


/**
 * Constructeur
 */
Pyramid::Pyramid()
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
        "  glFragColor = vec4(frgColor, 0.9); // transparence légère\n"
        "}";

    // compiler le shader de dessin
    m_ShaderId = Utils::makeShaderProgram(srcVertexShader, srcFragmentShader, "Tetraedre");

    // déterminer où sont les variables attribute et uniform
    m_MatrixLoc = glGetUniformLocation(m_ShaderId, "matrix");
    m_VertexLoc = glGetAttribLocation(m_ShaderId, "glVertex");
    m_ColorLoc = glGetAttribLocation(m_ShaderId, "glColor");

    /** VBOs */

    // créer et remplir le buffer des coordonnées
    std::vector<GLfloat> vertices {
         -1.0,  0.0,  0.0,
          0.0,  0.0,  1.0,
          1.0,  0.0,  0.0,
          0.0,  0.0, -1.0,
          0.0,  2.0,  0.0
    };
    m_VertexBufferId = Utils::makeFloatVBO(vertices, GL_ARRAY_BUFFER, GL_STATIC_DRAW);

    // créer et remplir le buffer des couleurs
    std::vector<GLfloat> colors {
         0.5, 0.0, 1.0,
         0.0, 1.0, 1.0,
         1.0, 1.0, 0.0,
         1.0, 0.0, 0.0,
         0.0, 1.0, 0.0
    };
    m_ColorBufferId = Utils::makeFloatVBO(colors, GL_ARRAY_BUFFER, GL_STATIC_DRAW);

    // créer et remplir le buffer des indices
    std::vector<GLushort> indexlist {
        0, 1, 4,
        1, 2, 4,
        2, 3, 4,
        3, 0, 4,
        0, 3, 1,
        3, 2, 1
    };
    m_IndexBufferId = Utils::makeShortVBO(indexlist, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);
}


/**
 * dessine l'objet
 * @param matProjection : matrice de projection
 * @param matView : matrice de vue
 * @param matModel : matrice de transformation du modèle
 */
void Pyramid::onDraw(mat4& matProjection, mat4& matView, mat4& matModel)
{
    // activer la transparence
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // activer le shader
    glUseProgram(m_ShaderId);

    // calculer la matrice P * V * M
    mat4 matrix = mat4::create();
    mat4::multiply(matrix, matProjection, matView);
    mat4::multiply(matrix, matrix, matModel);

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

    // activer et lier le buffer contenant les indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferId);

    // dessiner les triangles
    glDrawElements(GL_TRIANGLES, TRIANGLE_COUNT * 3, GL_UNSIGNED_SHORT, 0);

    // désactiver les buffers
    glDisableVertexAttribArray(m_VertexLoc);
    glDisableVertexAttribArray(m_ColorLoc);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // désactiver le shader
    glUseProgram(0);

    // désactiver la transparence
    glDisable(GL_BLEND);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Pyramid::~Pyramid()
{
    Utils::deleteVBO(m_IndexBufferId);
    Utils::deleteVBO(m_ColorBufferId);
    Utils::deleteVBO(m_VertexBufferId);
    Utils::deleteShaderProgram(m_ShaderId);
}
