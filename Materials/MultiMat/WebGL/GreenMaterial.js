// Définition de la classe GreenMaterial

// superclasses et classes nécessaires
Requires("libs/Material/Material.js");
Requires("libs/Material/VBOset.js");


class GreenMaterial extends Material
{
    /**
     * Constructeur
     */
    constructor()
    {
        super("GreenMaterial");

        // compiler le shader
        this.compileShader();
    }


    /**
     * retourne le source du Vertex Shader
     */
    getVertexShader()
    {
        let srcVertexShader =
            `#version 300 es
            in vec3 glVertex;
            uniform mat4 mat4ModelView;
            uniform mat4 mat4Projection;
            void main()
            {
                gl_Position = mat4Projection * mat4ModelView * vec4(glVertex, 1.0);
            }`;
        return srcVertexShader;
    }


    /**
     * retourne le source du Fragment Shader
     */
    getFragmentShader()
    {
        let srcFragmentShader =
            `#version 300 es
            precision mediump float;
            uniform float Time;
            out vec4 glFragColor;
            void main()
            {
                float phase = cos(Time*2.0);
                glFragColor = vec4(0.0, 0.6+0.2*phase, 0.2-0.1*phase, 1.0);
            }`;
        return srcFragmentShader;
    }


    /**
     * compile ou recompile le shader
     */
    compileShader()
    {
        // appeler la méthode de la superclasse
        super.compileShader();

        // déterminer où sont les variables uniform
        this.m_TimeLoc = gl.getUniformLocation(this.m_ShaderId, "Time");
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

        // fournir le temps
        gl.uniform1f(this.m_TimeLoc, Utils.Time);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        super.destroy();
    }
}
