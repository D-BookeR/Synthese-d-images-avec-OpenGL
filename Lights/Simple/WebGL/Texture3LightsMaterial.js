// Définition de la classe Texture3LightsMaterial

// superclasses et classes nécessaires
Requires("libs/Material/Material.js");
Requires("libs/Material/VBOset.js");


class Texture3LightsMaterial extends Material
{
    /**
     * Cette classe définit un matériau pour dessiner le maillage
     * @param texture : fournir une Texture2D définissant la couleur diffuse du matériau
     * @param specular : fournir un vec3 définissant la couleur spéculaire du matériau
     * @param shininess : aspect rugueux ou lisse
     */
    constructor(texture, specular, shininess)
    {
        super("Texture3LightsMaterial");

        // caractéristiques du matériau
        this.m_Texture = texture;
        this.m_SpecularColor = specular;
        this.m_Shininess = shininess;

        // coordonnées des sources de lumières dans le repère global
        this.m_LightsPositions = [
            vec3.fromValues( 8.0,  7.0,  1.0),
            vec3.fromValues(-6.0,  5.0, -4.0),
            vec3.fromValues( 2.0,  2.0, -9.0)
        ];

        // intensités des sources de lumière
        this.m_LightsColors = [
            vec3.fromValues(0.8, 0.8, 0.8),
            vec3.fromValues(0.6, 0.6, 0.6),
            vec3.fromValues(0.5, 0.5, 0.5),
        ];

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

            // paramètres uniform
            uniform mat4 mat4ModelView;
            uniform mat4 mat4Projection;
            uniform mat3 mat3Normal;

            // attributs de sommets
            in vec3 glVertex;
            in vec3 glNormal;
            in vec2 glTexCoord;

            // interpolation vers les fragments
            out vec4 frgPosition;
            out vec3 frgNormal;
            out vec2 frgTexCoord;

            void main()
            {
                frgPosition = mat4ModelView * vec4(glVertex, 1.0);
                gl_Position = mat4Projection * frgPosition;
                frgNormal = mat3Normal * glNormal;
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

            // interpolation vers les fragments
            precision mediump float;
            in vec4 frgPosition;
            in vec3 frgNormal;
            in vec2 frgTexCoord;

            // sources de lumière
            uniform vec3 LampesPositions[${this.m_LightsPositions.length}];
            uniform vec3 LampesCouleurs[${this.m_LightsPositions.length}];

            // caractéristiques du matériau
            uniform sampler2D txColor;
            const vec3 Ks = ${vec3.str(this.m_SpecularColor)};
            const float ns = ${this.m_Shininess.toFixed(2)};

            out vec4 glFragColor;

            void main()
            {
                // récupération de la couleur diffuse
                vec3 Kd = texture(txColor, frgTexCoord).rgb;

                // normale normalisée (après interpolation)
                vec3 N = normalize(frgNormal);

                // direction opposée du vecteur Vue au niveau du fragment
                vec3 mV = normalize(frgPosition.xyz);

                // direction du reflet du vecteur Vue
                vec3 R = reflect(mV, N);

                // couleur finale = cumul des éclairements
                vec3 sum = vec3(0.0, 0.0, 0.0);

                // boucle sur les deux lampes
                for (int i=0; i<${this.m_LightsPositions.length}; i++) {

                    // vecteur allant du point à la lumière
                    vec3 L = normalize(LampesPositions[i] - frgPosition.xyz);

                    // produit scalaire entre L et la normale
                    float dotNL = clamp(dot(N, L), 0.0, 1.0);

                    // alignement du reflet de la vue avec la lampe
                    float dotRL = clamp(dot(R,L), 0.0, 1.0);

                    // cumul des contributions lumineuses
                    sum += LampesCouleurs[i] * (Kd*dotNL + Ks*pow(dotRL, ns));
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
        this.m_LightsPositionsLoc = gl.getUniformLocation(this.m_ShaderId, "LampesPositions");
        this.m_LightsColorsLoc    = gl.getUniformLocation(this.m_ShaderId, "LampesCouleurs");
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
        vboset.addAttribute(MeshVertex.ID_ATTR_NORMAL,   Utils.VEC3, "glNormal");
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
        vec3.glUniform(this.m_LightsPositionsLoc, this.m_LightsPositions);
        vec3.glUniform(this.m_LightsColorsLoc,  this.m_LightsColors);

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
        super.destroy();
    }
}
