// Définition de la classe AppleMaterial

// inspiration : http://lodev.org/cgtutor/randomnoise.html

// superclasses et classes nécessaires
Requires("libs/Material/Material.js");
Requires("libs/Material/VBOset.js");
Requires("NoiseValue2D");


class AppleMaterial extends Material
{
    /**
     * Constructeur
     */
    constructor()
    {
        super("AppleMaterial");

        // créer la texture contenant le bruit de Perlin
        this.m_NoiseTexture = new NoiseValue2D(256, 256, 5);
        this.m_NoiseTextureLoc = -1;

        // charger la texture contenant le gradient
        this.m_TextureGradient = new Texture2D("data/textures/gradients/ciel.png");
        this.m_TextureGradientLoc = -1;

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
            uniform sampler2D txGradient;

            out vec4 glFragColor;

            void main()
            {
                float noise = texture(txBruit, frgTexCoord).r;
                glFragColor = texture(txGradient, vec2(noise,0));
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

        // déterminer où sont les variables uniform
        this.m_NoiseTextureLoc    = gl.getUniformLocation(this.m_ShaderId, "txBruit");
        this.m_TextureGradientLoc = gl.getUniformLocation(this.m_ShaderId, "txGradient");
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

        // activer les textures sur des unités différentes
        this.m_NoiseTexture.setTextureUnit(gl.TEXTURE0, this.m_NoiseTextureLoc);
        this.m_TextureGradient.setTextureUnit(gl.TEXTURE1, this.m_TextureGradientLoc);
    }


    /**
     * Cette méthode désactive le matériau
     */
    disable()
    {
        // désactiver les textures
        this.m_NoiseTexture.setTextureUnit(gl.TEXTURE0);
        this.m_TextureGradient.setTextureUnit(gl.TEXTURE1);

        // appeler la méthode de la superclasse
        super.disable();
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_TextureGradient.destroy();
        this.m_NoiseTexture.destroy();
        super.destroy();
    }
}
