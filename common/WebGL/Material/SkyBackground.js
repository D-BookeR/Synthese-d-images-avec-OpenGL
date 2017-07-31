/**
 * Cette classe permet de dessiner l'arrière plan uniforme en mode Deferred Shading
 * Le shader colore les zones non dessinées du FBO mrt (la coordonnée w des fragments vaut 0)
 */

// superclasses et classes nécessaires
Requires("libs/Material/Light.js");


class SkyBackground extends Light
{
    constructor()
    {
        super();

        // récupérer la couleur d'effacement
        let color = gl.getParameter(gl.COLOR_CLEAR_VALUE);
        this.m_Color = vec3.fromValues(color[0], color[1], color[2]);

        // compiler le shader
        this.compileShader();
    }


    /** met à jour la couleur d'effacement */
    update()
    {
        let color = gl.getParameter(gl.COLOR_CLEAR_VALUE);
        vec3.set(this.m_Color, color[0], color[1], color[2]);
    }


    /** destructeur */
    destroy()
    {
        super.destroy();
    }


    /**
     * construit le Fragment Shader qui colore les zones non dessinées du FBO mrt
     */
    getFragmentShader()
    {
        return dedent
            `#version 300 es
            precision mediump float;
            uniform sampler2D MapPosition;
            in vec2 frgTexCoord;
            uniform vec3 LightColor;
            out vec4 glFragColor;

            void main()
            {
                vec4 position = texture(MapPosition, frgTexCoord);
                if (position.w > 0.8) discard;
                gl_FragDepth = 1.0;
                glFragColor = vec4(LightColor, 1.0);
            }`;
    }
}
