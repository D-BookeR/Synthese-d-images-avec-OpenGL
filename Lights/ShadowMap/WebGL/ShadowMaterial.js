// Définition de la classe ShadowMaterial, une spécialisation de Material
// Ce matériau permet de dessiner des ombres


// superclasses et classes nécessaires
Requires("libs/Material/Material.js");


/**
 * Une instance de cette classe dessine d'une couleur uniforme
 */
class ShadowMaterial extends Material
{
    /**
     * constructeur
     * @param color couleur du matériau
     */
    constructor(color)
    {
        super("ShadowMaterial");

        // initialisations
        this.m_Color = vec4.clone(color);
        this.m_ShadowMap = null;
        this.m_ShadowMatrix = null;
        this.m_PositionCamera = null;

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
            in vec3 glNormal;
            uniform mat4 mat4ModelView;
            uniform mat4 mat4Projection;
            uniform mat3 mat3Normal;
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
            precision mediump float;
            in vec4 frgPosition;
            in vec3 frgNormal;

            // position de la lampe
            uniform vec3 LightPosition;

            // shadow map et matrice de retour pour cette lampe
            uniform sampler2D ShadowMap;
            uniform mat4 mat4Shadow;

            // rustine pour limiter l'acné de surface ou les murs volants
            const float DECALAGE = 0.0; // -0.005;

            // retourne 1.0 si le point est éclairé, 0.0 s'il est dans l'ombre
            float isIlluminated(vec4 position)
            {
                // calculer les coordonnées du vertex dans la shadow map
                vec4 posshadow = mat4Shadow * position;

                // normaliser les coordonnées homogènes
                posshadow /= posshadow.w;

                // distance entre ce fragment et la lumière
                float distancePointLight = posshadow.z + DECALAGE;

                // comparer la valeur donnée par la ShadowMap avec la distance du fragment à la lumière
                float distanceObstacleLight = texture(ShadowMap, posshadow.xy).r;
                if (distanceObstacleLight < distancePointLight) {
                    // un objet opaque est entre nous et la lumière
                    return 0.4;
                } else {
                    // pas d'ombre
                    return 1.0;
                }
            }

            out vec4 glFragColor;

            void main()
            {
                vec4 Kd = ${vec4.str(this.m_Color)};

                // déterminer la direction de la lumière d'après la matrice d'ombre
                vec3 L = normalize(LightPosition - frgPosition.xyz);

                // vecteur normal
                vec3 N = normalize(frgNormal);
                float dotNL = clamp(dot(N, L), 0.4, 1.0);

                glFragColor = min(isIlluminated(frgPosition), dotNL) * Kd;
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
        this.m_ShadowMapLoc     = gl.getUniformLocation(this.m_ShaderId, "ShadowMap");
        this.m_ShadowMatrixLoc  = gl.getUniformLocation(this.m_ShaderId, "mat4Shadow");
        this.m_LightPositionLoc = gl.getUniformLocation(this.m_ShaderId, "LightPosition");
    }


    /**
     * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
     * @return le VBOset du matériau
     */
    createVBOset()
    {
        // créer le VBOset et spécifier les noms des attribute nécessaires à ce matériau
        let vboset = super.createVBOset();
        vboset.addAttribute(MeshVertex.ID_ATTR_NORMAL, Utils.VEC3, "glNormal");
        return vboset;
    }


    /**
     * fournit la shadow map et sa matrice
     * @param shadowmatrix {mat4} à utiliser
     * @param positioncamera {vec4} position de la lampe dans le repère caméra
     * @param shadowmap {ShadowMap} à utiliser pour les ombres, null si désactivée (pour le dessin dans elle-même)
     */
    setShadowMap(shadowmatrix, positioncamera, shadowmap=null)
    {
        this.m_ShadowMap = shadowmap;
        this.m_ShadowMatrix = shadowmatrix;
        this.m_PositionCamera = positioncamera;
    }


    /**
     * Cette méthode active le matériau : met en place son shader,
     * fournit les variables uniform qu'il demande
     * @param mat4Projection {mat4} mat4 contenant la projection
     * @param mat4ModelView {mat4} mat4 contenant la transformation vers la caméra
     */
    enable(mat4Projection, mat4ModelView)
    {
        // appeler la méthode de la superclasse
        super.enable(mat4Projection, mat4ModelView);

        if (this.m_ShadowMap != null) {

            // associer la shadowmap à l'unité 0
            this.m_ShadowMap.setTextureUnit(gl.TEXTURE0, this.m_ShadowMapLoc);

            // fournir la matrice d'ombre
            mat4.glUniformMatrix(this.m_ShadowMatrixLoc, this.m_ShadowMatrix);

            // position de la lampe
            vec3.glUniform(this.m_LightPositionLoc, this.m_PositionCamera);
        }
    }


    /**
     * Cette méthode désactive le matériau
     */
    disable()
    {
        // désactiver la texture associée à la shadow map
        if (this.m_ShadowMap != null) {
            this.m_ShadowMap.setTextureUnit(gl.TEXTURE0);
        }

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
