/**
 * Définition de la classe SSAOLight, une lampe ambiante avec ombrage causé par les occlusions locales
 * voir http://john-chapman-graphics.blogspot.fr/2013/01/ssao-tutorial.html
 * voir http://www.gamedev.net/page/resources/_/technical/graphics-programming-and-theory/a-simple-and-practical-approach-to-ssao-r2753
 * voir http://blog.evoserv.at/index.php/2012/12/hemispherical-screen-space-ambient-occlusion-ssao-for-deferred-renderers-using-openglglsl/
 */

Requires("lib/Process/Process.js");
Requires("lib/Material/Light.js");


class SSAOLight extends Light
{
    /**
     * constructeur
     * C'est une lampe ambiante avec ombrage causé par les occlusions locales
     * @param radius : distance relative à [0.0, 1.0] d'exploration des points, mettre 0.04
     * @param mindistance : distance en unités du monde où un objet est considéré comme proche
     * @param maxdistance : distance en unités du monde à partir de laquelle un objet est trop loin pour occulter
     */
    constructor(radius, mindistance, maxdistance)
    {
        super();
        this.m_Name = "SSAOLight";

        // initialisation des variables membre spécifiques
        this.m_Radius      = radius;
        this.m_MinDistance = mindistance;
        this.m_MaxDistance = maxdistance;

        // compiler le shader
        this.compileShader();
    }


    /** destructeur */
    destroy()
    {
        super.destroy();
    }


    /**
     * construit le Fragment Shader qui calcule l'éclairement de cette lampe
     * @return source du shader
     */
    getFragmentShader()
    {
        let srcFragmentShader = dedent
            `#version 300 es
            precision mediump float;
            in vec2 frgTexCoord;

            uniform sampler2D MapDiffuse;
            uniform sampler2D MapPosition;
            uniform sampler2D MapNormale;
            uniform sampler2D MapDepth;
            uniform vec3 LightColor;

            // paramètres du traitement
            uniform float radius;
            uniform float MinDistance;
            uniform float MaxDistance;

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
                // coordonnées, profondeur, couleur et normale du fragment
                vec4 center = texture(MapPosition, frgTexCoord);
                if (center.w != 1.0) discard;
                gl_FragDepth = texture(MapDepth, frgTexCoord).r;
                vec4 color   = texture(MapDiffuse, frgTexCoord);
                vec3 N       = texture(MapNormale, frgTexCoord).xyz;

                // initialiser l'échantillonnage aléatoire de Poisson
                float angle = rand(frgTexCoord*gl_FragDepth) * 6.283285;
                float s = sin(angle);
                float c = cos(angle);
                mat2 RandomRotation = mat2(c, s, -s, c);

                // passer le voisinage en revue
                float occlusion = 0.0;
                for (int i=0; i<PoissonCount; i++) {
                    // échantillon courant
                    vec2 offset = RandomRotation * radius * PoissonSamples[i];
                    vec4 neighbor = texture(MapPosition, frgTexCoord + offset);

                    // direction et distance entre ce voisin et le fragment
                    vec3 D = neighbor.xyz - center.xyz;
                    float distance = length(D);
                    float neighbor_in_front = step(0.1, dot(N, D));

                    // accumuler les occlusions
                    occlusion += neighbor.w * neighbor_in_front * (1.0-smoothstep(MinDistance, MaxDistance, distance));
                }

                // normalisation de l'occlusion
                occlusion = 1.0 - occlusion / float(PoissonCount);

                // couleur finale
                glFragColor = vec4(occlusion * color.rgb * LightColor, 1.0) * color.a;
            }`;
        return srcFragmentShader;
    }


    /**
     * détermine où sont les variables uniform spécifiques de cette lampe
     */
    findUniformLocations()
    {
        // appeler la méthode de la superclasse
        super.findUniformLocations();

        // déterminer où sont les variables uniform spécifiques de cette lampe
        this.m_RadiusLoc      = gl.getUniformLocation(this.m_ShaderId, "radius");
        this.m_MinDistanceLoc = gl.getUniformLocation(this.m_ShaderId, "MinDistance");
        this.m_MaxDistanceLoc = gl.getUniformLocation(this.m_ShaderId, "MaxDistance");

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
     * active le shader, les VBO et les textures pour appliquer l'éclairement défini par la lampe
     * @param gbuffer : FBO MRT contenant toutes les informations de la scène à éclairer
     */
    startProcess(gbuffer)
    {
        // appeler la méthode de la superclasse
        super.startProcess(gbuffer);

        // paramètres de l'ombre SSAO
        gl.uniform1f(this.m_RadiusLoc,      this.m_Radius);
        gl.uniform1f(this.m_MinDistanceLoc, this.m_MinDistance);
        gl.uniform1f(this.m_MaxDistanceLoc, this.m_MaxDistance);
    }
}
