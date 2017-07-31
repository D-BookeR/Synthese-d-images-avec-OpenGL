// Cette classe permet de mettre une image en noir&blanc

// superclasses et classes nécessaires
Requires("libs/Process/Process.js");


class BlackAndWhite extends Process
{
    /**
     * Constructeur
     */
    constructor()
    {
        super("BlackAndWhite");

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
            in vec2 frgTexCoord;

            // sera lié à l'image à traiter
            uniform sampler2D ColorPicture;

            // coefficients UIT-R BT 709
            const vec3 coefs = vec3(0.2126, 0.7152, 0.0722);

            out vec4 glFragColor;

            void main()
            {
                vec3 color = texture(ColorPicture, frgTexCoord).rgb;
                float luma = dot(color, coefs);
                glFragColor = vec4(luma,luma,luma, 1.0);
            }`;
        return srcFragmentShader;
    }


    /**
     * détermine où sont les variables uniform spécifiques de ce traitement
     */
    findUniformLocations()
    {
        // déterminer où sont les variables uniform
        this.m_ColorPictureLoc  = gl.getUniformLocation(this.m_ShaderId, "ColorPicture");
    }


    /**
     * applique le traitement sur l'image fournie dans le FBO
     * @param texture : Texture2D contenant l'image à traiter
     */
    process(texture)
    {
        // préparer le shader pour le traitement
        this.startProcess();

        // fournir le color buffer du FBO à traiter
        texture.setTextureUnit(gl.TEXTURE0, this.m_ColorPictureLoc);

        // dessiner un quadrilatère avec les quatre vertices
        gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);

        // désactiver les textures
        texture.setTextureUnit(gl.TEXTURE0);

        // libérer les ressources
        this.endProcess();
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        super.destroy();
    }
}
