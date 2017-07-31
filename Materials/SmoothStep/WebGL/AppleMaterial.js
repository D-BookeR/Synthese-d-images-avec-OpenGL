// Définition de la classe AppleMaterial

// superclasses et classes nécessaires
Requires("libs/Material/Material.js");
Requires("libs/Material/VBOset.js");


class AppleMaterial extends Material
{
    /**
     * Constructeur
     */
    constructor()
    {
        super("AppleMaterial");

        // compiler le shader
        this.compileShader();
    }


    /**
     * retourne le source du Vertex Shader
     */
    getVertexShader()
    {
        return dedent
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
    }


    /**
     * retourne le source du Fragment Shader
     */
    getFragmentShader()
    {
        return dedent
            `#version 300 es
            precision mediump float;
            in vec2 frgTexCoord;

            const vec4 vert = vec4(0.2, 1.0, 0.3, 1.0);
            const vec4 jaune = vec4(0.9, 0.7, 0.1, 1.0);

            out vec4 glFragColor;

            void main()
            {
                // "hauteur relative" du fragment dans les coordonnées de texture
                float qt = smoothstep(0.4, 0.6, frgTexCoord.t);
                // la couleur est un mélange en fonction de cette hauteur
                glFragColor = mix(jaune, vert, qt);
            }`;
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
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        super.destroy();
    }
}
