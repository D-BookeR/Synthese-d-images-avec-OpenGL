/**
 * Définition de la classe TransparentMaterial, une spécialisation de DeferredShadingMaterial
 * Ce matériau est destiné à dessiner en mode Multiple Render Targets
 */

// superclasses et classes nécessaires
Requires("libs/Material/DeferredShadingMaterial.js");


class TransparentMaterial extends Material
{
    /**
     * Constructeur de la classe TransparentMaterial.
     * @param Kd : un vec4(r,g,b,a) donnant la couleur diffuse et la transparence
     */
    constructor(Kd)
    {
        super("TransparentMaterial");

        // propriétés
        this.m_Kd = Kd;

        // identifiants des textures associées aux sampler2D DepthMap*
        this.m_DepthMapFar = null;
        this.m_DepthMapNear = null;

        // dimensions de la fenêtre pour normaliser gl_FragCoord.xy
        this.m_WindowSize = vec2.fromValues(1.0, 1.0);

        // recompiler le shader
        this.compileShader();
    }


    /**
     * retourne le source du Fragment Shader
     */
    getFragmentShader()
    {
        let srcFragmentShader = dedent
            `#version 300 es

            // coordonnées du fragment
            precision mediump float;
            in vec4 frgPosition;

            // caractéristiques du matériau
            const vec4 Kd = ${vec4.str(this.m_Kd)};

            // depth maps à prendre en compte
            uniform sampler2D DepthMapFar;
            uniform sampler2D DepthMapNear;
            uniform vec2 WindowSize;

            // g-buffer
            out vec4 glFragData[4];

            void main()
            {
                // coordonnées dans les depth maps
                vec2 fragcoords = gl_FragCoord.xy/WindowSize;

                // comparaison avec la distance lointaine
                float depth = texture(DepthMapFar, fragcoords).x;
                if (gl_FragCoord.z >= depth) discard;

                // comparaison avec la distance proche
                depth = texture(DepthMapNear, fragcoords).x;
                if (gl_FragCoord.z <= depth) discard;

                // couleur finale = celle du matériau
                glFragData[0] = vec4(Kd.rgb * Kd.a, Kd.a); // prémultiplication par alpha
                glFragData[1] = vec4(0.0);
                glFragData[2] = vec4(frgPosition.xyz, 1.0);
                glFragData[3] = vec4(1.0, 1.0, 1.0, 1.0);
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

        // déterminer où sont les variables uniform spécifiques
        this.m_DepthMapFarLoc  = gl.getUniformLocation(this.m_ShaderId, "DepthMapFar");
        this.m_DepthMapNearLoc = gl.getUniformLocation(this.m_ShaderId, "DepthMapNear");
        this.m_WindowSizeLoc   = gl.getUniformLocation(this.m_ShaderId, "WindowSize");
    }


    /**
     * affecte la texture DepthMap du shader
     * @param far : depth buffer qui limite les dessins arrière de ce matériau
     * @param near : depth buffer qui limite les dessins avant de ce matériau
     */
    setDepthMaps(far, near)
    {
        this.m_DepthMapFar = far;
        this.m_DepthMapNear = near;
    }


    /**
     * spécifie la taille de la fenêtre
     * @param width : largeur en nombre de pixels de la fenêtre
     * @param height : hauteur en nombre de pixels de la fenêtre
     */
    setWindowDimensions(width, height)
    {
        this.m_WindowSize = vec2.fromValues(width, height);
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

        // fournir et activer la DepthMapFar dans l'unité 0
        gl.activeTexture(gl.TEXTURE0);
        gl.bindTexture(gl.TEXTURE_2D, this.m_DepthMapFar);
        gl.uniform1i(this.m_DepthMapFarLoc, gl.TEXTURE0-gl.TEXTURE0);

        // fournir et activer la DepthMapNear dans l'unité 1
        gl.activeTexture(gl.TEXTURE1);
        gl.bindTexture(gl.TEXTURE_2D, this.m_DepthMapNear);
        gl.uniform1i(this.m_DepthMapNearLoc, gl.TEXTURE1-gl.TEXTURE0);

        // fournir les dimensions de la fenêtre
        vec2.glUniform(this.m_WindowSizeLoc, this.m_WindowSize);
    }


    /**
     * Cette méthode désactive le matériau
     * @note le shader doit être activé
     */
    disable()
    {
        // désactiver les depth maps
        gl.activeTexture(gl.TEXTURE0);
        gl.bindTexture(gl.TEXTURE_2D, null);
        gl.activeTexture(gl.TEXTURE1);
        gl.bindTexture(gl.TEXTURE_2D, null);

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
