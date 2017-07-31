package com.dbooker.livreopengl;

import static android.opengl.GLES20.*;

import com.dbooker.livreopengl.utils.Utils;

public class RedTriangle
{
    /** identifiants liés aux VBOs */
    int m_VertexBufferId;

    /** identifiants liés au shader */
    int m_ShaderId;
    int m_VertexLoc;


    public RedTriangle() throws Exception
    {
        /** shader */

        final String srcVertexShader =
            "#version 310 es\n"+
            "in vec2 glVertex;\n"+
            "void main()\n"+
            "{\n"+
            "   gl_Position = vec4(glVertex, 0.0, 1.0);\n"+
            "}";

        final String srcFragmentShader =
            "#version 310 es\n"+
            "precision mediump float;\n"+
            "out vec4 glFragColor;\n" +
            "void main()\n"+
            "{\n"+
            "   glFragColor = vec4(0.84, 0.15, 0.51, 1.0);\n"+
            "}";

        // compiler le shader de dessin
        m_ShaderId = Utils.makeShaderProgram(srcVertexShader, srcFragmentShader, "RedTriangle");

        // déterminer où sont les variables attribute
        m_VertexLoc = glGetAttribLocation(m_ShaderId, "glVertex");


        /** VBOs */

        // créer et remplir le buffer des coordonnées
        float vertices[] = {
            -0.88f, +0.52f,       // P0
            +0.63f, -0.79f,       // P1
            +0.14f, +0.95f,       // P2
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
        glVertexAttribPointer(m_VertexLoc, Utils.VEC2, GL_FLOAT, false, 0, 0);

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
