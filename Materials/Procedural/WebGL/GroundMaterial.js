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
            const float PI = 3.14159;
            void main()
            {
                // multiplier (s,t) pour une répétition de 6 fois
                vec2 texCoord = frgTexCoord * 6.0;
                // appliquer une fonction sinus pour obtenir des vagues
                texCoord = abs(sin(texCoord * PI));
                // appliquer une fonction puissance pour «durcir» la variation
                float red = pow(texCoord.s, 8.0);
                float blue  = pow(texCoord.t, 8.0);
                // couleur = fonction des coordonnées de texture
                glFragColor = vec4(red, 0.7, blue, 1.0);
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
