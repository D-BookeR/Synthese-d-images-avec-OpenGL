// Définition de la classe SkyboxMaterial

// superclasses et classes nécessaires
Requires("libs/Material/Material.js");
Requires("libs/Material/TextureCube.js");
Requires("libs/Material/VBOset.js");


class SkyboxMaterial extends Material
{
    /**
     * Constructeur
     * @param skybox_basename : chemin d'accès aux 6 images
     */
    constructor(skybox_basename)
    {
        super("SkyboxMaterial");

        // créer une texture de type TEXTURE_CUBE_MAP
        this.m_Texture = new TextureCube(skybox_basename);
        this.m_TextureLoc = -1;

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
            uniform mat4 mat4ModelView;
            uniform mat4 mat4Projection;
            out vec3 frgDirection;
            void main()
            {
                gl_Position = mat4Projection * mat4ModelView * vec4(glVertex, 1.0);
                frgDirection = glVertex.xyz;
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
            uniform samplerCube txCube;
            in vec3 frgDirection;
            out vec4 glFragColor;
            void main()
            {
                glFragColor = texture(txCube, frgDirection);
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
        this.m_TextureLoc = gl.getUniformLocation(this.m_ShaderId, "txCube");
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
        this.m_Texture.destroy();
        super.destroy();
    }
}
