// Cette classe permet d'appliquer une convolution sur un FBO

// superclasses et classes nécessaires
Requires("libs/Process/Process.js");


class Convolution extends Process
{
    /**
     * Constructeur
     */
    constructor()
    {
        super("Convolution");

        // compiler le shader
        this.compileShader();
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
            uniform vec2 TexelSize;
            in vec2 frgTexCoord;

            vec3 getTexel(int ds, int dt)
            {
                vec2 dsdt = vec2(float(ds), float(dt)) * TexelSize;
                vec2 texcoord = frgTexCoord + dsdt;
                return texture(ColorMap, texcoord).rgb;
            }

            out vec4 glFragColor;

            void main()
            {
                vec3 sum = vec3(0.0);

                sum +=    1.0 * getTexel(-2, -2);
                sum +=    4.0 * getTexel(-1, -2);
                sum +=    6.0 * getTexel( 0, -2);
                sum +=    4.0 * getTexel(+1, -2);
                sum +=    1.0 * getTexel(+2, -2);

                sum +=    4.0 * getTexel(-2, -1);
                sum +=   16.0 * getTexel(-1, -1);
                sum +=   24.0 * getTexel( 0, -1);
                sum +=   16.0 * getTexel(+1, -1);
                sum +=    4.0 * getTexel(+2, -1);

                sum +=    6.0 * getTexel(-2,  0);
                sum +=   24.0 * getTexel(-1,  0);
                sum += -476.0 * getTexel( 0,  0);
                sum +=   24.0 * getTexel(+1,  0);
                sum +=    6.0 * getTexel(+2,  0);

                sum +=    4.0 * getTexel(-2, +1);
                sum +=   16.0 * getTexel(-1, +1);
                sum +=   24.0 * getTexel( 0, +1);
                sum +=   16.0 * getTexel(+1, +1);
                sum +=    4.0 * getTexel(+2, +1);

                sum +=    1.0 * getTexel(-2, +2);
                sum +=    4.0 * getTexel(-1, +2);
                sum +=    6.0 * getTexel( 0, +2);
                sum +=    4.0 * getTexel(+1, +2);
                sum +=    1.0 * getTexel(+2, +2);

                const float sommecoefs = -256.0;

                glFragColor = vec4(sum/sommecoefs, 1.0);
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
        this.m_TexelSizeLoc = gl.getUniformLocation(this.m_ShaderId, "TexelSize");
    }


    /**
     * applique le traitement sur l'image fournie dans le FBO
     * @param fbo : FBO contenant l'image à traiter
     */
    process(fbo)
    {
        // préparer le shader pour le traitement
        this.startProcess();

        // fournir le color buffer du FBO à traiter
        this.setTextureUnit(gl.TEXTURE0, this.m_ColorMapLoc, fbo.getColorBuffer());

        // fournir les paramètres du flou
        let texelSize = vec2.fromValues(1.0/fbo.getWidth(), 1.0/fbo.getHeight());
        vec2.glUniform(this.m_TexelSizeLoc, texelSize);

        // dessiner un quadrilatère avec les quatre vertices
        gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);

        // désactiver les textures
        this.setTextureUnit(gl.TEXTURE0);

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
