// Définition de la classe GroundMaterial

// superclasses et classes nécessaires
Requires("libs/Material/Material.js");
Requires("libs/Material/VBOset.js");


class GroundMaterial extends Material
{
    /**
     * Constructeur
     */
    constructor()
    {
        super("GroundMaterial");

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

            const vec4 bleu = vec4(0.0, 0.0, 1.0, 1.0);
            const vec4 cyan = vec4(0.2, 0.7, 1.0, 1.0);

            out vec4 glFragColor;

            void main()
            {
                // distance du fragment par rapport à (0.5, 0.5)
                const vec2 center = vec2(0.5, 0.5);
                float dist = distance(frgTexCoord.st, center);
                float q = smoothstep(0.3, 0.5, dist);
                // couleur = mélange en fonction de la distance
                glFragColor = mix(cyan, bleu, q);
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
