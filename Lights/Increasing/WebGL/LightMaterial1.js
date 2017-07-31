// Définition de la classe LightMaterial1

// superclasses et classes nécessaires
Requires("libs/Material/Material.js");
Requires("libs/Material/VBOset.js");


class LightMaterial1 extends Material
{
    /**
     * Cette classe définit un matériau pour dessiner le maillage
     * @param color : fournir un vec3 définissant la couleur diffuse du matériau
     */
    constructor(color)
    {
        super("LightMaterial1");

        // caractéristiques du matériau
        this.m_Color = color;

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
        let srcFragmentShader = dedent
            `#version 300 es

            // interpolation vers les fragments
            precision mediump float;
            in vec4 frgPosition;
            in vec3 frgNormal;

            // source de lumière (coordonnées globales !)
            vec3 LightPosition = vec3(5.0, 5.0, -2.0);

            // caractéristiques du matériau
            const vec3 Kd = ${vec3.str(this.m_Color)};

            out vec4 glFragColor;

            void main()
            {
                // normale normalisée (après interpolation)
                vec3 N = normalize(frgNormal);

                // vecteur allant du point à la lumière
                vec3 L = normalize(LightPosition - frgPosition.xyz);

                // produit scalaire entre L et la normale
                float dotNL = clamp(dot(N, L), 0.0, 1.0);

                // couleur résultante
                glFragColor = vec4(Kd*dotNL, 1.0);
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
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        super.destroy();
    }
}
