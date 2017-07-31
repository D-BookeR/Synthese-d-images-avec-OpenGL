package com.dbooker.livreopengl;

import static android.opengl.GLES20.*;

import com.dbooker.livreopengl.utils.Utils;

public class GreenTriangle
{
    /** identifiants des VBOs */
    int m_VertexBufferId;

    /** identifiants liés au shader */
    int m_ShaderId;
    int m_VertexLoc;


    public GreenTriangle() throws Exception
    {
        /** shader */

        final String srcVertexShader =
            "#version 310 es\n" +
            "in vec3 glVertex;\n" +
            "void main()\n" +
            "{\n" +
            "   gl_Position = vec4(glVertex, 1.0);\n" +
            "}";

        final String srcFragmentShader =
            "#version 310 es\n" +
            "precision mediump float;\n" +
            "out vec4 glFragColor;\n" +
            "void main()\n" +
            "{\n" +
            "   glFragColor = vec4(0.40, 0.75, 0.11, 1.0);\n" +
            "}";

        // compiler le shader de dessin
        m_ShaderId = Utils.makeShaderProgram(srcVertexShader, srcFragmentShader, "GreenTriangle");

        // déterminer où sont les variables attribute
        m_VertexLoc = glGetAttribLocation(m_ShaderId, "glVertex");

        /** VBOs */

        // créer et remplir le buffer des coordonnées
        float vertices[] = {
            +0.86f, +0.65f, -0.5f,
            -0.04f, +0.57f, -0.5f,
            -0.40f, -0.92f, -0.5f,
        };
        m_VertexBufferId = Utils.makeFloatVBO(vertices, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
    }


    /** dessiner l'objet */
    public void onDraw()
    {
        // activer le shader
        glUseProgram(m_ShaderId);

        // activer et lier le buffer contenant les coordonnées
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
        glEnableVertexAttribArray(m_VertexLoc);
        glVertexAttribPointer(m_VertexLoc, Utils.VEC3, GL_FLOAT, false, 0, 0);

        // dessiner un triangle avec les trois vertices
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
