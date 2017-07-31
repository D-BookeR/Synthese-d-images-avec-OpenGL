/**
 * Cette classe permet de dessiner l'arrière plan uniforme en mode Deferred Shading
 * Le shader colore les zones non dessinées du FBO mrt (la coordonnée w des fragments vaut 0)
 */

package com.dbooker.livreopengl.material;

import static android.opengl.GLES20.*;

import java.nio.FloatBuffer;

import com.dbooker.livreopengl.glmatrix.vec3;


@SuppressWarnings("unused")
public class SkyBackground extends Light
{
    /**
     * constructeur
     * @throws Exception
     */
    public SkyBackground() throws Exception
    {
        // récupérer la couleur d'effacement
        FloatBuffer color = FloatBuffer.allocate(4);
        glGetFloatv(GL_COLOR_CLEAR_VALUE, color);
        m_Color = vec3.fromValues(color.get(0), color.get(1), color.get(2));

        // compiler le shader
        compileShader();
    }


    /** destructeur */
    public void destroy()
    {
        super.destroy();
    }


    /**
     * construit le Fragment Shader qui colore les zones non dessinées du FBO mrt
     */
    @Override
    protected String getFragmentShader()
    {
        return
            "#version 310 es\n" +
            "precision mediump float;\n" +
            "uniform sampler2D MapPosition;\n" +
            "in vec2 frgTexCoord;\n" +
            "uniform vec3 LightColor;\n" +
            "out vec4 glFragColor;\n" +
            "\n" +
            "void main()\n" +
            "{\n" +
            "    vec4 position = texture(MapPosition, frgTexCoord);\n" +
            "    if (position.w > 0.8) discard;\n" +
            "    gl_FragDepth = 1.0;\n" +
            "    glFragColor = vec4(LightColor, 1.0);\n" +
            "}";
    }
}
