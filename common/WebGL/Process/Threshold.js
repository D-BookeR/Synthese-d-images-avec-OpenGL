// Cette classe permet d'appliquer un seuil sur un FBO


// superclasses et classes nécessaires
Requires("libs/Process/Process.js");


class Threshold extends Process
{
    /** constructeur */
    constructor()
    {
        super("Threshold");

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

            float luminance(vec3 rgb)
            {
                const vec3 coefs = vec3(0.299, 0.587, 0.114);
                return dot(rgb, coefs);
            }

            uniform float threshold;

            out vec4 glFragColor;

            void main()
            {
                vec3 color = texture(ColorMap, frgTexCoord).rgb;
                //if (luminance(color) > threshold) {
                //    glFragColor = vec4(1.0, 1.0, 1.0, 1.0);
                //} else {
                //    glFragColor = vec4(0.0, 0.0, 0.0, 0.0);
                //}
                glFragColor = vec4(vec3(1.0) * step(threshold, luminance(color)), 1.0);

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
        this.m_ThresholdLoc = gl.getUniformLocation(this.m_ShaderId, "threshold");
    }


    /**
     * applique le traitement sur le FBO fourni : les pixels dont la luminance est
     * inférieure au seuil sont mis à noir, les autres sont remplacés par du blanc
     * @param fbo : FBO contenant l'image à traiter
     * @param threshold : seuil 0.0 à 1.0
     */
    process(fbo, threshold=0.5)
    {
        // préparer le shader pour le traitement
        this.startProcess();

        // fournir le color buffer du FBO intermédiaire
        this.setTextureUnit(gl.TEXTURE0, this.m_ColorMapLoc, fbo.getColorBuffer());

        // fournir les paramètres du shader
        gl.uniform1f(this.m_ThresholdLoc, threshold);

        // dessiner un quadrilatère avec les quatre vertices
        gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);

        // désactiver les textures
        this.setTextureUnit(gl.TEXTURE0);

        // libérer les ressources
        this.endProcess();
    }
}
