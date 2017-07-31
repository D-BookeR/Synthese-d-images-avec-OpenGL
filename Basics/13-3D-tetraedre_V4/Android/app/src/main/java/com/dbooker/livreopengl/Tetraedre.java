package com.dbooker.livreopengl;

import static android.opengl.GLES20.*;

import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.utils.Utils;

public class Tetraedre
{
    /** identifiants liés aux VBOs */
    int m_VertexBufferId;
    int m_ColorBufferId;
    final int TRIANGLE_COUNT = 4;
    int m_TrianglesIndexBufferId;
    final int LINE_COUNT = 6;
    int m_LinesIndexBufferId;

    /** identifiants liés au shader des triangles */
    int m_TrianglesShaderId;
    int m_TrianglesMatrixLoc;
    int m_Triangles_glVertexLoc;
    int m_Triangles_glColorLoc;

    /** identifiants liés au shader des lignes */
    int m_LinesShaderId;
    int m_LinesMatrixLoc;
    int m_LinesVertexLoc;


    public Tetraedre() throws Exception
    {
        /** shader des triangles */

        final String Triangles_vs =
            "#version 310 es\n" +
            "uniform mat4 matrix;\n" +
            "in vec3 glVertex;\n" +
            "in vec3 glColor;\n" +
            "out vec3 frgColor;\n" +
            "void main()\n" +
            "{\n" +
            "   gl_Position = matrix * vec4(glVertex, 1.0);\n" +
            "   frgColor = glColor;\n" +
            "}";

        final String Triangles_fs =
            "#version 310 es\n" +
            "precision mediump float;\n" +
            "in vec3 frgColor;\n" +
            "out vec4 glFragColor;\n" +
            "void main()\n" +
            "{\n" +
            "   glFragColor = vec4(frgColor, 1.0);\n" +
            "}";
        // compiler le shader de dessin des triangles
        m_TrianglesShaderId = Utils.makeShaderProgram(Triangles_vs, Triangles_fs, "shader Tetraedre Triangles");

        // déterminer où sont les variables attribute et uniform
        m_TrianglesMatrixLoc = glGetUniformLocation(m_TrianglesShaderId, "matrix");
        m_Triangles_glVertexLoc = glGetAttribLocation(m_TrianglesShaderId, "glVertex");
        m_Triangles_glColorLoc = glGetAttribLocation(m_TrianglesShaderId, "glColor");

        /** shader des lignes */

        final String Lignes_vs =
            "#version 310 es\n" +
            "uniform mat4 matrix;\n" +
            "in vec3 glVertex;\n" +
            "void main()\n" +
            "{\n" +
            "   gl_Position = matrix * vec4(glVertex, 1.0);\n" +
            "}";

        final String Lignes_fs =
            "#version 310 es\n" +
            "precision mediump float;\n" +
            "out vec4 glFragColor;\n" +
            "void main()\n" +
            "{\n" +
            "   glFragColor = vec4(1.0, 1.0, 1.0, 1.0);\n" +
            "}";

        // compiler le shader de dessin des lignes
        m_LinesShaderId = Utils.makeShaderProgram(Lignes_vs, Lignes_fs, "shader Tetraedre Lignes");

        // déterminer où sont les variables attribute et uniform
        m_LinesMatrixLoc = glGetUniformLocation(m_LinesShaderId, "matrix");
        m_LinesVertexLoc = glGetAttribLocation(m_LinesShaderId, "glVertex");

        /** VBOs */

        // créer et remplir le buffer des coordonnées
        final float b = 0.5f;
        float vertices[] = {
            -b, +b, +b,     // P0 bleu
            +b, -b, +b,     // P1 orange
            +b, +b, -b,     // P2 vert
            -b, -b, -b,     // P3 violet
        };
        m_VertexBufferId = Utils.makeFloatVBO(vertices, GL_ARRAY_BUFFER, GL_STATIC_DRAW);

        // créer et remplir le buffer des couleurs
        float colors[] = {
            0.0f, 0.5f, 1.0f,  // P0 bleu
            1.0f, 0.5f, 0.0f,  // P1 orange
            0.0f, 1.0f, 0.0f,  // P2 vert
            0.7f, 0.0f, 0.7f,  // P3 violet
        };
        m_ColorBufferId = Utils.makeFloatVBO(colors, GL_ARRAY_BUFFER, GL_STATIC_DRAW);

        // créer et remplir le buffer des indices
        short indicesT[] = {
            0, 1, 2,
            0, 1, 3,
            1, 2, 3,
            0, 2, 3,
        };
        m_TrianglesIndexBufferId = Utils.makeShortVBO(indicesT, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);

        // créer et remplir le buffer des indices
        short indicesL[] = {
            0, 1,
            0, 2,
            0, 3,
            1, 2,
            1, 3,
            2, 3,
        };
        m_LinesIndexBufferId = Utils.makeShortVBO(indicesL, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);
    }


    /**
     * dessiner l'objet
     * @param matrix : matrice à appliquer sur l'objet
     */
    public void onDraw(mat4 matrix)
    {
        // activer le shader des triangles
        glUseProgram(m_TrianglesShaderId);

        // fournir la matrice au shader
        mat4.glUniformMatrix(m_TrianglesMatrixLoc, matrix);

        // activer et lier le buffer contenant les coordonnées
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
        glEnableVertexAttribArray(m_Triangles_glVertexLoc);
        glVertexAttribPointer(m_Triangles_glVertexLoc, Utils.VEC3, GL_FLOAT, false, 0, 0);

        // activer et lier le buffer contenant les couleurs
        glBindBuffer(GL_ARRAY_BUFFER, m_ColorBufferId);
        glEnableVertexAttribArray(m_Triangles_glColorLoc);
        glVertexAttribPointer(m_Triangles_glColorLoc, Utils.VEC3, GL_FLOAT, false, 0, 0);

        // activer et lier le buffer contenant les indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_TrianglesIndexBufferId);

        // dessiner les triangles
        glDrawElements(GL_TRIANGLES, TRIANGLE_COUNT * 3, GL_UNSIGNED_SHORT, 0);

        // désactiver les buffers qui ne sont plus utilisés
        glDisableVertexAttribArray(m_Triangles_glColorLoc);


        // activer le shader des lignes
        glUseProgram(m_LinesShaderId);

        // fournir la matrice au shader
        mat4.glUniformMatrix(m_LinesMatrixLoc, matrix);

        // activer et lier le buffer contenant les coordonnées
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
        glEnableVertexAttribArray(m_LinesVertexLoc);
        glVertexAttribPointer(m_LinesVertexLoc, Utils.VEC3, GL_FLOAT, false, 0, 0);

        // activer et lier le buffer contenant les indices des lignes
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_LinesIndexBufferId);

        // dessiner les lignes
        glLineWidth(3.0f);
        glDrawElements(GL_LINES, LINE_COUNT * 2, GL_UNSIGNED_SHORT, 0);

        // désactiver les buffers
        glDisableVertexAttribArray(m_LinesVertexLoc);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        // désactiver le shader
        glUseProgram(0);
    }


    /** destructeur */
    public void destroy()
    {
        // supprimer les shaders et les VBOs
        Utils.deleteShaderProgram(m_TrianglesShaderId);
        Utils.deleteShaderProgram(m_LinesShaderId);
        Utils.deleteVBO(m_VertexBufferId);
        Utils.deleteVBO(m_ColorBufferId);
        Utils.deleteVBO(m_TrianglesIndexBufferId);
        Utils.deleteVBO(m_LinesIndexBufferId);
    }
}
