// Cette classe permet d'appliquer un tramage ordonné type Bayer sur un FBO

// superclasses et classes nécessaires
Requires("libs/Process/Process.js");


class Dithering extends Process
{
    /**
     * Constructeur
     */
    constructor()
    {
        super("Dithering");

        // charger la texture de Bayer, en mode nearest pour ne surtout pas interpoler ses texels
        this.m_BayerTexture = new Texture2D("data/textures/bayer.png", gl.NEAREST, gl.CLAMP);

        // compiler le shader
        this.compileShader();
    }


    /**
     * retourne le source du Fragment Shader
     */
    getFragmentShader()
    {
        let srcFragmentShader = dedent
            `#version 300 es
            precision mediump float;
            uniform vec2 WindowSize;
            uniform sampler2D ColorMap;
            uniform sampler2D BayerMap;
            in vec2 frgTexCoord;

            float luminance(vec3 rgb)
            {
                const vec3 coefs = vec3(0.299, 0.587, 0.114);
                return dot(rgb, coefs);
            }

            out vec4 glFragColor;

            void main()
            {
                // coordonnées dans la matrice de Bayer
                vec2 coordsbayer = mod(frgTexCoord * WindowSize, 4.0) / 4.0;

                // seuil de Bayer à cet endroit
                float threshold = texture(BayerMap, coordsbayer).r;

                // luminance du texel de l'image à traiter
                vec3 color = texture(ColorMap, frgTexCoord).rgb;
                float lum = luminance(color);

                // comparaison au threshold sur chaque composante
                glFragColor = vec4(vec3(1.0) * step(threshold, lum), 1.0);
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
        this.m_BayerMapLoc  = gl.getUniformLocation(this.m_ShaderId, "BayerMap");
        this.m_WindowSizeLoc = gl.getUniformLocation(this.m_ShaderId, "WindowSize");
    }


    /**
     * applique un tramage de Bayer à l'image du FBO
     * @param fbo : FBO contenant l'image à traiter
     */
    process(fbo)
    {
        // préparer le shader pour le traitement
        this.startProcess();

        // fournir le color buffer du FBO à traiter
        this.setTextureUnit(gl.TEXTURE0, this.m_ColorMapLoc, fbo.getColorBuffer());

        // fournir la texture de Bayer
        this.m_BayerTexture.setTextureUnit(gl.TEXTURE1, this.m_BayerMapLoc);

        // fournir les paramètres du traitement
        let windowsize = vec2.fromValues(fbo.getWidth(), fbo.getHeight());
        vec2.glUniform(this.m_WindowSizeLoc, windowsize);

        // dessiner un quadrilatère avec les quatre vertices
        gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);

        // désactiver les textures
        this.setTextureUnit(gl.TEXTURE0);
        this.setTextureUnit(gl.TEXTURE1);

        // libérer les ressources
        this.endProcess();
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_BayerTexture.destroy();
        super.destroy();
    }
}
