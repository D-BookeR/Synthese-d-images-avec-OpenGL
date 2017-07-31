// Cette classe permet d'appliquer un flou variable selon la distance
// voir http://www.nutty.ca/?page_id=352&link=depth_of_field
// voir https://github.com/ga-m3dv/ga-worldwind-suite/blob/master/Common/src/main/java/au/gov/ga/worldwind/common/effects/depthoffield/DepthOfFieldFragmentShader.glsl

// superclasses et classes nécessaires
Requires("libs/Process/Process.js");


/**
 * constructeur d'un traitement qui applique une profondeur de champ à un FBO
 */
class DepthOfFieldBlur extends Process
{
    constructor(near, far)
    {
        super("DepthOfFieldBlur");

        // compiler le shader
        this.compileShader();
    }


    /**
     * change les distances near et far du shader
     * @param near : distance la plus proche dans le depth buffer du FBO à traiter
     * @param far : distance la plus lointaine dans le depth buffer du FBO à traiter
     */
    setNearFar(near, far)
    {
        this.m_Near = near;
        this.m_Far = far;
    }


    /**
     * retourne le source du Fragment Shader
     */
    getFragmentShader()
    {
        return dedent
            `#version 300 es
            precision mediump float;
            uniform float sharpDistance;
            uniform float sharpness;
            uniform sampler2D ColorMap;
            uniform sampler2D DepthMap;
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

            const float maxBlurRadius = 5.0; // rayon maximal du flou

            // calcul de la distance réelle correspondant à depth
            uniform float near;
            uniform float far;
            float depth2distance(float depth)
            {
                if (depth == 1.0) return 1e38;
                return (near * far) / (far - depth * (far - near));
            }

            // retourne le rayon du flou à appliquer en ft de la distance
            float BlurRadius(float distance)
            {
                float radius = abs(distance - sharpDistance) * sharpness;
                return clamp(radius, -maxBlurRadius, +maxBlurRadius);
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
                    // définir un texel à consulter dans le voisinage
                    vec2 offset = RandomRotation * PoissonSamples[i] * maxBlurRadius;
                    vec2 coord = frgTexCoord +  TexelSize * offset;

                    // déterminer le rayon du disque de Bokeh de ce texel
                    float radius = BlurRadius(depth2distance(texture(DepthMap, coord).r));

                    // si le rayon > distance du fragment, on voit le disque de Bokeh
                    //if (radius > length(offset)) {
                    //    // le fragment est dans le disque de bokeh de l'échantillon
                    //    sum += texture(ColorMap, coord).rgb;
                    //} else {
                    //    // on ne voit pas le disque de bokeh
                    //    sum += texture(ColorMap, frgTexCoord).rgb;
                    //}
                    coord += step(radius, length(offset)) * (frgTexCoord-coord);
                    sum += texture(ColorMap, coord).rgb;
                }

                glFragColor = vec4(sum / float(PoissonCount), 1.0);
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
        this.m_DepthMapLoc      = gl.getUniformLocation(this.m_ShaderId, "DepthMap");
        this.m_TexelSizeLoc     = gl.getUniformLocation(this.m_ShaderId, "TexelSize");
        this.m_NearLoc          = gl.getUniformLocation(this.m_ShaderId, "near");
        this.m_FarLoc           = gl.getUniformLocation(this.m_ShaderId, "far");
        this.m_SharpDistanceLoc = gl.getUniformLocation(this.m_ShaderId, "sharpDistance");
        this.m_SharpnessLoc     = gl.getUniformLocation(this.m_ShaderId, "sharpness");
        this.m_TimeLoc          = gl.getUniformLocation(this.m_ShaderId, "Time");

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
     * dessine la texture fournie sur l'écran
     * @param fbonet : FBO contenant l'image et son depth buffer
     * @param focalLength : distance focale de l'objectif
     * @param aperture : ouverture de l'objectif
     * @param distance : distance où l'image est parfaitement nette (doit être >> focale)
     */
    process(fbonet, focalLength, aperture, distance)
    {
        // préparer le shader pour le traitement
        super.startProcess();

        // fournir les color et depth buffers
        this.setTextureUnit(gl.TEXTURE0, this.m_ColorMapLoc, fbonet.getColorBuffer());
        this.setTextureUnit(gl.TEXTURE1, this.m_DepthMapLoc, fbonet.getDepthBuffer());

        // taille des texels
        let texelSize = vec2.fromValues(1.0/fbonet.getWidth(), 1.0/fbonet.getHeight());
        vec2.glUniform(this.m_TexelSizeLoc, texelSize);

        // fournir les paramètres du calcul de netteté
        gl.uniform1f(this.m_NearLoc, this.m_Near);
        gl.uniform1f(this.m_FarLoc, this.m_Far);
        gl.uniform1f(this.m_SharpDistanceLoc, distance);
        gl.uniform1f(this.m_SharpnessLoc, focalLength/aperture);
        gl.uniform1f(this.m_TimeLoc, Utils.Time);

        // dessiner un quadrilatère avec les quatre vertices
        gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);

        // désactiver les textures
        this.setTextureUnit(gl.TEXTURE0);
        this.setTextureUnit(gl.TEXTURE1);

        // libérer les ressources
        super.endProcess();
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        super.destroy();
    }
}
