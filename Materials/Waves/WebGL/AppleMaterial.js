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

            const float PI = 3.14159;

            out vec4 glFragColor;

            void main()
            {
                // rotation de 25° des coordonnées de texture
                const float angle = 25.0 * PI/180.0;
                float k = sin(angle)*frgTexCoord.s - cos(angle)*frgTexCoord.t;
                // 8 ondulations par unité de distance
                float f = 0.5*sin(2.0*PI*(8.0*k))+0.5;
                // action sur le vert et le bleu => cyan un peu sombre
                glFragColor = vec4(0, f*0.8, f, 1.0);
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
