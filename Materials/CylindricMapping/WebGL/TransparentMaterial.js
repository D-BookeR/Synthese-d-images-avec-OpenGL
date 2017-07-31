/**
 * Définition de la classe TransparentMaterial, une spécialisation de Material
 */

// superclasses et classes nécessaires
Requires("libs/Material/Texture2D.js");
Requires("libs/Material/Material.js");
Requires("libs/Material/VBOset.js");


class TransparentMaterial extends Material
{
    /**
     * Constructeur de la classe TransparentMaterial.
     * @param texture Texture2D à appliquer
     * @param alpha transparence du matériau
     */
    constructor(texture, alpha=1.0)
    {
        super("TransparentMaterial");

        // propriétés
        this.m_Texture = texture;
        this.m_TextureLoc = -1;
        this.m_Alpha = alpha;

        // identifiants des textures associées aux sampler2D DepthMap*
        this.m_DepthMapFar = null;
        this.m_DepthMapNear = null;

        // dimensions de la fenêtre pour normaliser gl_FragCoord.xy
        this.m_WindowSize = vec2.fromValues(1.0, 1.0);

        // recompiler le shader
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
            uniform sampler2D txColor;
            uniform sampler2D DepthMapFar;
            uniform sampler2D DepthMapNear;
            uniform vec2 WindowSize;
            out vec4 glFragColor;
            void main()
            {
                // coordonnées dans les depth maps
                vec2 fragcoords = gl_FragCoord.xy/WindowSize;

                // comparaison avec la distance lointaine
                float depth = texture(DepthMapFar, fragcoords).x;
                if (gl_FragCoord.z >= depth) discard;

                // comparaison avec la distance proche
                depth = texture(DepthMapNear, fragcoords).x;
                if (gl_FragCoord.z <= depth) discard;

                // retourner la couleur de la texture prémultipliée par la transparence
                glFragColor = texture(txColor, frgTexCoord) * ${this.m_Alpha.toFixed(2)};
            }`;
        return srcFragmentShader;
    }


    /**
     * compile le shader du matériau
     */
    compileShader()
    {
        // appeler la méthode de la superclasse
        super.compileShader();

        // déterminer où sont les variables uniform spécifiques
        this.m_TextureLoc      = gl.getUniformLocation(this.m_ShaderId, "txColor");
        this.m_DepthMapFarLoc  = gl.getUniformLocation(this.m_ShaderId, "DepthMapFar");
        this.m_DepthMapNearLoc = gl.getUniformLocation(this.m_ShaderId, "DepthMapNear");
        this.m_WindowSizeLoc   = gl.getUniformLocation(this.m_ShaderId, "WindowSize");
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
     * affecte la texture DepthMap du shader
     * @param far : depth buffer qui limite les dessins arrière de ce matériau
     * @param near : depth buffer qui limite les dessins avant de ce matériau
     */
    setDepthMaps(far, near)
    {
        this.m_DepthMapFar  = far;
        this.m_DepthMapNear = near;
    }


    /**
     * spécifie la taille de la fenêtre
     * @param width : largeur en nombre de pixels de la fenêtre
     * @param height : hauteur en nombre de pixels de la fenêtre
     */
    setWindowDimensions(width, height)
    {
        this.m_WindowSize = vec2.fromValues(width, height);
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

        // activer la texture sur l'unité 0
        this.m_Texture.setTextureUnit(gl.TEXTURE0, this.m_TextureLoc);

        // fournir et activer la DepthMapFar dans l'unité 1
        gl.activeTexture(gl.TEXTURE1);
        gl.bindTexture(gl.TEXTURE_2D, this.m_DepthMapFar);
        gl.uniform1i(this.m_DepthMapFarLoc, gl.TEXTURE1-gl.TEXTURE0);

        // fournir et activer la DepthMapNear dans l'unité 2
        gl.activeTexture(gl.TEXTURE2);
        gl.bindTexture(gl.TEXTURE_2D, this.m_DepthMapNear);
        gl.uniform1i(this.m_DepthMapNearLoc, gl.TEXTURE2-gl.TEXTURE0);

        // fournir les dimensions de la fenêtre
        vec2.glUniform(this.m_WindowSizeLoc, this.m_WindowSize);
    }


    /**
     * Cette méthode désactive le matériau
     * @note le shader doit être activé
     */
    disable()
    {
        // désactiver les textures
        this.m_Texture.setTextureUnit(gl.TEXTURE0);

        // désactiver les depth maps
        gl.activeTexture(gl.TEXTURE1);
        gl.bindTexture(gl.TEXTURE_2D, null);
        gl.activeTexture(gl.TEXTURE2);
        gl.bindTexture(gl.TEXTURE_2D, null);

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
