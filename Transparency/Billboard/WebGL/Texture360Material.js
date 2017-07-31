// Définition de la classe Texture360Material

// superclasses et classes nécessaires
Requires("libs/Material/Material.js");
Requires("libs/Material/VBOset.js");
Requires("Texture360");


class Texture360Material extends Material
{
    /**
     * Constructeur
     * @param texture360 sur laquelle est basé le matériau
     */
    constructor(texture360)
    {
        super("Texture360Material");

        // charger la texture
        this.m_Texture360 = texture360;
        this.m_TextureLoc = -1;
        this.m_Texture = null;

        // modification de la couleur et transparence
        this.m_ColorCoefficient = 1.0;
        this.m_AlphaCoefficient = 1.0;

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
            uniform mat4 mat4ModelView;
            uniform mat4 mat4Projection;
            out vec2 frgTexCoord;

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

            uniform sampler2D txColor;
            uniform float colorCoefficient;
            uniform float alphaCoefficient;

            out vec4 glFragColor;

            void main()
            {
                // accès à la texture
                vec4 color = texture(txColor, frgTexCoord);
                // modulation de la couleur et la transparence
                glFragColor = vec4(color.rgb*colorCoefficient, color.a*alphaCoefficient);
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
        this.m_TextureLoc          = gl.getUniformLocation(this.m_ShaderId, "txColor");
        this.m_ColorCoefficientLoc = gl.getUniformLocation(this.m_ShaderId, "colorCoefficient");
        this.m_AlphaCoefficientLoc = gl.getUniformLocation(this.m_ShaderId, "alphaCoefficient");
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
     * modifie les coefficients de couleur et de transparence
     * @param colorCoefficient : float de 0 à 1 multiplié par la couleur
     * @param alphaCoefficient : float de 0 à 1 multiplié par le coefficient alpha
     */
    setCoefficients(colorCoefficient=1.0, alphaCoefficient=1.0)
    {
        this.m_ColorCoefficient = colorCoefficient;
        this.m_AlphaCoefficient = alphaCoefficient;
    }


    /**
     * sélectionne la Texture2D correspondant à l'angle
     * @param angle : float entre 0 et 1
     */
    select(angle)
    {
        this.m_Texture = this.m_Texture360.select(angle);
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

        // spécifier le coefficient de teinte et le coefficient de transparence
        gl.uniform1f(this.m_ColorCoefficientLoc, this.m_ColorCoefficient);
        gl.uniform1f(this.m_AlphaCoefficientLoc, this.m_AlphaCoefficient);

        // activer la texture sur l'unité 0
        this.m_Texture.setTextureUnit(gl.TEXTURE0, this.m_TextureLoc);
    }


    /**
     * Cette méthode désactive le matériau
     */
    disable()
    {
        // désactiver les textures
        this.m_Texture.setTextureUnit(gl.TEXTURE0);

        // appeler la méthode de la superclasse
        super.disable();
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        super.destroy();
    }
}
