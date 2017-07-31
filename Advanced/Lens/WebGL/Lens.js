// Cette classe permet d'appliquer une déformation type loupe sur un FBO

// superclasses et classes nécessaires
Requires("libs/Process/Process.js");


class Lens extends Process
{
    /**
     * Constructeur
     */
    constructor()
    {
        super("Lens");

        // rapport largeur/hauteur de la fenêtre
        this.m_Ratio = 1.0;

        // compiler le shader
        this.compileShader();
    }


    /**
     * met à jour le rapport largeur/hauteur de la fenêtre
     * @param ratio largeur/hauteur
     */
    setRatio(ratio)
    {
        this.m_Ratio = ratio;
    }


    /**
     * retourne le source du Fragment Shader
     */
    getFragmentShader()
    {
        let srcFragmentShader = dedent
            `#version 300 es
            precision mediump float;
            uniform sampler2D ColorMap;
            in vec2 frgTexCoord;

            // centre de la loupe
            const vec2 center = vec2(0.5, 0.5);
            const float PI = 3.14159;

            // fourni par m_Ratio
            uniform float ratio;

            // force de l'effet
            uniform float strength;

            out vec4 glFragColor;

            void main()
            {
                // coordonnées par rapport au centre
                vec2 texCoords = frgTexCoord - center;

                // compensation de l'image non carrée
                vec2 xyratio = vec2(1.0, ratio);
                texCoords /= xyratio;

                // coordonnées polaires par rapport au centre
                float r = length(texCoords);
                float angle = atan(texCoords.y, texCoords.x);

                // modifier le rayon
                r = pow(r, strength+1.0);

                // retourner en coordonnées cartésiennes
                texCoords = r*vec2(cos(angle), sin(angle))*xyratio + center;

                // aller chercher le texel situé à ces coordonnées modifiées
                vec3 color = texture(ColorMap, texCoords).rgb;
                glFragColor = vec4(color, 1.0);
            }`;
        return srcFragmentShader;
    }


    /**
     * détermine où sont les variables uniform spécifiques de ce traitement
     */
    findUniformLocations()
    {
        // appeler la méthode de la superclasse
        super.findUniformLocations();

        // déterminer où sont les variables uniform spécifiques
        this.m_StrengthLoc = gl.getUniformLocation(this.m_ShaderId, "strength");
        this.m_RatioLoc       = gl.getUniformLocation(this.m_ShaderId, "ratio");
    }


    /**
     * applique une distorsion en forme de loupe à l'image du FBO
     * @param fbo : FBO contenant l'image à traiter
     * @param strength : ampleur de l'effet, aucun pour 0.0
     */
    process(fbo, strength=1.0)
    {
        // préparer le shader pour le traitement
        this.startProcess();

        // fournir le color buffer du FBO à traiter
        this.setTextureUnit(gl.TEXTURE0, this.m_ColorMapLoc, fbo.getColorBuffer());

        // fournir les paramètres du traitement
        gl.uniform1f(this.m_StrengthLoc, strength);
        gl.uniform1f(this.m_RatioLoc,       this.m_Ratio);

        // dessiner un quadrilatère avec les quatre vertices
        gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);

        // désactiver les textures
        this.setTextureUnit(gl.TEXTURE0);

        // libérer les ressources
        this.endProcess();
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        super.destroy();
    }
}
