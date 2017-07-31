// Cette classe permet d'appliquer un flou gaussien sur un FBO
// voir https://github.com/mattdesl/lwjgl-basics/wiki/ShaderLesson5
// voir http://xissburg.com/faster-gaussian-blur-in-glsl/
// voir https://software.intel.com/en-us/blogs/2014/07/15/an-investigation-of-fast-real-time-gpu-based-image-blur-algorithms


// superclasses et classes nécessaires
Requires("libs/Process/FrameBufferObject.js");
Requires("libs/Process/Process.js");


class GaussianBlur extends Process
{
    /**
     * constructeur
     * @param width : largeur du FBO utilisé pour calculer le flou
     * @param height : hauteur du FBO utilisé pour calculer le flou
     */
    constructor(width, height)
    {
        super("GaussianBlur");

        // créer un FBO pour le résultat intermédiaire
        this.m_FBO = new FrameBufferObject(width, height, gl.TEXTURE_2D, gl.NONE);

        // initialisations
        this.m_HorizShaderId = null;
        this.m_VertiShaderId = null;

        // il y a deux shaders : un pour flouter horizontalement, l'autre verticalement
        this.compileHorizShader();
        this.compileVertiShader();
    }


    /** destructeur */
    destroy()
    {
        this.m_FBO.destroy();
        Utils.deleteShaderProgram(this.m_HorizShaderId);
        Utils.deleteShaderProgram(this.m_VertiShaderId);

        super.destroy();
    }


    /**
     * retourne le source du Vertex Shader horizontal
     */
    getHorizVertexShader()
    {
        let srcVertexShader = dedent
            `#version 300 es
            in vec2 glVertex;
            in vec2 glTexCoord;
            uniform float TexelSize;
            out vec2 frgTexCoord;
            out vec2 frgBlurTexCoord[14];
            void main()
            {
                gl_Position = vec4(glVertex, 0.0, 1.0);
                frgTexCoord = glTexCoord;
                frgBlurTexCoord[ 0] = glTexCoord + vec2(-7.0*TexelSize, 0.0);
                frgBlurTexCoord[ 1] = glTexCoord + vec2(-6.0*TexelSize, 0.0);
                frgBlurTexCoord[ 2] = glTexCoord + vec2(-5.0*TexelSize, 0.0);
                frgBlurTexCoord[ 3] = glTexCoord + vec2(-4.0*TexelSize, 0.0);
                frgBlurTexCoord[ 4] = glTexCoord + vec2(-3.0*TexelSize, 0.0);
                frgBlurTexCoord[ 5] = glTexCoord + vec2(-2.0*TexelSize, 0.0);
                frgBlurTexCoord[ 6] = glTexCoord + vec2(-1.0*TexelSize, 0.0);
                frgBlurTexCoord[ 7] = glTexCoord + vec2( 1.0*TexelSize, 0.0);
                frgBlurTexCoord[ 8] = glTexCoord + vec2( 2.0*TexelSize, 0.0);
                frgBlurTexCoord[ 9] = glTexCoord + vec2( 3.0*TexelSize, 0.0);
                frgBlurTexCoord[10] = glTexCoord + vec2( 4.0*TexelSize, 0.0);
                frgBlurTexCoord[11] = glTexCoord + vec2( 5.0*TexelSize, 0.0);
                frgBlurTexCoord[12] = glTexCoord + vec2( 6.0*TexelSize, 0.0);
                frgBlurTexCoord[13] = glTexCoord + vec2( 7.0*TexelSize, 0.0);
            }`;
        return srcVertexShader;
    }


    /**
     * retourne le source du Vertex Shader vertical
     */
    getVertiVertexShader()
    {
        let srcVertexShader = dedent
            `#version 300 es
            in vec2 glVertex;
            in vec2 glTexCoord;
            uniform float TexelSize;
            out vec2 frgTexCoord;
            out vec2 frgBlurTexCoord[14];
            void main()
            {
                gl_Position = vec4(glVertex, 0.0, 1.0);
                frgTexCoord = glTexCoord;
                frgBlurTexCoord[ 0] = glTexCoord + vec2(0.0, -7.0*TexelSize);
                frgBlurTexCoord[ 1] = glTexCoord + vec2(0.0, -6.0*TexelSize);
                frgBlurTexCoord[ 2] = glTexCoord + vec2(0.0, -5.0*TexelSize);
                frgBlurTexCoord[ 3] = glTexCoord + vec2(0.0, -4.0*TexelSize);
                frgBlurTexCoord[ 4] = glTexCoord + vec2(0.0, -3.0*TexelSize);
                frgBlurTexCoord[ 5] = glTexCoord + vec2(0.0, -2.0*TexelSize);
                frgBlurTexCoord[ 6] = glTexCoord + vec2(0.0, -1.0*TexelSize);
                frgBlurTexCoord[ 7] = glTexCoord + vec2(0.0,  1.0*TexelSize);
                frgBlurTexCoord[ 8] = glTexCoord + vec2(0.0,  2.0*TexelSize);
                frgBlurTexCoord[ 9] = glTexCoord + vec2(0.0,  3.0*TexelSize);
                frgBlurTexCoord[10] = glTexCoord + vec2(0.0,  4.0*TexelSize);
                frgBlurTexCoord[11] = glTexCoord + vec2(0.0,  5.0*TexelSize);
                frgBlurTexCoord[12] = glTexCoord + vec2(0.0,  6.0*TexelSize);
                frgBlurTexCoord[13] = glTexCoord + vec2(0.0,  7.0*TexelSize);
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
            uniform sampler2D ColorMap;
            in vec2 frgTexCoord;
            in vec2 frgBlurTexCoord[14];
            out vec4 glFragColor;
            void main()
            {
                vec4 sum = vec4(0.0);
                sum += texture(ColorMap, frgBlurTexCoord[ 0])*0.0044299121055113265;
                sum += texture(ColorMap, frgBlurTexCoord[ 1])*0.00895781211794;
                sum += texture(ColorMap, frgBlurTexCoord[ 2])*0.0215963866053;
                sum += texture(ColorMap, frgBlurTexCoord[ 3])*0.0443683338718;
                sum += texture(ColorMap, frgBlurTexCoord[ 4])*0.0776744219933;
                sum += texture(ColorMap, frgBlurTexCoord[ 5])*0.115876621105;
                sum += texture(ColorMap, frgBlurTexCoord[ 6])*0.147308056121;
                sum += texture(ColorMap, frgTexCoord        )*0.159576912161;
                sum += texture(ColorMap, frgBlurTexCoord[ 7])*0.147308056121;
                sum += texture(ColorMap, frgBlurTexCoord[ 8])*0.115876621105;
                sum += texture(ColorMap, frgBlurTexCoord[ 9])*0.0776744219933;
                sum += texture(ColorMap, frgBlurTexCoord[10])*0.0443683338718;
                sum += texture(ColorMap, frgBlurTexCoord[11])*0.0215963866053;
                sum += texture(ColorMap, frgBlurTexCoord[12])*0.00895781211794;
                sum += texture(ColorMap, frgBlurTexCoord[13])*0.0044299121055113265;

                glFragColor = sum;
            }`;
        return srcFragmentShader;
    }


    /**
     * compile ou recompile le shader horizontal
     */
    compileHorizShader()
    {
        // supprimer l'ancien shader s'il y en avait un
        if (this.m_HorizShaderId != null) Utils.deleteShaderProgram(this.m_HorizShaderId);

        // compiler le shader de floutage horizontal
        let srcVertexShader = this.getHorizVertexShader();
        let srcFragmentShader = this.getFragmentShader();
        this.m_HorizShaderId = Utils.makeShaderProgram(srcVertexShader, srcFragmentShader, "HorizBlur");

        // déterminer où sont les variables attribute
        this.m_HorizVertexLoc   = gl.getAttribLocation(this.m_HorizShaderId, "glVertex");
        this.m_HorizTexCoordLoc = gl.getAttribLocation(this.m_HorizShaderId, "glTexCoord");

        // déterminer où sont les variables uniform
        this.m_HorizColorMapLoc  = gl.getUniformLocation(this.m_HorizShaderId, "ColorMap");
        this.m_HorizTexelSizeLoc = gl.getUniformLocation(this.m_HorizShaderId, "TexelSize");
    }


    /**
     * compile ou recompile le shader vertical
     */
    compileVertiShader()
    {
        // supprimer l'ancien shader s'il y en avait un
        if (this.m_VertiShaderId != null) Utils.deleteShaderProgram(this.m_VertiShaderId);

        // compiler le shader de floutage vertical
        let srcVertexShader = this.getVertiVertexShader();
        let srcFragmentShader = this.getFragmentShader();
        this.m_VertiShaderId = Utils.makeShaderProgram(srcVertexShader, srcFragmentShader, "VertiBlur");

        // déterminer où sont les variables attribute
        this.m_VertiVertexLoc   = gl.getAttribLocation(this.m_VertiShaderId, "glVertex");
        this.m_VertiTexCoordLoc = gl.getAttribLocation(this.m_VertiShaderId, "glTexCoord");

        // déterminer où sont les variables uniform
        this.m_VertiColorMapLoc  = gl.getUniformLocation(this.m_VertiShaderId, "ColorMap");
        this.m_VertiTexelSizeLoc = gl.getUniformLocation(this.m_VertiShaderId, "TexelSize");
    }


    /**
     * applique le traitement sur l'image fournie dans le FBO
     * @param fbo : FBO contenant l'image à flouter
     * @param radius : rayon du flou
     */
    process(fbo, radius)
    {
        // Première étape : flouter horizontalement dans un FBO intermédiaire
        this.m_FBO.enable();

        // désactiver le test du depth buffer
        gl.disable(gl.DEPTH_TEST);

        // activer le shader horizontal
        gl.useProgram(this.m_HorizShaderId);

        // activer et lier le buffer contenant les coordonnées
        gl.enableVertexAttribArray(this.m_HorizVertexLoc);
        gl.bindBuffer(gl.ARRAY_BUFFER, Process.m_VertexBufferId);
        gl.vertexAttribPointer(this.m_HorizVertexLoc, 2, gl.FLOAT, gl.FALSE, 0, null);

        // activer et lier le buffer contenant les coordonnées de texture
        gl.enableVertexAttribArray(this.m_HorizTexCoordLoc);
        gl.bindBuffer(gl.ARRAY_BUFFER, Process.m_TexCoordBufferId);
        gl.vertexAttribPointer(this.m_HorizTexCoordLoc, 2, gl.FLOAT, gl.FALSE, 0, null);

        // fournir le color buffer
        this.setTextureUnit(gl.TEXTURE0, this.m_HorizColorMapLoc, fbo.getColorBuffer());

        // fournir les paramètres du flou
        gl.uniform1f(this.m_HorizTexelSizeLoc, radius/fbo.getWidth());

        // dessiner un quadrilatère avec les quatre vertices
        gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);

        // désactiver les textures
        this.setTextureUnit(gl.TEXTURE0);

        // désactiver le buffer
        gl.bindBuffer(gl.ARRAY_BUFFER, null);
        gl.disableVertexAttribArray(this.m_HorizVertexLoc);
        gl.disableVertexAttribArray(this.m_HorizTexCoordLoc);

        // termine les dessins dans le FBO intermédiaire et remet le précédent FBO
        this.m_FBO.disable();

        // Deuxième phase : flouter verticalement le précédent FBO

        // activer le shader vertical
        gl.useProgram(this.m_VertiShaderId);

        // activer et lier le buffer contenant les coordonnées
        gl.enableVertexAttribArray(this.m_VertiVertexLoc);
        gl.bindBuffer(gl.ARRAY_BUFFER, Process.m_VertexBufferId);
        gl.vertexAttribPointer(this.m_VertiVertexLoc, 2, gl.FLOAT, gl.FALSE, 0, null);

        // activer et lier le buffer contenant les coordonnées de texture
        gl.enableVertexAttribArray(this.m_VertiTexCoordLoc);
        gl.bindBuffer(gl.ARRAY_BUFFER, Process.m_TexCoordBufferId);
        gl.vertexAttribPointer(this.m_VertiTexCoordLoc, 2, gl.FLOAT, gl.FALSE, 0, null);

        // fournir le color buffer du FBO intermédiaire
        this.setTextureUnit(gl.TEXTURE0, this.m_VertiColorMapLoc, this.m_FBO.getColorBuffer());

        // fournir les paramètres du flou
        gl.uniform1f(this.m_VertiTexelSizeLoc, radius/fbo.getHeight());

        // dessiner un quadrilatère avec les quatre vertices
        gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);

        // désactiver les textures
        this.setTextureUnit(gl.TEXTURE0);

        // désactiver le buffer
        gl.bindBuffer(gl.ARRAY_BUFFER, null);
        gl.disableVertexAttribArray(this.m_VertiVertexLoc);
        gl.disableVertexAttribArray(this.m_VertiTexCoordLoc);

        // désactiver le shader
        gl.useProgram(null);

        // réactiver le test du depth buffer
        gl.enable(gl.DEPTH_TEST);
    }
}
