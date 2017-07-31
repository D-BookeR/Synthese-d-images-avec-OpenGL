// Cette classe permet de combiner réflexion et réfraction selon WaterMaterial
// https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-shading/reflection-refraction-fresnel
// http://graphicsrunner.blogspot.fr/2010/08/water-using-flow-maps.html

// superclasses et classes nécessaires
Requires("libs/Material/Material.js");
Requires("libs/Material/Texture2D.js");


/**
 * Cette classe réalise la fusion du reflet et de la vue sous-surface.
 * Elle contient plusieurs constantes qui correspondent aux textures et objets employés
 */
class WaterMaterial extends Material
{
    /**
     * Constructeur
     */
    constructor()
    {
        super("WaterMaterial");

        // texture donnant la phase des vaguelettes
        this.m_MapPhase = new Texture2D("data/textures/eau/phase.png", gl.LINEAR_MIPMAP_LINEAR, gl.REPEAT);

        // FBO fournissant le reflet et le position_ground
        this.m_FBOreflection = null;
        this.m_FBObackground = null;

        // allocation d'une matrice temporaire
        this.m_Mat3Normal = mat3.create();
        this.m_ViewPort = vec2.create();

        // compiler le shader
        this.compileShader();
    }


    /**
     * fournit les deux g-buffer utilisés par ce matériau
     * @param FBOreflection {FrameBufferObject} FBO contenant deux color buffers : couleur diffuse et position du reflet
     * @param FBObackground {FrameBufferObject} FBO contenant deux color buffers : couleur diffuse et position du fond
     * @param width : nombre de pixels du viewport
     * @param height : nombre de pixels du viewport
     */
    setGBuffers(FBOreflection, FBObackground, width, height)
    {
        this.m_FBOreflection = FBOreflection;
        this.m_FBObackground = FBObackground;
        vec2.set(this.m_ViewPort, width, height);
    }


    /**
     * retourne le source du Vertex Shader
     */
    getVertexShader()
    {
        let srcVertexShader = dedent
            `#version 300 es

            // paramètres uniform
            uniform mat4 mat4ModelView;
            uniform mat4 mat4Projection;

            // attributs de sommets
            in vec3 glVertex;
            in vec2 glTexCoord;

            // interpolation vers les fragments
            out vec4 frgPosition;
            out vec2 frgTexCoord;

            void main()
            {
                frgPosition = mat4ModelView * vec4(glVertex, 1.0);
                gl_Position = mat4Projection * frgPosition;
                frgTexCoord = glTexCoord * 0.2;
            }`;
        return srcVertexShader;
    }


    /**
     * retourne le source du Fragment Shader
     */
    getFragmentShader()
    {
        let srcFragmentShader = dedent
            `#version 300 es
            precision mediump float;
            in vec4 frgPosition;
            in vec2 frgTexCoord;

            // informations venant du g-buffer
            uniform mat3 mat3Normal;
            uniform vec2 viewport;
            uniform sampler2D MapIslandPosition;
            uniform sampler2D MapDepth;

            // informations venant des dessins de la scène
            uniform sampler2D MapReflexion;
            uniform sampler2D MapRefraction;

            // texture aléatoire
            uniform sampler2D MapPhase;

            // temps
            uniform float Time;

            // divers
            const float twopi = 6.283185307;

            /// rapport des indices air/eau par couleur
            const vec3 eta = vec3(0.76, 0.75, 0.71);

            /// exposant du produit scalaire de Schlick, normalement c'est 5.0
            const float SchlickExponent = 3.0;

            /// coefficient multiplicatif pour augmenter la profondeur
            const float DepthFactor = 0.5;

            /// amplitude (hauteur) des vagues
            const float WavesAmplitude = 0.2;

            /// distance minimale pour avoir la taille maximale des vagues
            const float MinDistance = 8.0;

            /// fréquence spatiale (écartement) des vagues
            const float WavesSpatFreq = 8.0;

            /// fréquence temporelle (vitesse) des vagues
            const float WavesSpeed = 1.0;

            /// importance des vagues sur les textures
            const float WavesReflexionStrength = 0.2;
            const float WavesRefractionStrength = 0.08;

            /// coefficients d'absorption du reflet et de la réfraction
            const float ReflexionAttenuation = 0.6;
            const float RefractionAttenuation = 0.6;

            /// couleur du ciel = couleur de l'eau en profondeur infinie
            const vec3 SkyColor = vec3(0.7,0.9,1.0);

            /// g-buffer
            out vec4 glFragData[4];

            void main()
            {
                // coordonnées écran [0, 1]
                vec2 screenposition = gl_FragCoord.xy / viewport;

                // coordonnées caméra du point de contact avec le fond
                vec4 position_ground = texture(MapIslandPosition, screenposition);

                // coordonnées caméra du point de contact avec l'eau
                vec4 position_surface = frgPosition;

                // distance traversée dans l'eau à cet endroit
                float distance = length(position_surface.xyz - position_ground.xyz) * DepthFactor;

                // vecteur vue inversé (il va vers l'oeil)
                vec3 V = -normalize(position_ground.xyz);

                // perturbation par les vaguelettes stationnaires, l'importance décroit avec la distance
                vec2 phase = texture(MapPhase, frgTexCoord * WavesSpatFreq).xy;
                vec2 offset = WavesAmplitude * sin(twopi*(Time*WavesSpeed + phase)) / (MinDistance-position_surface.z);

                // calculer les coordonnées caméra de la normale
                vec3 normal = mat3Normal * vec3(offset.x, 1.0, offset.y);
                vec3 N = normalize(normal);
                float dotVN = max(dot(V,N), 0.0);

                // calcul du coefficient de Fresnel par l'approximation de Schlick
                const vec3 R0 = ((1.0-eta)*(1.0-eta)) / ((1.0+eta)*(1.0+eta));
                float powdotVN = pow(1.0-dotVN, SchlickExponent);
                vec3 fresnel = clamp(R0 + (1.0-R0) * powdotVN, 0.0, 1.0);

                // couleurs données par le reflet et la réfraction
                vec3 reflexion  = texture(MapReflexion,  screenposition + offset*WavesReflexionStrength).rgb * ReflexionAttenuation;
                vec3 refraction = texture(MapRefraction, screenposition + offset*WavesRefractionStrength).rgb;

                // la couleur est attenuée par la distance parcourue dans l'eau
                refraction = mix(refraction, SkyColor, distance) * RefractionAttenuation;

                // mélange entre réfraction et réflexion
                vec3 Kd = mix(refraction, reflexion, fresnel);

                // couleur résultante
                glFragData[0] = vec4(Kd, 1.0);
                glFragData[1] = vec4(1.0, 1.0, 1.0, 1024.0);
                glFragData[2] = vec4(frgPosition.xyz, 1.0);
                glFragData[3] = vec4(N, 0.0);
            }`;
        return srcFragmentShader;
    }


    /**
     * recompile le shader du matériau
     */
    compileShader()
    {
        // appeler la méthode de la superclasse
        super.compileShader();

        // déterminer où sont les variables uniform spécifiques
        this.m_MapIslandPositionLoc = gl.getUniformLocation(this.m_ShaderId, "MapIslandPosition");
        this.m_MapReflectionLoc     = gl.getUniformLocation(this.m_ShaderId, "MapReflexion");
        this.m_MapRefractionLoc     = gl.getUniformLocation(this.m_ShaderId, "MapRefraction");
        this.m_MapPhaseLoc          = gl.getUniformLocation(this.m_ShaderId, "MapPhase");
        this.m_ViewPortLoc          = gl.getUniformLocation(this.m_ShaderId, "viewport");
        this.m_TimeLoc              = gl.getUniformLocation(this.m_ShaderId, "Time");
    }


    /**
     * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
     * @return le VBOset du matériau
     */
    createVBOset()
    {
        // créer le VBOset et spécifier les noms des attribute nécessaires à ce matériau
        let vboset = super.createVBOset();
        vboset.addAttribute(MeshVertex.ID_ATTR_TEXCOORD, Utils.VEC2, "glTexCoord");
        return vboset;
    }


    /**
     * Cette méthode active le matériau : met en place son shader,
     * fournit les variables uniform qu'il demande
     * @param mat4Projection : mat4 contenant la projection
     * @param mat4ModelView : mat4 contenant la transformation vers la caméra
     */
    enable(mat4Projection, mat4ModelView)
    {
        // appeler la méthode de la superclasse
        super.enable(mat4Projection, mat4ModelView);

        // fournir les images à traiter
        this.m_Unit = gl.TEXTURE0;
        if (this.m_FBOreflection != null) {
            this.m_FBOreflection.setTextureUnit(this.m_Unit++,  this.m_MapReflectionLoc,    this.m_FBOreflection.getColorBuffer(0));
        }
        if (this.m_FBObackground != null) {
            this.m_FBObackground.setTextureUnit(this.m_Unit++,  this.m_MapRefractionLoc,     this.m_FBObackground.getColorBuffer(0));
            this.m_FBObackground.setTextureUnit(this.m_Unit++,  this.m_MapIslandPositionLoc, this.m_FBObackground.getColorBuffer(1));
        }
        this.m_MapPhase.setTextureUnit(this.m_Unit++, this.m_MapPhaseLoc);

        // viewport
        vec2.glUniform(this.m_ViewPortLoc, this.m_ViewPort);

        // fournir le temps
        gl.uniform1f(this.m_TimeLoc, Utils.Time);
    }


    /**
     * Cette méthode désactive le matériau
     * NB : le shader doit être activé
     */
    disable()
    {
        // désactiver les textures
        while (this.m_Unit > gl.TEXTURE0) {
            this.m_Unit--;
            gl.activeTexture(this.m_Unit);
            gl.bindTexture(gl.TEXTURE_2D, null);
        }

        // appeler la méthode de la superclasse
        super.disable();
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_MapPhase.destroy();
        super.destroy();
    }
}
