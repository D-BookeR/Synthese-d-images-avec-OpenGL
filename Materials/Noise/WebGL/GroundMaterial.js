// Définition de la classe GroundMaterial

// superclasses et classes nécessaires
Requires("libs/Material/Material.js");
Requires("libs/Material/VBOset.js");
Requires("NoiseValue2D");


class GroundMaterial extends Material
{
    /**
     * Constructeur
     */
    constructor()
    {
        super("GroundMaterial");

        // créer la texture contenant le bruit de Perlin
        this.m_NoiseTexture = new NoiseValue2D(256, 256, 6);
        this.m_NoiseTextureLoc = -1;

        // compiler le shader
        this.compileShader();
    }


    /**
     * retourne le source du Vertex Shader
     */
    getVertexShader()
    {
        let srcVertexShader = dedent
            `#version 300 es
            in vec3 glVertex;
            in vec2 glTexCoord;
            out vec2 frgTexCoord;
            uniform mat4 mat4ModelView;
            uniform mat4 mat4Projection;
            void main()
            {
                gl_Position = mat4Projection * mat4ModelView * vec4(glVertex, 1.0);
                frgTexCoord = glTexCoord;
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
            in vec2 frgTexCoord;
            uniform sampler2D txBruit;
            const float pi = 3.14159;
            const vec3 darkwood = vec3(0.7, 0.3, 0.2);
            const vec3 lightwood = vec3(0.9, 0.7, 0.4);
            const float frequency = 50.0;
            const float narrowness = 10.0;
            const float turbulence = 0.1;

            out vec4 glFragColor;

            void main()
            {
                // valeur du bruit en ce point
                float noise = texture(txBruit, frgTexCoord).r * turbulence;
                // rayon des vagues = distance au centre + bruit
                float radius = distance(frgTexCoord, vec2(0.5, 0.5)) + noise;
                // on construit des vagues à partir du rayon
                float rate = pow(abs(sin(frequency*radius*pi)), narrowness);
                // la couleur varie entre clair et foncé
                glFragColor = vec4(mix(lightwood, darkwood, rate), 1.0);
            }`;
        return srcFragmentShader;
    }


    /**
     * compile ou recompile le shader
     */
    compileShader()
    {
        // appeler la méthode de la superclasse
        super.compileShader();

        // déterminer où sont les variables uniform
        this.m_NoiseTextureLoc = gl.getUniformLocation(this.m_ShaderId, "txBruit");
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

        // activer les textures chacune sur son unité
        this.m_NoiseTexture.setTextureUnit(gl.TEXTURE0, this.m_NoiseTextureLoc);
    }


    /**
     * Cette méthode désactive le matériau
     */
    disable()
    {
        // désactiver les textures
        this.m_NoiseTexture.setTextureUnit(gl.TEXTURE0);

        // appeler la méthode de la superclasse
        super.disable();
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_NoiseTexture.destroy();
        super.destroy();
    }
}
