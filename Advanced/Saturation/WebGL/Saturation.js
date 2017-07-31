// Cette classe permet de saturer les couleurs d'un FBO


// superclasses et classes nécessaires
Requires("libs/Process/FrameBufferObject.js");
Requires("libs/Process/Process.js");


class Saturation extends Process
{
    /**
     * constructeur
     * @param width : largeur du FBO utilisé
     * @param height : hauteur du FBO utilisé
     */
    constructor(width, height)
    {
        super("Saturation");

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
        return dedent
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

            uniform float strength;

            out vec4 glFragColor;

            void main()
            {
                vec4 color = texture(ColorMap, frgTexCoord);
                vec3 hsv = rgb2hsv(color.rgb);
                /// modification de la saturation (décommenter la ligne suivante)
                hsv.y = clamp(hsv.y * strength, 0.0, 1.0);
                /// sépia (décommenter la ligne suivante)
                //hsv.xy = vec2(0.1, 0.5);
                /// vignettage (décommenter les deux lignes suivantes)
                //float dist = distance(frgTexCoord, vec2(0.5, 0.5));
                //hsv.z *= 1.0 - smoothstep(0.2, 0.9, dist);
                glFragColor = vec4(hsv2rgb(hsv), color.a);
            }`;
    }


    /**
     * détermine où sont les variables uniform spécifiques de ce traitement
     */
    findUniformLocations()
    {
        // appeler la méthode de la superclasse
        super.findUniformLocations();

        // déterminer où sont les variables uniform spécifiques
        this.m_StrengthLoc = gl.getUniformLocation(this.m_ShaderId, "strength");
    }


    /**
     * applique le traitement sur l'image fournie dans le FBO
     * @param fbo : FBO contenant l'image à traiter
     * @param strength : force de l'effet, 1.0 ne change rien
     */
    process(fbo, strength)
    {
        // préparer le shader pour le traitement
        this.startProcess();

        // fournir le color buffer du FBO à traiter
        this.setTextureUnit(gl.TEXTURE0, this.m_ColorMapLoc, fbo.getColorBuffer());

        // fournir le paramètre du traitement
        gl.uniform1f(this.m_StrengthLoc, strength);

        // dessiner un quadrilatère avec les quatre vertices
        gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);

        // désactiver les textures
        this.setTextureUnit(gl.TEXTURE0);

        // libérer les ressources
        this.endProcess();
    }
}
