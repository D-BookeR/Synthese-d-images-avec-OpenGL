// Définition de la classe ColorMaterial

// superclasses et classes nécessaires
Requires("libs/Material/Material.js");
Requires("libs/Material/VBOset.js");


class ColorPerVertexMaterial extends Material
{
    /**
     * Constructeur
     */
    constructor()
    {
        super("ColorPerVertexMaterial");

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
            in vec3 glColor;
            out vec3 frgColor;
            uniform mat4 mat4ModelView;
            uniform mat4 mat4Projection;
            void main()
            {
                gl_Position = mat4Projection * mat4ModelView * vec4(glVertex, 1.0);
                frgColor = glColor;
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
            uniform float Time;
            in vec3 frgColor;
            out vec4 glFragColor;
            void main()
            {
                // désaturer la couleur en fonction du temps
                const vec3 coefs = vec3(0.29,0.58,0.11);
                float lum = dot(frgColor, coefs);
                vec3 gray = vec3(lum, lum, lum);
                float k = 0.65 + 0.35*sin(Time*2.0);
                vec3 desat = mix(gray, frgColor, k);
                glFragColor = vec4(desat, 1.0);
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
        this.m_TimeLoc = gl.getUniformLocation(this.m_ShaderId, "Time");
    }


    /**
     * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
     * @return le VBOset du matériau
     */
    createVBOset()
    {
        // créer le VBOset et spécifier les noms des attribute nécessaires à ce matériau
        let vboset = super.createVBOset();
        vboset.addAttribute(MeshVertex.ID_ATTR_COLOR,  Utils.VEC3, "glColor");
        return vboset;
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
