package com.dbooker.livreopengl;

import static android.opengl.GLES20.*;

import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.utils.Utils;

public class RedTriangle
{
    /** identifiants liés aux VBOs */
    int m_VertexBufferId;

    /** identifiants liés au shader */
    int m_ShaderId;
    int m_MatrixLoc;
    int m_VertexLoc;


    public RedTriangle() throws Exception
    {
        /** shader */

        final String srcVertexShader =
            "#version 310 es\n"+
            "uniform mat4 matrix;\n" +
            "in vec3 glVertex;\n" +
            "void main()\n" +
            "{\n" +
            "   gl_Position = matrix * vec4(glVertex, 1.0);\n" +
            "}";

        final String srcFragmentShader =
            "#version 310 es\n"+
            "precision mediump float;\n" +
            "out vec4 glFragColor;\n" +
            "void main()\n" +
            "{\n" +
            "   glFragColor = vec4(0.84, 0.15, 0.51, 1.0);\n" +
            "}";

        // compiler le shader de dessin
        m_ShaderId = Utils.makeShaderProgram(srcVertexShader, srcFragmentShader, "RedTriangle");

        // déterminer où sont les variables attribute et uniform
        m_MatrixLoc = glGetUniformLocation(m_ShaderId, "matrice");
        m_VertexLoc = glGetAttribLocation(m_ShaderId, "glVertex");

        /** VBOs */

        // créer et remplir le buffer des coordonnées
        float vertices[] = {
            +0.14f, +0.95f, +0.5f,
            -0.88f, +0.52f,  0.0f,
            +0.63f, -0.79f, -0.5f,
        };
        m_VertexBufferId = Utils.makeFloatVBO(vertices, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
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

        // dessiner l'objet
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // désactiver les buffers
        glDisableVertexAttribArray(m_VertexLoc);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        // désactiver le shader
        glUseProgram(0);
    }


    /** destructeur */
    public void destroy()
    {
        // supprimer le shader et le VBO
        Utils.deleteShaderProgram(m_ShaderId);
        Utils.deleteVBO(m_VertexBufferId);
    }
}
