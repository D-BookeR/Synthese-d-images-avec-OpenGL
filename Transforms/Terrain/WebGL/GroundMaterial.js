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
     * @param diffuse : nom d'un fichier image contenant la texture diffuse
     * @param Ks : vec3
     * @param Ns : poli du matériau
     */
    constructor(heightmap, hmax, delta, diffuse, Ks, Ns)
    {
        super("GroundMaterial");

        // charger la texture donnant le relief
        this.m_TxHeightmap = new Texture2D(heightmap);
        this.m_TxHeightmapLoc = -1;

        // charger la texture diffuse
        this.m_TxDiffuse = new Texture2D(diffuse);
        this.m_TxDiffuseLoc = -1;

        this.m_HMax = hmax;
        this.m_Delta = delta;
        this.m_Ks = Ks;
        this.m_Ns = Ns;

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
            out vec4 frgPosition;
            out vec2 frgTexCoord;
            out vec3 frgNormal;

            // matrices de transformation
            uniform mat4 mat4ModelView;
            uniform mat4 mat4Projection;
            uniform mat3 mat3Normal;

            void main()
            {
                // transformation du point par la heightmap
                vec3 position = glVertex;
                float height = texture(txHeightmap, glTexCoord).g * hmax;
                position.y += height;
                // position du fragment par rapport à la caméra et projection écran
                frgPosition = mat4ModelView * vec4(position, 1.0);
                gl_Position = mat4Projection * frgPosition;
                // calcul de la normale locale
                float heightN = texture(txHeightmap, glTexCoord+vec2(0.0,+delta)).g;
                float heightS = texture(txHeightmap, glTexCoord+vec2(0.0,-delta)).g;
                float heightE = texture(txHeightmap, glTexCoord+vec2(+delta,0.0)).g;
                float heightW = texture(txHeightmap, glTexCoord+vec2(-delta,0.0)).g;
                float dX = (heightE - heightW) * hmax;
                float dZ = (heightS - heightN) * hmax;
                vec3 N = vec3(-dX, 2.0*delta, -dZ);
                frgNormal = mat3Normal * N;
                // coordonnées de texture
                frgTexCoord = glTexCoord;
            }`;
        return srcVertexShader;
    }


    /**
     * retourne le source du Fragment Shader
     */
    getFragmentShader()
    {
        let fragdata1 = "vec4(0.0)";
        if (this.m_Ns >= 0.0) {
            fragdata1 = "vec4(Ks, Ns)";
        }
        let srcFragmentShader = dedent
            `#version 300 es
            precision mediump float;

            // texture diffuse
            uniform sampler2D txDiffuse;

            // données interpolées venant du vertex shader
            in vec4 frgPosition;
            in vec2 frgTexCoord;
            in vec3 frgNormal;

            // g-buffer
            out vec4 glFragData[4];

            void main()
            {
                // couleur diffuse
                vec4 Kd = texture(txDiffuse, frgTexCoord);

                // couleur spéculaire
                const vec3 Ks = ${vec3.str(this.m_Ks)};
                const float Ns = float(${this.m_Ns.toFixed(2)});

                // calcul du vecteur normal
                vec3 N = normalize(frgNormal);

                // remplir les buffers MRT avec les informations nécessaires pour Phong plus tard
                glFragData[0] = Kd;
                glFragData[1] = ${fragdata1};
                glFragData[2] = vec4(frgPosition.xyz, 1.0);
                glFragData[3] = vec4(N, 0.0);
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
        this.m_TxDiffuseLoc   = gl.getUniformLocation(this.m_ShaderId, "txDiffuse");
        this.m_TxHeightmapLoc = gl.getUniformLocation(this.m_ShaderId, "txHeightmap");
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

        // activer la texture diffuse sur l'unité 1
        this.m_TxDiffuse.setTextureUnit(gl.TEXTURE1, this.m_TxDiffuseLoc);
    }


    /**
     * Cette méthode désactive le matériau
     * NB: le shader doit être activé
     */
    disable()
    {
        // désactiver les textures
        this.m_TxHeightmap.setTextureUnit(gl.TEXTURE0, null);
        this.m_TxDiffuse.setTextureUnit(gl.TEXTURE1, null);

        // appeler la méthode de la superclasse
        super.disable();
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_TxDiffuse.destroy();
        this.m_TxHeightmap.destroy();
        super.destroy();
    }
}
