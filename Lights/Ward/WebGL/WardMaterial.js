// Définition de la classe WardMaterial
// voir http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/

// superclasses et classes nécessaires
Requires("libs/Material/Material.js");
Requires("libs/Material/VBOset.js");


/**
 * Cette fonction définit un matériau pour dessiner le maillage
 */
class WardMaterial extends Material
{
    /**
     * Cette classe définit un matériau pour dessiner le maillage
     * @param lights_count : nombre de lampes à gérer
     * @param diffuse : fournir un vec3 définissant la couleur diffuse du matériau
     * @param specular : fournir un vec3 définissant la couleur spéculaire du matériau
     * @param alpha : vec2 coefficient de Ward
     */
    constructor(lights_count, diffuse, specular, alpha)
    {
        super("WardMaterial");

        // caractéristiques du matériau
        this.m_DiffuseColor = diffuse;
        this.m_SpecularColor = specular;
        this.m_Alpha = alpha;

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
            in vec3 glVertex;
            in vec3 glNormal;
            in vec3 glTangent;
            out vec4 frgPosition;
            out vec3 frgNormal;
            out vec3 frgTangent;
            uniform mat4 mat4ModelView;
            uniform mat4 mat4Projection;
            uniform mat3 mat3Normal;
            void main()
            {
                frgPosition = mat4ModelView * vec4(glVertex, 1.0);
                gl_Position = mat4Projection * frgPosition;
                frgNormal = mat3Normal * glNormal;
                frgTangent = mat3Normal * glTangent;
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
            precision mediump float;

            // informations géométriques
            in vec4 frgPosition;
            in vec3 frgNormal;
            in vec3 frgTangent;

            // sources de lumière
            uniform vec4 LightsPositions[${lights_count}];
            uniform vec3 LightsColors[${lights_count}];

            // caractéristiques du matériau
            const vec3 Kd = ${vec3.str(this.m_DiffuseColor)};
            const vec3 Ks = ${vec3.str(this.m_SpecularColor)};
            const vec2 alpha = ${vec2.str(this.m_Alpha)};

            const float pi = 3.14159;

            out vec4 glFragColor;

            void main()
            {
                // reconstruire le repère de Frenet normalisé
                vec3 N = normalize(frgNormal);
                vec3 T = normalize(frgTangent);
                vec3 B = cross(N, T);

                // direction du vecteur Vue au niveau du fragment
                vec3 V = normalize(-frgPosition.xyz);
                float dotNV = clamp(dot(N, V), 0.0, 1.0);

                // couleur finale = cumul des éclairements
                vec3 sum = vec3(0.0, 0.0, 0.0);

                // boucle sur les lampes
                for (int i=0; i<${lights_count}; i++) {

                    // vecteur allant du point à la lumière
                    vec3 L = normalize(LightsPositions[i].xyz - frgPosition.xyz);

                    // modulation de l'éclairement type Lambert
                    float dotNL = clamp(dot(N, L), 0.0, 1.0);

                    // demi-vecteur H entre L et V, normalisé (!)
                    vec3 H = normalize(L + V);
                    float dotNH = clamp(dot(N, H), 0.0, 1.0);

                    // éclairement spéculaire Ward
                    float hts = dot(H, T) / alpha.s;
                    float hbs = dot(H, B) / alpha.t;
                    float beta = -2.0 * (hts*hts + hbs*hbs)/(1.0 + dotNH);
                    float s = exp(beta) / (4.0 * pi * alpha.s * alpha.t * sqrt(dotNL*dotNV));

                    // cumul des contributions lumineuses
                    sum += LightsColors[i] * ( (Kd + Ks * s) * dotNL );
                }

                glFragColor = vec4(sum, 1.0);
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
        vboset.addAttribute(MeshVertex.ID_ATTR_TANGENT,  Utils.VEC3, "glTangent");
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
