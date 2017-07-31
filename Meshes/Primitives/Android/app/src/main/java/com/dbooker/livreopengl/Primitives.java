package com.dbooker.livreopengl;

import static android.opengl.GLES20.*;

import com.dbooker.livreopengl.utils.Utils;


public class Primitives
{
    private int m_ShaderId;

    private int m_VertexLoc;
    private int m_ColorLoc;

    private int m_VertexBufferId;
    private int m_ColorBufferId;


    public Primitives() throws Exception
    {
        /** shader */

        String srcVertexShader =
            "#version 310 es\n"+
            "in vec2 glVertex;\n"+
            "in vec3 glColor;\n"+
            "out vec3 frgColor;\n"+
            "void main()\n"+
            "{\n"+
            "    gl_Position = vec4(glVertex*0.4, 0.0, 1.0);\n"+
            "    frgColor = glColor;\n"+
            "    gl_PointSize = 10.0;\n" +
            "}";

       String srcFragmentShader =
            "#version 310 es\n"+
            "precision mediump float;\n"+
            "in vec3 frgColor;\n"+
            "out vec4 glFragColor;\n" +
            "void main()\n"+
            "{\n"+
            "  glFragColor = vec4(frgColor, 1.0);\n"+
            "}";


        // compiler le shader de dessin
        m_ShaderId = Utils.makeShaderProgram(srcVertexShader, srcFragmentShader, "shader Primitives");

        // déterminer où sont les variables attribute
        m_VertexLoc = glGetAttribLocation(m_ShaderId, "glVertex");
        m_ColorLoc = glGetAttribLocation(m_ShaderId, "glColor");

        /** VBOs */

        // créer et remplir le buffer des coordonnées
        float[] vertices = {
              0.5f,  0.0f,
              2.0f, -0.5f,
              1.5f,  2.0f,
             -0.5f,  1.5f,
             -2.0f, -0.5f,
             -0.5f, -1.5f,
              1.0f, -2.0f,
              1.5f, -1.5f
        };
        m_VertexBufferId = Utils.makeFloatVBO(vertices, GL_ARRAY_BUFFER, GL_STATIC_DRAW);

        // créer et remplir le buffer des couleurs
        float[] colors = {
            0.72f, 0.08f, 0.08f,
            0.72f, 0.51f, 0.08f,
            0.51f, 0.72f, 0.08f,
            0.08f, 0.72f, 0.08f,
            0.08f, 0.72f, 0.51f,
            0.08f, 0.51f, 0.72f,
            0.08f, 0.08f, 0.72f,
            0.51f, 0.08f, 0.72f
        };
        m_ColorBufferId = Utils.makeFloatVBO(colors, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
     }

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

        // largeur des lignes (la largeur des points est dans le MeshVertex Shader)
        glLineWidth(5.0f);

        // dessiner différentes primitives à tour de rôle
        switch ((int)Math.floor(Utils.Time/2) % 7) {
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
    public void destroy()
    {
        Utils.deleteVBO(m_ColorBufferId);
        Utils.deleteVBO(m_VertexBufferId);
        Utils.deleteShaderProgram(m_ShaderId);
    }
}
