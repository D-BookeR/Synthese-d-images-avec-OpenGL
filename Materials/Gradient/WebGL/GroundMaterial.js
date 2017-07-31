// Définition de la classe GroundMaterial

// superclasses et classes nécessaires
Requires("libs/Material/Material.js");
Requires("libs/Material/Texture2D.js");
Requires("libs/Material/VBOset.js");


class GroundMaterial extends Material
{
    /**
     * Cette fonction définit la classe GroundMaterial pour dessiner le terrain.
     * @param heightmap : nom d'un fichier image contenant le relief
     * @param hmax : float qui donne la hauteur relative du terrain, ex: 0.4
     * @param delta : float qui indique la distance pour calculer la normale
     */
    constructor(heightmap, hmax, delta)
    {
        super("GroundMaterial");

        // charger le relief
        this.m_TxHeightmap = new Texture2D(heightmap);
        this.m_TxHeightmapLoc = -1;

        this.m_HMax = hmax;
        this.m_Delta = delta;

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

            // attributs de sommets
            in vec3 glVertex;
            in vec2 glTexCoord;

            // paramètres du matériau
            const float delta = ${this.m_Delta.toFixed(4)};
            const float hmax = ${this.m_HMax.toFixed(3)};
            uniform sampler2D txHeightmap;

            // interpolation pour le fragment shader
            out vec3 frgPosition;
            out vec3 frgNormal;

            // matrices de transformation
            uniform mat4 mat4ModelView;
            uniform mat4 mat4Projection;

            void main()
            {
                // transformation du point par la heightmap
                frgPosition = glVertex;
                float height = texture(txHeightmap, glTexCoord).g * hmax;
                frgPosition.y += height;
                // position du fragment par rapport à la caméra et projection écran
                gl_Position = mat4Projection * mat4ModelView * vec4(frgPosition, 1.0);
                // calcul de la normale locale
                float heightN = texture(txHeightmap, glTexCoord+vec2(0.0,+delta)).g;
                float heightS = texture(txHeightmap, glTexCoord+vec2(0.0,-delta)).g;
                float heightE = texture(txHeightmap, glTexCoord+vec2(+delta,0.0)).g;
                float heightW = texture(txHeightmap, glTexCoord+vec2(-delta,0.0)).g;
                float dX = (heightE - heightW) * hmax;
                float dZ = (heightS - heightN) * hmax;
                frgNormal = vec3(-dX, 2.0*delta, -dZ);
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
            const float hmax = ${this.m_HMax}; // hauteur relative maximale

            // données interpolées venant du vertex shader
            in vec3 frgPosition;
            in vec3 frgNormal;

            out vec4 glFragColor;

            void main()
            {
                // normaliser le vecteur normal local
                vec3 Nlocal = normalize(frgNormal);

                // altitude et pente
                float elevation = frgPosition.y/hmax;
                float slope = 2.0 * (1.0 - Nlocal.y);

                // couleur diffuse : dépend de l'altitude et de la pente
                glFragColor = vec4(elevation, slope, 0.7-elevation, 1.0);
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
        this.m_TxHeightmapLoc     = gl.getUniformLocation(this.m_ShaderId, "txHeightmap");
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
     * Cette méthode active le matériau : met en place son shader,
     * fournit les variables uniform qu'il demande
     * @param mat4Projection : mat4 contenant la projection
     * @param mat4ModelView : mat4 contenant la transformation vers la caméra
     */
    enable(mat4Projection, mat4ModelView)
    {
        // appeler la méthode de la superclasse
        super.enable(mat4Projection, mat4ModelView);

        // activer la texture d'altitude sur l'unité 0
        this.m_TxHeightmap.setTextureUnit(gl.TEXTURE0, this.m_TxHeightmapLoc);
    }


    /**
     * Cette méthode désactive le matériau
     * NB: le shader doit être activé
     */
    disable()
    {
        // désactiver les textures
        this.m_TxHeightmap.setTextureUnit(gl.TEXTURE0, null);

        // appeler la méthode de la superclasse
        super.disable();
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_TxHeightmap.destroy();
        super.destroy();
    }
}
