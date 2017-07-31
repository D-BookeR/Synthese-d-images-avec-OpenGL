// Cette classe permet d'appliquer un flou de Poisson sur un FBO
// voir https://github.com/spite/Wagner/blob/master/fragment-shaders/poisson-disc-blur-fs.glsl

// superclasses et classes nécessaires
Requires("libs/Process/Process.js");


class PoissonBlur extends Process
{
    /**
     * Constructeur
     */
    constructor()
    {
        super("PoissonBlur");

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
            uniform sampler2D ColorMap;
            uniform vec2 TexelSize;
            in vec2 frgTexCoord;
            uniform float Time;

            // échantillonnage de Poisson
            const int PoissonCount = 16;
            uniform vec2 PoissonSamples[PoissonCount];

            highp float rand(vec2 co)
            {
                highp float a  = 12.9898;
                highp float b  = 78.233;
                highp float c  = 43758.5453;
                highp float dt = dot(co ,vec2(a,b));
                highp float sn = mod(dt, 3.141592);
                return fract(sin(sn) * c);
            }

            out vec4 glFragColor;

            void main()
            {
                // initialiser l'échantillonnage aléatoire de Poisson
                float angle = rand(frgTexCoord) * 6.283285 * Time;
                float s = sin(angle);
                float c = cos(angle);
                mat2 RandomRotation = mat2(c, s, -s, c);

                // accumuler les couleurs
                vec3 sum = vec3(0.0);
                for (int i=0; i < PoissonCount; i++) {
                    vec2 offset = RandomRotation * PoissonSamples[i];
                    sum += texture(ColorMap, frgTexCoord + offset*TexelSize).rgb;
                }
                glFragColor = vec4(sum/float(PoissonCount), 1.0);
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
        this.m_TexelSizeLoc = gl.getUniformLocation(this.m_ShaderId, "TexelSize");
        this.m_TimeLoc      = gl.getUniformLocation(this.m_ShaderId, "Time");

        // initialiser le tableau des constantes (erreur : arrays may not be declared constant since they cannot be initialized)
        let PoissonSamplesLoc = gl.getUniformLocation(this.m_ShaderId, "PoissonSamples");
        gl.useProgram(this.m_ShaderId);
        let PoissonSamples = [
            +0.79636, -0.56051,
            -0.64373, +0.68863,
            -0.00856, +0.93684,
            -0.26651, -0.82735,
            -0.81180, +0.29899,
            -0.58342, -0.62513,
            -0.78033, -0.28858,
            +0.43087, +0.70461,
            +0.71023, +0.34784,
            +0.70937, -0.22050,
            +0.28912, -0.65419,
            -0.44501, +0.31930,
            -0.47742, -0.03139,
            -0.21586, -0.39196,
            +0.26554, -0.22434,
            +0.02771, +0.26856,
        ];
        gl.uniform2fv(PoissonSamplesLoc, PoissonSamples);
        gl.useProgram(null);
    }


    /**
     * applique le traitement sur l'image fournie dans le FBO
     * @param fbo : FBO contenant l'image à flouter
     * @param radius : rayon du flou
     */
    process(fbo, radius=1.0)
    {
        // préparer le shader pour le traitement
        this.startProcess();

        // fournir le color buffer du FBO à traiter
        this.setTextureUnit(gl.TEXTURE0, this.m_ColorMapLoc, fbo.getColorBuffer());

        // fournir les paramètres du flou
        radius = Math.abs(radius);
        let texelSize = vec2.fromValues(radius/fbo.getWidth(), radius/fbo.getHeight());
        vec2.glUniform(this.m_TexelSizeLoc, texelSize);
        gl.uniform1f(this.m_TimeLoc, Utils.Time);

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
