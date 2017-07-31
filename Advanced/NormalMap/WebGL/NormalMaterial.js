// Définition de la classe NormalMaterial
// voir http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/

// superclasses et classes nécessaires
Requires("libs/Material/Texture2D.js");
Requires("libs/Material/Material.js");
Requires("libs/Material/VBOset.js");


/**
 * Cette fonction définit un matériau pour dessiner le maillage
 */
class NormalMaterial extends Material
{
    constructor()
    {
        super("NormalMaterial");

        // charger les textures
        this.m_DiffuseTexture = new Texture2D("data/textures/BenCloward/metalSheet/diffuse.jpg", gl.LINEAR, gl.REPEAT);
        this.m_DiffuseTextureLoc = -1;
        this.m_NormalTexture = new Texture2D("data/textures/BenCloward/metalSheet/normal.jpg", gl.LINEAR, gl.REPEAT);
        this.m_NormalTextureLoc = -1;

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
            // matrices de changement de repère
            uniform mat4 mat4ModelView;
            uniform mat4 mat4Projection;
            uniform mat3 mat3Normal;

            // VBOs contenant le repère de Frenet complet
            in vec3 glVertex;
            in vec3 glNormal;
            in vec3 glTangent;
            in vec2 glTexCoord;

            // variables à interpoler pour le fragment shader
            out vec4 frgPosition;
            out vec3 frgNormal;
            out vec3 frgTangent;
            out vec2 frgTexCoord;

            void main()
            {
                frgPosition = mat4ModelView * vec4(glVertex, 1.0);
                gl_Position = mat4Projection * frgPosition;
                frgNormal = mat3Normal * glNormal;
                frgTangent = mat3Normal * glTangent;
                frgTexCoord = glTexCoord * 5.0; // x5 pour réduire la taille de la texture
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

            // variables interpolées venant du vertex shader
            in vec4 frgPosition;
            in vec3 frgNormal;
            in vec3 frgTangent;
            in vec2 frgTexCoord;

            // textures diffuse et normale
            uniform sampler2D txDiffuse;
            uniform sampler2D txNormal;

            out vec4 glFragColor;

            void main()
            {
                // matrice de passage du repère de Frenet à l'espace global
                vec3 N = normalize(frgNormal);
                vec3 T = normalize(frgTangent);
                vec3 B = cross(N, T);
                mat3 TBN = mat3(T, B, N);

                // vecteur normal dans le repère de Frenet
                vec3 Nfrenet = texture(txNormal, frgTexCoord).xyz * 2.0 - 1.0;

                // changement de repère du vecteur normal extrait de la normal map
                N = normalize(TBN * Nfrenet);

                // caractéristiques locales du matériau
                vec3 Kd = texture(txDiffuse, frgTexCoord).rgb;
                vec3 Ks = vec3(1.0, 1.0, 1.0);
                float Ns = 120.0;

                // direction et couleur de la lumière
                vec3 LightDirection = vec3(0.7, 0.3, 0.5);
                vec3 LightColor = vec3(1.5, 1.5, 1.5);

                // contribution diffuse de Lambert
                vec3 L = normalize(LightDirection);
                float dotNL = clamp(dot(N, L), 0.0, 1.0);

                // contribution spéculaire de Phong
                vec3 R = reflect(normalize(frgPosition.xyz), N);
                float dotRL = clamp(dot(R, L), 0.0, 1.0);

                // couleur finale
                glFragColor = vec4(LightColor*(Kd *dotNL + Ks*pow(dotRL, Ns)), 1.0);
            }`
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
        this.m_DiffuseTextureLoc = gl.getUniformLocation(this.m_ShaderId, "txDiffuse");
        this.m_NormalTextureLoc = gl.getUniformLocation(this.m_ShaderId, "txNormal");
    }


    /**
     * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
     * @return le VBOset du matériau
     */
    createVBOset()
    {
        // créer le VBOset et spécifier les noms des attribute nécessaires à ce matériau
        let vboset = super.createVBOset();
        vboset.addAttribute(MeshVertex.ID_ATTR_NORMAL,   Utils.VEC3, "glNormal");
        vboset.addAttribute(MeshVertex.ID_ATTR_TANGENT,  Utils.VEC3, "glTangent");
        vboset.addAttribute(MeshVertex.ID_ATTR_TEXCOORD, Utils.VEC2, "glTexCoord");
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

        // activer les textures chacune sur une unité différente
        this.m_DiffuseTexture.setTextureUnit(gl.TEXTURE0, this.m_DiffuseTextureLoc);
        this.m_NormalTexture.setTextureUnit(gl.TEXTURE1, this.m_NormalTextureLoc);
    }


    /**
     * Cette méthode désactive le matériau
     */
    disable()
    {
        // désactiver les textures
        this.m_DiffuseTexture.setTextureUnit(gl.TEXTURE0);
        this.m_NormalTexture.setTextureUnit(gl.TEXTURE1);

        // appeler la méthode de la superclasse
        super.disable();
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_NormalTexture.destroy();
        this.m_DiffuseTexture.destroy();
        super.destroy();
    }
}
