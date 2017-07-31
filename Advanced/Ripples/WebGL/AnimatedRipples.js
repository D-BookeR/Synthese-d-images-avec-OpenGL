// Cette classe permet d'appliquer une déformation type tremblement sur un FBO
// voir https://github.com/SFML/SFML/wiki/Source:-HeatHazeShader

// superclasses et classes nécessaires
Requires("libs/Process/Process.js");


class AnimatedRipples extends Process
{
    /**
     * Constructeur
     */
    constructor()
    {
        super("AnimatedRipples");

        // charger la texture de distorsion
        this.m_Texture = new Texture2D("data/textures/distorsion.png", gl.LINEAR, gl.REPEAT);

        // compiler le shader
        this.compileShader();
    }


    /**
     * retourne le source du Fragment Shader
     */
    getFragmentShader()
    {
        return dedent
            `#version 300 es
            precision mediump float;
            uniform sampler2D ColorMap;
            uniform sampler2D DistorsionMap;

            uniform float time;
            uniform float strength;
            uniform float speed;

            in vec2 frgTexCoord;

            out vec4 glFragColor;

            void main()
            {
                // coordonnées dans la texture de distorsion
                vec2 texCoords = frgTexCoord + fract(vec2(0.0, time * speed));

                // accès à la texture, elle retourne un décalage
                vec2 offset = texture(DistorsionMap, texCoords).xy * 2.0 - 1.0;

                // conditionnement du décalage
                offset *= strength;

                // coordonnées dans l'image à déformer
                texCoords = frgTexCoord + offset;

                // accès à l'image déformée
                glFragColor = texture(ColorMap, texCoords);
            }`;
    }


    /**
     * détermine où sont les variables uniform spécifiques de ce traitement
     */
    findUniformLocations()
    {
        // appeler la méthode de la superclasse
        super.findUniformLocations();

        // déterminer où sont les variables uniform spécifiques
        this.m_DistorsionMapLoc = gl.getUniformLocation(this.m_ShaderId, "DistorsionMap");
        this.m_StrengthLoc      = gl.getUniformLocation(this.m_ShaderId, "strength");
        this.m_SpeedLoc         = gl.getUniformLocation(this.m_ShaderId, "speed");
        this.m_TimeLoc          = gl.getUniformLocation(this.m_ShaderId, "time");
    }


    /**
     * applique une distorsion à l'image du FBO
     * @param fbo : FBO contenant l'image à traiter
     * @param strength : importance de l'effet
     * @param speed : vitesse de l'effet
     */
    process(fbo, strength=0.003, speed=1.0)
    {
        // préparer le shader pour le traitement
        this.startProcess();

        // fournir le color buffer du FBO à traiter
        this.setTextureUnit(gl.TEXTURE0, this.m_ColorMapLoc, fbo.getColorBuffer());
        this.m_Texture.setTextureUnit(gl.TEXTURE1, this.m_DistorsionMapLoc);

        // fournir les paramètres du traitement
        gl.uniform1f(this.m_StrengthLoc, strength);
        gl.uniform1f(this.m_SpeedLoc,    speed);
        gl.uniform1f(this.m_TimeLoc,     Utils.Time);

        // dessiner un quadrilatère avec les quatre vertices
        gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);

        // désactiver les textures
        this.setTextureUnit(gl.TEXTURE0);
        this.setTextureUnit(gl.TEXTURE1);

        // libérer les ressources
        this.endProcess();
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Texture.destroy();
        super.destroy();
    }
}
