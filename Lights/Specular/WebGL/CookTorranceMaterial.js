// Définition de la classe CookTorranceMaterial

// superclasses et classes nécessaires
Requires("libs/Material/Material.js");
Requires("libs/Material/VBOset.js");


class CookTorranceMaterial extends Material
{
    /**
     * Cette classe définit un matériau pour dessiner le maillage
     * @param lights_count : nombre de lampes à gérer
     * @param diffuse : fournir un vec3 définissant la couleur diffuse du matériau
     * @param specular : fournir un vec3 définissant la couleur spéculaire du matériau
     * @param m : float >0..1 définissant l'aspect lisse (poche de 0) ou rugueux (proche de 0.5)
     * @param f0 : reflectance à la verticale
     */
    constructor(lights_count, diffuse, specular, m, f0)
    {
        super("CookTorranceMaterial");

        // caractéristiques du matériau
        this.m_DiffuseColor = diffuse;
        this.m_SpecularColor = specular;
        this.m_M = m;
        this.m_F0 = f0;

        // coordonnées et couleurs des sources de lumières
        this.m_LightsPositions = [];
        this.m_LightsColors = [];
        for (let i=0; i<lights_count; i++) {
            this.m_LightsPositions.push(vec4.create());
            this.m_LightsColors.push(vec3.create());
        }

        // compiler le shader
        this.compileShader();
    }


    /**
     * définit la position d'une lampe
     * @param num : numéro de la lampe
     * @param position : vec4 indiquant où elle se trouve dans l'espace caméra
     * @param color : vec3 indiquant sa couleur et son intensité
     */
    setLightPositionColor(num, position, color)
    {
        vec4.copy(this.m_LightsPositions[num], position);
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
            uniform mat3 mat3Normal;

            // attributs de sommets
            in vec3 glVertex;
            in vec3 glNormal;

            // interpolation vers les fragments
            out vec4 frgPosition;
            out vec3 frgNormal;

            void main()
            {
                frgPosition = mat4ModelView * vec4(glVertex, 1.0);
                gl_Position = mat4Projection * frgPosition;
                frgNormal = mat3Normal * glNormal;
            }`;
        return srcVertexShader;
    }


    /**
     * retourne le source du Fragment Shader
     */
    getFragmentShader()
    {
        let lights_count = this.m_LightsPositions.length;

        let srcFragmentShader = dedent
            `#version 300 es

            // interpolation vers les fragments
            precision mediump float;
            in vec4 frgPosition;
            in vec3 frgNormal;

            // sources de lumière
            uniform vec4 LightsPositions[${lights_count}];
            uniform vec3 LightsColors[${lights_count}];

            // caractéristiques du matériau
            const vec3 Kd = ${vec3.str(this.m_DiffuseColor)};
            const vec3 Ks = ${vec3.str(this.m_SpecularColor)};
            const float m = ${this.m_M.toFixed(2)};
            const float f0 = ${this.m_F0.toFixed(2)};

            out vec4 glFragColor;

            void main()
            {
                // normale normalisée (après interpolation)
                vec3 N = normalize(frgNormal);

                // direction vers l'oeil au niveau du fragment
                vec3 V = -normalize(frgPosition.xyz);

                // couleur finale = cumul des éclairements
                vec3 sum = vec3(0.0, 0.0, 0.0);

                // boucle sur les lampes
                for (int i=0; i<${lights_count}; i++) {

                    // vecteur allant du point à la lumière
                    vec3 L = normalize(LightsPositions[i].xyz - frgPosition.xyz);

                    // produit scalaire entre L et la normale = cosinus de l'angle entre L et N
                    float dotNL = clamp(dot(N, L), 0.0, 1.0);

                    // cos de l'angle entre la normale et la vue
                    float dotNV = clamp(dot(N, V), 0.0, 1.0);

                    // demi-vecteur H entre L et V, normalisé
                    vec3 H = normalize(L + V);
                    float dotNH = clamp(dot(N, H), 0.0, 1.0);

                    // terme de rugosité
                    float dotNH2 = dotNH * dotNH;
                    float m2 = m * m;
                    float a = 1.0 / (4.0 * m2 * dotNH2 * dotNH2);
                    float b = (dotNH2 - 1.0) / (m2 * dotNH2);
                    float R = a * exp(b);

                    // terme géométrique
                    float dotVH = clamp(dot(V,H), 0.0, 1.0);
                    float k = 2.0 * dotNH / dotVH;
                    float g1 = k * dotNV;
                    float g2 = k * dotNL;
                    float G = clamp(min(g1, g2), 0.0, 1.0);

                    // terme de Fresnel
                    float F = f0 + (1.0 - f0) * pow(1.0 - dotVH, 5.0);

                    // coefficient spéculaire complet
                    float rs = clamp(F * R * G / (dotNV * dotNL), 0.0, 1.0);

                    // cumul des contributions lumineuses
                    sum += LightsColors[i] * ( dotNL * (Kd + Ks * rs) );
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
        this.m_LightsPositionsLoc = gl.getUniformLocation(this.m_ShaderId, "LightsPositions");
        this.m_LightsColorsLoc  = gl.getUniformLocation(this.m_ShaderId, "LightsColors");
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
        vec4.glUniform(this.m_LightsPositionsLoc, this.m_LightsPositions);
        vec3.glUniform(this.m_LightsColorsLoc,  this.m_LightsColors);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        super.destroy();
    }
}
