package com.dbooker.livreopengl;

import static android.opengl.GLES20.*;

import com.dbooker.livreopengl.utils.Utils;

public class Triangle
{
    /** identifiants liés aux VBOs */
    int m_VertexBufferId;
    int m_ColorBufferId;

    /** identifiants liés au shader */
    int m_ShaderId;
    int m_VertexLoc;
    int m_ColorLoc;


    public Triangle() throws Exception
    {
        /// Shader
        final String srcVertexShader =
            "#version 310 es\n"+
            "in vec2 glVertex;\n"+
            "in vec3 glColor;\n"+
            "out vec3 frgColor;\n"+
            "void main()\n"+
            "{\n"+
            "   gl_Position = vec4(glVertex, 0.0, 1.0);\n"+
            "   frgColor = glColor;\n"+
            "}";

        final String srcFragmentShader =
            "#version 310 es\n"+
            "precision mediump float;\n"+
            "in vec3 frgColor;\n"+
            "out vec4 glFragColor;\n" +
            "void main()\n"+
            "{\n"+
            "   glFragColor = vec4(frgColor, 1.0);\n"+
            "}";

        // compiler le shader de dessin
        m_ShaderId = Utils.makeShaderProgram(srcVertexShader, srcFragmentShader, "Triangle");

        // déterminer où sont les variables attribute
        m_VertexLoc = glGetAttribLocation(m_ShaderId, "glVertex");
        m_ColorLoc = glGetAttribLocation(m_ShaderId, "glColor");

        /** VBOs */

        // créer et remplir le buffer des coordonnées
        float vertices[] = {
            -0.88f, +0.52f,       // P0
            +0.63f, -0.79f,       // P1
            +0.14f, +0.95f,       // P2
        };
        m_VertexBufferId = Utils.makeFloatVBO(vertices, GL_ARRAY_BUFFER, GL_STATIC_DRAW);

        // créer et remplir le buffer des couleurs
        float colors[] = {
            1.0f, 0.0f, 0.5f,      // P0
            0.0f, 0.0f, 1.0f,      // P1
            1.0f, 1.0f, 0.0f,      // P2
        };
        m_ColorBufferId = Utils.makeFloatVBO(colors, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
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

        // activer et lier le buffer contenant les couleurs
        glBindBuffer(GL_ARRAY_BUFFER, m_ColorBufferId);
        glEnableVertexAttribArray(m_ColorLoc);
        glVertexAttribPointer(m_ColorLoc, Utils.VEC3, GL_FLOAT, false, 0, 0);

        // dessiner un triangle avec les trois vertices
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // désactiver les buffers
        glDisableVertexAttribArray(m_VertexLoc);
        glDisableVertexAttribArray(m_ColorLoc);
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
    }
}
