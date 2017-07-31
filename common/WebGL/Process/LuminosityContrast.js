// Cette classe permet d'appliquer un traitement d'image sur un FBO

// superclasses et classes nécessaires
Requires("libs/Process/Process.js");


class LuminosityContrast extends Process
{
    /** constructeur */
    constructor()
    {
        super("LuminosityContrast");

        // compiler le shader
        this.compileShader();
    }


    /** destructeur */
    destroy()
    {
        super.destroy();
    }


    /**
     * retourne le source du Fragment Shader
     */
    getFragmentShader()
    {
        let srcFragmentShader = dedent
            `#version 300 es
            precision mediump float;
            uniform sampler2D ColorMap;
            in vec2 frgTexCoord;

            // conversion RGB en HSV
            vec3 rgb2hsv(vec3 c)
            {
                vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
                vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
                vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
                float d = q.x - min(q.w, q.y);
                float e = 1.0e-10;
                return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
            }

            // conversion HSV en RGB
            vec3 hsv2rgb(vec3 c)
            {
                vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
                vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
                return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
            }

            uniform float luminosity;
            uniform float contrast;

            out vec4 glFragColor;

            void main()
            {
                vec4 color = texture(ColorMap, frgTexCoord);
                vec3 hsv = rgb2hsv(color.rgb);
                hsv.z = clamp((hsv.z + luminosity - 0.5) * contrast + 0.5, 0.0, 1.0);
                glFragColor = vec4(hsv2rgb(hsv), color.a);
            }`;
        return srcFragmentShader;
    }


    /**
     * détermine où sont les variables uniform spécifiques de ce traitement
     */
    findUniformLocations()
    {
        // appeler la méthode de la superclasse
        super.findUniformLocations();

        // déterminer où sont les variables uniform spécifiques
        this.m_LuminosityLoc = gl.getUniformLocation(this.m_ShaderId, "luminosity");
        this.m_ContrastLoc   = gl.getUniformLocation(this.m_ShaderId, "contrast");
    }


    /**
     * déclenche le traitement du FBO avec le Fragment Shader
     * @param fbo : FBO contenant l'image à traiter
     * @param luminosity : coefficient pour faire varier la luminosité, 1.0 = aucun changement
     * @param contrast : coefficient pour changer le contraste, 1.0 = aucun changement
     */
    process(fbo, luminosity, contrast)
    {
        // préparer le shader pour le traitement
        this.startProcess();

        // fournir le color buffer du FBO intermédiaire
        this.setTextureUnit(gl.TEXTURE0, this.m_ColorMapLoc, fbo.getColorBuffer());

        // fournir les paramètres du shader
        gl.uniform1f(this.m_LuminosityLoc, luminosity);
        gl.uniform1f(this.m_ContrastLoc, contrast);

        // dessiner un quadrilatère avec les quatre vertices
        gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);

        // désactiver les textures
        this.setTextureUnit(gl.TEXTURE0);

        // libérer les ressources
        this.endProcess();
    }
}
