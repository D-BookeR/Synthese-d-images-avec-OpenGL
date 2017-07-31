// Définition de la classe ProceduralMaterial

// superclasses et classes nécessaires
Requires("libs/Material/Material.js");
Requires("libs/Material/VBOset.js");


class ProceduralMaterial extends Material
{
    /**
     * Constructeur
     */
    constructor()
    {
        super("ProceduralMaterial");

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
                // la distance du fragment au point (0.5, 0.5)
                float d = min(distance(frgTexCoord.st, vec2(0.5, 0.5)), 1.0);
                // couleurs fonction de la distance
                float red   = 1.0 - d;
                float green = 0.5 - d;
                float blue  = d;
                glFragColor = vec4(red, green, blue, 1.0);
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
