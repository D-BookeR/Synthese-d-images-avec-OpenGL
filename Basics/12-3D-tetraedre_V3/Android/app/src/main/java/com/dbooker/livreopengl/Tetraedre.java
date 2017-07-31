package com.dbooker.livreopengl;

import static android.opengl.GLES20.*;

import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.utils.Utils;

public class Tetraedre
{
    /** identifiants liés aux VBOs */
    int m_VertexBufferId;
    int m_ColorBufferId;
    final int LINE_COUNT = 6;
    int m_IndexBufferId;

    /** identifiants liés au shader */
    int m_ShaderId;
    int m_MatrixLoc;
    int m_VertexLoc;
    int m_ColorLoc;


    public Tetraedre() throws Exception
    {
        /** shader */

        final String srcVertexShader =
            "#version 310 es\n"+
            "uniform mat4 matrix;\n" +
            "in vec3 glVertex;\n" +
            "in vec3 glColor;\n" +
            "out vec3 frgColor;\n" +
            "void main()\n" +
            "{\n" +
            "   gl_Position = matrix * vec4(glVertex, 1.0);\n" +
            "   frgColor = glColor;\n" +
            "}";

        final String srcFragmentShader =
            "#version 310 es\n"+
            "precision mediump float;\n" +
            "in vec3 frgColor;\n" +
            "out vec4 glFragColor;\n" +
            "void main()\n" +
            "{\n" +
            "   glFragColor = vec4(frgColor, 1.0);\n" +
            "}";

        // compiler le shader de dessin
        m_ShaderId = Utils.makeShaderProgram(srcVertexShader, srcFragmentShader, "shader Tetraedre");

        // déterminer où sont les variables attribute et uniform
        m_MatrixLoc = glGetUniformLocation(m_ShaderId, "matrix");
        m_VertexLoc = glGetAttribLocation(m_ShaderId, "glVertex");
        m_ColorLoc = glGetAttribLocation(m_ShaderId, "glColor");

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
        short indexlist[] = {
            0, 1,
            0, 2,
            0, 3,
            1, 2,
            1, 3,
            2, 3,
        };
        m_IndexBufferId = Utils.makeShortVBO(indexlist, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);
    }


    /**
     * dessiner l'objet
     * @param matrix : matrice à appliquer sur l'objet
     */
    public void onDraw(mat4 matrix)
    {
        // activer le shader
        glUseProgram(m_ShaderId);

        // fournir la matrice au shader
        mat4.glUniformMatrix(m_MatrixLoc, matrix);

        // activer et lier le buffer contenant les coordonnées
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
        glEnableVertexAttribArray(m_VertexLoc);
        glVertexAttribPointer(m_VertexLoc, Utils.VEC3, GL_FLOAT, false, 0, 0);

        // activer et lier le buffer contenant les couleurs
        glBindBuffer(GL_ARRAY_BUFFER, m_ColorBufferId);
        glEnableVertexAttribArray(m_ColorLoc);
        glVertexAttribPointer(m_ColorLoc, Utils.VEC3, GL_FLOAT, false, 0, 0);

        // activer et lier le buffer contenant les indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferId);

        // dessiner les lignes
        glLineWidth(3.0f);
        glDrawElements(GL_LINES, LINE_COUNT * 2, GL_UNSIGNED_SHORT, 0);

        // désactiver les buffers
        glDisableVertexAttribArray(m_VertexLoc);
        glDisableVertexAttribArray(m_ColorLoc);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        // désactiver le shader
        glUseProgram(0);
    }


    /** destructeur */
    public void destroy()
    {
        // supprimer le shader et les VBOs
        Utils.deleteShaderProgram(m_ShaderId);
        Utils.deleteVBO(m_VertexBufferId);
        Utils.deleteVBO(m_ColorBufferId);
        Utils.deleteVBO(m_IndexBufferId);
    }
}
