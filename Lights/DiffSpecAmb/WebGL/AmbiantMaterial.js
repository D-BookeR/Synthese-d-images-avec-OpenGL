// Définition de la classe AmbiantMaterial

// superclasses et classes nécessaires
Requires("libs/Material/Material.js");
Requires("libs/Material/VBOset.js");


class AmbiantMaterial extends Material
{
    /**
     * Cette classe définit un matériau pour dessiner le maillage
     * @param lights_count : nombre de lampes à gérer
     * @param ambiant : fournir un vec3 définissant la couleur diffuse du matériau
     * @param diffuse : fournir une texture définissant la couleur diffuse du matériau
     */
    constructor(lights_count, ambiant, diffuse)
    {
        super("AmbiantMaterial");

        // caractéristiques du matériau
        this.m_AmbiantColor = ambiant;
        this.m_DiffuseTexture = diffuse;

        // couleurs des sources de lumières
        this.m_LightsColors = [];
        for (let i=0; i<lights_count; i++) {
            this.m_LightsColors.push(vec3.create());
        }

        // compiler le shader
        this.compileShader();
    }


    /**
     * définit la position et l'intensité d'une lampe
     * @param num : numéro de la lampe
     * @param position : vec4 indiquant où elle se trouve dans l'espace caméra
     * @param color : vec3 indiquant sa couleur et son intensité
     */
    setLightPositionColor(num, position, color)
    {
        vec3.copy(this.m_LightsColors[num], color);
    }


    /**
     * retourne le source du Vertex Shader
     */
    getVertexShader()
    {
        let srcVertexShader = dedent
            `#version 300 es

            // paramètres uniform
            uniform mat4 mat4ModelView;
            uniform mat4 mat4Projection;

            // attributs de sommets
            in vec3 glVertex;
            in vec2 glTexCoord;

            // interpolation vers les fragments
            out vec2 frgTexCoord;

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
        let lights_count = this.m_LightsColors.length;

        let srcFragmentShader = dedent
            `#version 300 es

            // interpolation vers les fragments
            precision mediump float;
            in vec2 frgTexCoord;

            // sources de lumière
            uniform vec3 LightsColors[${lights_count}];

            // caractéristiques du matériau
            const vec3 Ka = ${vec3.str(this.m_AmbiantColor)};
            uniform sampler2D txColor;

            out vec4 glFragColor;

            void main()
            {
                // récupération de la couleur diffuse
                vec3 Kd = texture(txColor, frgTexCoord).rgb;

                // couleur finale = cumul des éclairements
                vec3 sum = vec3(0.0, 0.0, 0.0);

                // boucle sur les lampes
                for (int i=0; i<${lights_count}; i++) {

                    // cumul des contributions lumineuses
                    sum += LightsColors[i] * Ka * Kd;
                }

                glFragColor = vec4(sum, 1.0);
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
        this.m_LightsColorsLoc  = gl.getUniformLocation(this.m_ShaderId, "LightsColors");
        this.m_TextureLoc         = gl.getUniformLocation(this.m_ShaderId, "txColor");
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

        // initialisation des lampes
        vec3.glUniform(this.m_LightsColorsLoc,  this.m_LightsColors);

        // activer la texture sur l'unité 0
        this.m_DiffuseTexture.setTextureUnit(gl.TEXTURE0, this.m_TextureLoc);
    }


    /**
     * Cette méthode désactive le matériau
     */
    disable()
    {
        // désactiver les textures
        this.m_DiffuseTexture.setTextureUnit(gl.TEXTURE0);

        // appeler la méthode de la superclasse
        super.disable();
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        super.destroy();
    }
}
