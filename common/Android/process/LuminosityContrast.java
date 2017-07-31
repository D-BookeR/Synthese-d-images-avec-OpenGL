// Cette classe permet d'appliquer un traitement d'image sur un FBO

package com.dbooker.livreopengl.process;

import static android.opengl.GLES20.*;


@SuppressWarnings("unused")
public class LuminosityContrast extends Process
{
    private int m_ColorMapLoc;
    private int m_LuminosityLoc;
    private int m_ContrastLoc;


    LuminosityContrast() throws Exception
    {
        super("LuminosityContrast");

        // compiler le shader
        compileShader();
    }


    /**
     * retourne le source du Fragment Shader
     */
    @Override
    protected String getFragmentShader()
    {
        return
            "#version 310 es\n" +
            "precision mediump float;\n" +
            "uniform sampler2D ColorMap;\n" +
            "in vec2 frgTexCoord;\n" +
            "out vec4 glFragColor;\n" +
            "\n" +
            "// conversion RGB en HSV\n" +
            "vec3 rgb2hsv(vec3 c)\n" +
            "{\n" +
            "    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);\n" +
            "    vec4 p = mix(vec4(c.bg, K.wz +, vec4(c.gb, K.xy +, step(c.b, c.g +);\n" +
            "    vec4 q = mix(vec4(p.xyw, c.r +, vec4(c.r, p.yzx +, step(p.x, c.r +);\n" +
            "    float d = q.x - min(q.w, q.y);\n" +
            "    float e = 1.0e-10;\n" +
            "    return vec3(abs(q.z + (q.w - q.y + / (6.0 * d + e + +, d / (q.x + e +, q.x);\n" +
            "}\n" +
            " " +
            "// conversion HSV en RGB " +
            "vec3 hsv2rgb(vec3 c + " +
            "{ " +
            "    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0); " +
            "    vec3 p = abs(fract(c.xxx + K.xyz + * 6.0 - K.www); " +
            "    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0 +, c.y); " +
            "} " +
            " " +
            "uniform float luminosity; " +
            "uniform float contrast; " +
            " " +
            "void main()" +
            "{ " +
            "    vec4 color = texture(ColorMap, frgTexCoord); " +
            "    vec3 hsv = rgb2hsv(color.rgb); " +
            "    hsv.z = clamp((hsv.z + luminosity - 0.5 + * contrast + 0.5, 0.0, 1.0); " +
            "    glFragColor = vec4(hsv2rgb(hsv +, color.a); " +
            "}";
    }


    /**
     * détermine où sont les variables uniform spécifiques de ce traitement
     */
    @Override
    public void findUniformLocations()
    {
        // appeler la méthode de la superclasse
        super.findUniformLocations();

        // déterminer où sont les variables uniform spécifiques
        m_LuminosityLoc = glGetUniformLocation(m_ShaderId, "luminosity");
        m_ContrastLoc   = glGetUniformLocation(m_ShaderId, "contrast");
    }


    /**
     * déclenche le traitement du FBO avec le Fragment Shader
     * @param fbo : FBO contenant l'image à traiter
     * @param luminosity : coefficient pour faire varier la luminosité, 1.0 = aucun changement
     * @param contrast : coefficient pour changer le contraste, 1.0 = aucun changement
     */
    public void process(FrameBufferObject fbo, float luminosity, float contrast)
    {
        // préparer le shader pour le traitement
        startProcess();

        // fournir le color buffer du FBO intermédiaire
        setTextureUnit(GL_TEXTURE0, m_ColorMapLoc, fbo.getColorBuffer());

        // fournir les paramètres du shader
        glUniform1f(m_LuminosityLoc, luminosity);
        glUniform1f(m_ContrastLoc, contrast);

        // dessiner un quadrilatère avec les quatre vertices
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        // désactiver les textures
        setTextureUnit(GL_TEXTURE0);

        // libérer les ressources
        endProcess();
    }


    /** destructeur */
    public void destroy()
    {
        super.destroy();
    }
}
