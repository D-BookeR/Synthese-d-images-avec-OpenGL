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
            out vec4 glFragColor;

            void main()
            {
                // multiplier (s,t) afin de faire une répétition
                vec2 texCoord = fract(frgTexCoord * vec2(16.0, 8.0));
                // distance entre (s,t) et le centre (0.5, 0.5)
                float dist = distance(texCoord, vec2(0.5,0.5));
                // selon la distance : jaune ou rouge
                if (dist > 0.4) {
                    glFragColor = vec4(1.0, 1.0, 0.1, 1.0);
                } else {
                    glFragColor = vec4(1.0, 0.0, 0.0, 1.0);
                }
            }`;
        return srcFragmentShader;
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
