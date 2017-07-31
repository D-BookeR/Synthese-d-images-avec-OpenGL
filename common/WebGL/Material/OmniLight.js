/**
 * Définition de la classe OmniLight, une base pour construire des éclairages diffus et localisés
 */

// superclasses et classes nécessaires
Requires("libs/Material/Light.js");


class OmniLight extends Light
{
    /**
     * constructeur
     * C'est une lampe toute simple, positionnelle ou directionnelle, sans ombre
     */
    constructor()
    {
        super();
        this.m_Name = "OmniLight";

        // initialisation des variables membre spécifiques
        this.m_PositionScene = vec4.fromValues(0,0,1,0);
        this.m_PositionCamera = vec4.clone(this.m_PositionScene);

        // compiler le shader
        this.compileShader();
    }


    /** destructeur */
    destroy()
    {
        super.destroy();
    }


    /**
     * indique si cette lampe a une position (true) ou est ambiante (false)
     * @return false pour une lampe ambiante
     */
    hasPosition()
    {
        return true;
    }


    /**
     * Cette méthode définit la position de la lampe
     * @param position : vec4 indiquant où se trouve la lampe
     */
    setPosition(position)
    {
        vec4.copy(this.m_PositionScene, position);
        vec4.copy(this.m_PositionCamera, position);
    }


    /**
     * retourne la position de la lampe dans le repère de la scène
     * @return position
     */
    getPosition()
    {
        return this.m_PositionScene;
    }


    /**
     * applique une matrice sur la position de la lampe afin d'obtenir la position caméra
     * @param matrix : mat4
     */
    transformPosition(matrix)
    {
        // calculer les coordonnées dans le repère caméra
        vec4.transformMat4(this.m_PositionCamera, this.m_PositionScene, matrix);
    }


    /**
     * Cette méthode définit la position visée par la lampe
     * NB : Cette méthode ne fait rien dans le cas d'une lampe omnidirectionnelle
     * @param target : vec4 indiquant le point ou l'axe que vise la lampe
     * NB : Cette méthode ne fait rien dans le cas d'une lampe omnidirectionnelle
     * NB : l'une des deux informations position et cible doivent être des points
     */
    setTarget(target)
    {
    }


    /**
     * applique une matrice sur la cible de la lampe afin d'obtenir la position caméra
     * NB : cette méthode ne fait rien dans le cas d'une lampe omnidirectionnelle
     * @param matrix : mat4
     */
    transformTarget(matrix)
    {
    }


    /**
     * applique une matrice sur la position et la cible de la lampe
     * @param matrix : mat4
     */
    transform(matrix)
    {
        this.transformPosition(matrix);
        this.transformTarget(matrix);
    }


    /**
     * retourne la position de la lampe dans le repère caméra
     */
    getPositionCamera()
    {
        return this.m_PositionCamera;
    }


    /**
     * construit le Fragment Shader qui calcule l'éclairement de cette lampe
     * @return source du shader
     */
    getFragmentShader()
    {
        return dedent
            `#version 300 es
            precision mediump float;
            in vec2 frgTexCoord;

            uniform sampler2D MapPosition;
            uniform sampler2D MapNormale;
            uniform sampler2D MapDiffuse;
            uniform sampler2D MapSpecular;
            uniform sampler2D MapDepth;
            uniform vec3 LightColor;
            uniform vec4 LightPosition;

            out vec4 glFragColor;

            void main()
            {
                // récupérer les infos du g-buffer
                vec4 position = texture(MapPosition, frgTexCoord);
                if (position.w != 1.0) discard;
                gl_FragDepth = texture(MapDepth, frgTexCoord).r;
                vec4 normal  = texture(MapNormale, frgTexCoord);
                vec4 Kd = texture(MapDiffuse, frgTexCoord);

                if (normal.w != 0.0) {
                    // éclairement diffus uniquement
                    glFragColor = vec4(LightColor * Kd.rgb, 1.0) * Kd.a;
                } else {
                    // calculer la direction et l'intensité de la lampe
                    vec3 L;
                    vec3 color;
                    if (LightPosition.w != 0.0) {
                        // positionnelle
                        L = LightPosition.xyz - position.xyz;
                        float distance = length(L);
                        // diviser la couleur par la distance au carré
                        color = LightColor / (distance*distance);
                        L = L / distance;
                    } else {
                        // directionnelle
                        L = normalize(LightPosition.xyz);
                        color = LightColor;
                    }
                    // direction de la normale et produit scalaire
                    vec3 N = normal.xyz;
                    float dotNL = clamp(dot(N,L), 0.0, 1.0);

                    // couleur spéculaire et coefficient ns
                    vec4 Ks = texture(MapSpecular, frgTexCoord);
                    float ns = Ks.a;
                    if (ns > 0.0) {
                        // reflet spéculaire
                        vec3 R = reflect(normalize(position.xyz), N);
                        float dotRL = clamp(dot(R,L), 0.0, 1.0);
                        // éclairement diffus et reflet spéculaire
                        glFragColor = vec4(color * (Kd.rgb*dotNL + Ks.rgb*pow(dotRL, ns)), 1.0) * Kd.a;
                    } else {
                        // éclairement diffus sans reflet spéculaire
                        glFragColor = vec4(color * (Kd.rgb*dotNL), 1.0) * Kd.a;
                    }
                }
            }`;
    }


    /**
     * détermine où sont les variables uniform spécifiques de cette lampe
     */
    findUniformLocations()
    {
        // obtenir les emplacements de la superclasse
        super.findUniformLocations();

        // emplacement de la position de la lampe
        this.m_LightPositionLoc = gl.getUniformLocation(this.m_ShaderId, "LightPosition");
    }


    /**
     * applique l'éclairement défini par cette lampe
     * @param gbuffer : FBO MRT contenant toutes les informations de la scène à éclairer
     */
    startProcess(gbuffer)
    {
        // préparer le shader pour le traitement
        super.startProcess(gbuffer);

        // fournir la position de la lampe (la couleur est fournie par la superclasse)
        vec4.glUniform(this.m_LightPositionLoc, this.m_PositionCamera);
    }
}
