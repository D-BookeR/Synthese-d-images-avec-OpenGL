// Cette classe permet d'appliquer un traitement d'image sur un FBO

// superclasses et classes nécessaires
Requires("libs/Process/Process.js");


class Fog extends Process
{
    /**
     * constructeur
     * @param color : vec4 contenant la couleur de saturation de la brume
     * @param exponent : exposant pour élever la distance
     */
    constructor(color, exponent)
    {
        super("Fog");

        // compiler le shader
        this.compileShader();

        // fournir la couleur d'effacement au shader
        gl.useProgram(this.m_ShaderId);
        vec4.glUniform(this.m_BackgroundColorLoc, color);
        gl.uniform1f(this.m_ExponentLoc, exponent);
        gl.useProgram(null);
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
            uniform sampler2D DepthMap;
            uniform vec4 BackgroundColor;
            uniform float exponent;

            in vec2 frgTexCoord;
            out vec4 glFragColor;

            void main()
            {
                // couleur actuellement présente
                vec4 color = texture(ColorMap, frgTexCoord);

                // distance donnée par le depth buffer
                float distance = pow(texture(DepthMap, frgTexCoord).r, exponent);

                // mélange entre la couleur présente et celle du fond
                glFragColor = mix(color, BackgroundColor, distance);
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
        this.m_DepthMapLoc    = gl.getUniformLocation(this.m_ShaderId, "DepthMap");
        this.m_BackgroundColorLoc = gl.getUniformLocation(this.m_ShaderId, "BackgroundColor");
        this.m_ExponentLoc    = gl.getUniformLocation(this.m_ShaderId, "exponent");
    }


    /**
     * effectue le traitement
     * @param fbo : FBO contenant l'image à traiter
     */
    process(fbo)
    {
        // préparer le shader pour le traitement
        this.startProcess();

        // fournir le color buffer et le depth buffer du FBO
        this.setTextureUnit(gl.TEXTURE0, this.m_ColorMapLoc, fbo.getColorBuffer());
        this.setTextureUnit(gl.TEXTURE1, this.m_DepthMapLoc, fbo.getDepthBuffer());

        // dessiner un quadrilatère avec les quatre vertices
        gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);

        // désactiver les textures
        this.setTextureUnit(gl.TEXTURE0);
        this.setTextureUnit(gl.TEXTURE1);

        // libérer les ressources
        this.endProcess();
    }


    /** destructeur */
    destroy()
    {
        super.destroy();
    }
}
