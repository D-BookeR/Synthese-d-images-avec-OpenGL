// Définition de la classe TwistMaterial

// superclasses et classes nécessaires
Requires("libs/Material/Texture2D.js");
Requires("libs/Material/Material.js");
Requires("libs/Material/VBOset.js");


class TwistMaterial extends Material
{
    /**
     * Constructeur
     */
    constructor()
    {
        super("TwistMaterial");

        // charger la texture
        this.m_Texture = new Texture2D("data/textures/earth/earth_map.jpg");
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
            out vec4 glFragColor;
            in vec2 frgTexCoord;

            uniform sampler2D txColor;
            uniform float AngleMax;

            const float PI = 3.1415927;

            void main()
            {
                // modification des coordonnées de texture
                vec2 texcoord = frgTexCoord;

                // la rotation est d'autant plus forte qu'on est proche du centre
                float effet = pow(sin(texcoord.s * PI) * sin(texcoord.t * PI), 3.0);
                float angle = radians(AngleMax) * effet;

                // produire une matrice de rotation 2D
                float cosa = cos(angle);
                float sina = sin(angle);
                mat2 rotation = mat2(cosa,sina,-sina,cosa);

                // appliquer cette rotation aux coordonnées de texture
                const vec2 center = vec2(0.5, 0.5);
                texcoord = rotation * (texcoord-center) + center;

                glFragColor = texture(txColor, texcoord);
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
        this.m_TextureLoc   = gl.getUniformLocation(this.m_ShaderId, "txColor");
        this.m_AngleMaxLoc  = gl.getUniformLocation(this.m_ShaderId, "AngleMax");
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
     * modifie l'angle maximal de la torsion
     * @param anglemax en degrés
     */
    setMaxAngle(anglemax)
    {
        // activer le shader
        gl.useProgram(this.m_ShaderId);

        // fournir le nouvel angle
        gl.uniform1f(this.m_AngleMaxLoc, anglemax);

        // désactiver le shader
        gl.useProgram(null);
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
