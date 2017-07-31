package com.dbooker.livreopengl;

import static android.opengl.GLES20.GL_ARRAY_BUFFER;
import static android.opengl.GLES20.GL_ELEMENT_ARRAY_BUFFER;
import static android.opengl.GLES20.GL_FLOAT;
import static android.opengl.GLES20.GL_STATIC_DRAW;
import static android.opengl.GLES20.GL_TRIANGLES;
import static android.opengl.GLES20.GL_TRIANGLE_FAN;
import static android.opengl.GLES20.GL_TRIANGLE_STRIP;
import static android.opengl.GLES20.GL_UNSIGNED_SHORT;
import static android.opengl.GLES20.glBindBuffer;
import static android.opengl.GLES20.glDisableVertexAttribArray;
import static android.opengl.GLES20.glDrawElements;
import static android.opengl.GLES20.glEnableVertexAttribArray;
import static android.opengl.GLES20.glGetAttribLocation;
import static android.opengl.GLES20.glUseProgram;
import static android.opengl.GLES20.glVertexAttribPointer;

import com.dbooker.livreopengl.utils.Utils;

public class PrimitiveGroups
{
    private int m_ShaderId;

    private int m_VertexLoc;
    private int m_ColorLoc;

    private int m_VertexBufferId;
    private int m_ColorBufferId;

    private int m_IndexBufferId;


    public PrimitiveGroups() throws Exception
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
             -1.5f, -1.5f,
              1.0f, -2.0f,
             -2.0f, -0.5f,
              2.0f,  0.0f,
             -1.0f,  1.0f,
              1.0f,  1.5f,
             -1.5f,  2.0f,
              1.5f,  2.0f
        };
        m_VertexBufferId = Utils.makeFloatVBO(vertices, GL_ARRAY_BUFFER, GL_STATIC_DRAW);

        // créer et remplir le buffer des couleurs
        float[] colors = {
            1.0f, 0.11f, 0.11f,
            1.0f, 0.71f, 0.11f,
            0.71f, 1.0f, 0.11f,
            0.11f, 1.0f, 0.11f,
            0.11f, 1.0f, 0.72f,
            0.11f, 0.72f, 1.0f,
            0.11f, 0.11f, 1.0f,
            0.72f, 0.11f, 1.0f
        };
        m_ColorBufferId = Utils.makeFloatVBO(colors, GL_ARRAY_BUFFER, GL_STATIC_DRAW);

        // créer et remplir le buffer des indices
        short[] indexlist = {
            0,1,2, 6,2,4,  // triangles
            4,5,6,7,       // ruban
            3,7,5,4,2,1    // éventail
        };
        m_IndexBufferId = Utils.makeShortVBO(indexlist, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);
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

        // activer et lier le buffer contenant les indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferId);

        // 2 octet par indice dans le VBO m_IndexBufferId
        final int SIZEOF_USHORT = 2;

        // dessiner différentes primitives
        glDrawElements(GL_TRIANGLES,      6, GL_UNSIGNED_SHORT,  0 * SIZEOF_USHORT);
        glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT,  6 * SIZEOF_USHORT);
        glDrawElements(GL_TRIANGLE_FAN,   6, GL_UNSIGNED_SHORT, 10 * SIZEOF_USHORT);

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
    public void destroy()
    {
        Utils.deleteVBO(m_IndexBufferId);
        Utils.deleteVBO(m_ColorBufferId);
        Utils.deleteVBO(m_VertexBufferId);
        Utils.deleteShaderProgram(m_ShaderId);
    }
}
