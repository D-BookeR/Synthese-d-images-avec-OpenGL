/**
 * Définition de la classe Light, une base pour construire des éclairages
 */

Requires("lib/Process/Process.js");


class Light extends Process
{
    /**
     * constructeur
     * C'est une lampe ambiante toute simple, sans ombre
     */
    constructor()
    {
        super("Light");

        // initialisation des variables membre spécifiques
        this.m_Color = vec3.fromValues(1,1,1);

        // compiler le shader
        this.compileShader();
    }


    /** destructeur */
    destroy()
    {
        super.destroy();
    }


    /**
     * Cette méthode définit la couleur de la lampe
     * @param color : vec3 donnant la couleur (intensité) de la lampe
     */
    setColor(color)
    {
        vec3.copy(this.m_Color, color);
    }


    /**
     * retourne la couleur de la lampe
     */
    getColor()
    {
        return this.m_Color;
    }


    /**
     * indique si cette lampe a une position (true) ou est ambiante (false)
     * @return false pour une lampe ambiante
     */
    hasPosition()
    {
        return false;
    }


    /**
     * retourne la position de la lampe dans le repère de la caméra
     */
    getPositionCamera()
    {
        return vec4.create();
    }


    /**
     * applique une matrice sur la position et la cible de la lampe
     * @param matrix : mat4
     */
    transform(matrix)
    {
    }


    /**
     * dessine la scène dans la shadowmap de cette lampe
     * NB : Cette méthode ne fait rien dans le cas d'une lampe abstraite
     * @param scene à dessiner (on va appeler sa méthode onDraw, vue de la lampe)
     * @param mat4ViewScene : matrice de transformation dans laquelle sont dessinés les objets
     */
    makeShadowMap(scene, mat4ViewScene)
    {
    }


    /**
     * "dessine" cette lampe dans le cas où elle est un SceneElement d'une scène
     * @param mat4Projection : matrice de projection
     * @param mat4View : matrice de vue
     */
    onDraw(mat4Projection, mat4View)
    {
    }


    /**
     * construit le Fragment Shader qui calcule l'éclairement de cette lampe
     */
    getFragmentShader()
    {
        let srcFragmentShader = dedent
            `#version 300 es
            precision mediump float;
            in vec2 frgTexCoord;

            uniform sampler2D MapDiffuse;
            uniform sampler2D MapPosition;
            uniform sampler2D MapDepth;
            uniform vec3 LightColor;

            out vec4 glFragColor;

            void main()
            {
                vec4 position = texture(MapPosition, frgTexCoord);
                if (position.w != 1.0) discard;
                gl_FragDepth = texture(MapDepth, frgTexCoord).r;
                vec4 color   = texture(MapDiffuse, frgTexCoord);
                glFragColor  = vec4(color.rgb * LightColor, 1.0) * color.a;
            }`;
        return srcFragmentShader;
    }


    /**
     * détermine où sont les variables uniform spécifiques de cette lampe
     */
    findUniformLocations()
    {
        // appeler la méthode de la superclasse
        super.findUniformLocations();

        // textures du FBO MRT (toutes ne sont pas forcément utilisées, c'est selon la lampe)
        this.m_MapDiffuseLoc  = gl.getUniformLocation(this.m_ShaderId, "MapDiffuse");
        this.m_MapSpecularLoc = gl.getUniformLocation(this.m_ShaderId, "MapSpecular");
        this.m_MapPositionLoc = gl.getUniformLocation(this.m_ShaderId, "MapPosition");
        this.m_MapNormalLoc   = gl.getUniformLocation(this.m_ShaderId, "MapNormale");
        this.m_MapDepthLoc    = gl.getUniformLocation(this.m_ShaderId, "MapDepth");

        // emplacement de la couleur de la lampe
        this.m_LightColorLoc = gl.getUniformLocation(this.m_ShaderId, "LightColor");
    }


    /**
     * active le shader, les VBO et les textures pour appliquer l'éclairement défini par la lampe
     * @param gbuffer : FBO MRT contenant toutes les informations de la scène à éclairer
     */
    startProcess(gbuffer)
    {
        // appeler la méthode de la superclasse
        super.startProcess();

        // fournir la couleur de la lampe
        vec3.glUniform(this.m_LightColorLoc, this.m_Color);

        // fournir les buffers du FBO MRT
        this.setTextureUnit(gl.TEXTURE0, this.m_MapDiffuseLoc,  gbuffer.getColorBuffer(0));
        this.setTextureUnit(gl.TEXTURE1, this.m_MapSpecularLoc, gbuffer.getColorBuffer(1));
        this.setTextureUnit(gl.TEXTURE2, this.m_MapPositionLoc, gbuffer.getColorBuffer(2));
        this.setTextureUnit(gl.TEXTURE3, this.m_MapNormalLoc,   gbuffer.getColorBuffer(3));
        this.setTextureUnit(gl.TEXTURE4, this.m_MapDepthLoc,    gbuffer.getDepthBuffer());
    }


    /**
     * applique l'éclairement défini par cette lampe
     * @param gbuffer : FBO MRT contenant toutes les informations de la scène à éclairer
     */
    process(gbuffer)
    {
        // préparer le shader pour le traitement
        this.startProcess(gbuffer);

        // mettre le depth test en mode <=
        let depth_func = gl.getParameter(gl.DEPTH_FUNC);
        gl.depthFunc(gl.LEQUAL);

        // dessiner un quadrilatère avec les quatre vertices
        gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);

        // remettre comme c'était
        gl.depthFunc(depth_func);

        // libérer les ressources
        this.endProcess();
    }


    /**
     * désactive shader, VBO et textures
     */
    endProcess()
    {
        // désactiver les textures
        this.setTextureUnit(gl.TEXTURE0);
        this.setTextureUnit(gl.TEXTURE1);
        this.setTextureUnit(gl.TEXTURE2);
        this.setTextureUnit(gl.TEXTURE3);
        this.setTextureUnit(gl.TEXTURE4);

        // appeler la méthode de la superclasse
        super.endProcess();
    }
}
