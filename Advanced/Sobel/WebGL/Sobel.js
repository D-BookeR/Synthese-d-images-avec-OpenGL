// Cette classe permet d'appliquer le filtre de Sobel sur un FBO


// superclasses et classes nécessaires
Requires("libs/Process/FrameBufferObject.js");
Requires("libs/Process/Process.js");


class Sobel extends Process
{
    /**
     * constructeur
     * @param width : largeur du FBO utilisé
     * @param height : hauteur du FBO utilisé
     */
    constructor(width, height)
    {
        super("Sobel");

        // créer deux FBO pour les résultats intermédiaires
        this.m_FBO = [
            new FrameBufferObject(width, height, gl.TEXTURE_2D, gl.NONE),
            new FrameBufferObject(width, height, gl.TEXTURE_2D, gl.NONE)
        ];

        // taille des pixels : inverse de la fenêtre
        let texelsize = vec2.fromValues(1.0/width, 1.0/height);

        // définir les tableaux
        this.m_ShaderId     = [ null, null, null, null ];
        this.m_VertexLoc    = [ null, null, null, null ];
        this.m_TexCoordLoc  = [ null, null, null, null ];
        this.m_ColorMapLoc  = [ null, null, null, null ];
        this.m_TexelSizeLoc = [ null, null, null, null ];

        // il y a quatre shaders : un pour chaque passe
        for (let pass=0; pass<4; pass++) {
            // compiler le shader correspondant à la passe
            this.compileShader(pass);

            // lui fournir la dimension de la fenêtre
            gl.useProgram(this.m_ShaderId[pass]);
            vec2.glUniform(this.m_TexelSizeLoc[pass], texelsize);
            gl.useProgram(null);
        }
    }


    /** destructeur */
    destroy()
    {
        this.m_FBO[0].destroy();
        this.m_FBO[1].destroy();

        for (let pass=0; pass<4; pass++) {
            Utils.deleteShaderProgram(this.m_ShaderId[pass]);
        }

        super.destroy();
    }


    /**
     * retourne le source du Vertex Shader correspondant à la passe
     * @param pass : code à 2 bits indiquant passe une horizontale pour 0 ou 1, verticale pour 2 ou 3
     */
    getVertexShader(pass)
    {
        if (pass < 2) return dedent
            `#version 300 es
            in vec2 glVertex;
            in vec2 glTexCoord;
            uniform vec2 TexelSize;
            out vec2 frgKernelTexCoord[3];
            void main()
            {
                gl_Position = vec4(glVertex, 0.0, 1.0);
                frgKernelTexCoord[0] = glTexCoord + vec2(-TexelSize.x, 0.0);
                frgKernelTexCoord[1] = glTexCoord + vec2( 0.0,         0.0);
                frgKernelTexCoord[2] = glTexCoord + vec2(+TexelSize.x, 0.0);
            }`;
        else return dedent
            `#version 300 es
            in vec2 glVertex;
            in vec2 glTexCoord;
            uniform vec2 TexelSize;
            out vec2 frgKernelTexCoord[3];
            void main()
            {
                gl_Position = vec4(glVertex, 0.0, 1.0);
                frgKernelTexCoord[0] = glTexCoord + vec2(0.0, -TexelSize.y);
                frgKernelTexCoord[1] = glTexCoord + vec2(0.0,  0.0);
                frgKernelTexCoord[2] = glTexCoord + vec2(0.0, +TexelSize.y);
            }`;
        return srcVertexShader;
    }


    /**
     * retourne le source du Fragment Shader correspondant à la passe
     * @param pass : convolution [1 2 1] pour 0 ou 2, [-1 0 +1] pour 1 ou 3
     */
    getFragmentShader(pass)
    {
        if (pass%2 == 0) return dedent
            `#version 300 es
            precision mediump float;
            uniform sampler2D ColorMap;
            in vec2 frgKernelTexCoord[3];
            out vec4 glFragColor;
            void main()
            {
                vec4 sum = vec4(0.0);
                sum += texture(ColorMap, frgKernelTexCoord[0]) * +1.0;
                sum += texture(ColorMap, frgKernelTexCoord[1]) * +2.0;
                sum += texture(ColorMap, frgKernelTexCoord[2]) * +1.0;

                glFragColor = sum;
            }`;
        else return dedent
            `#version 300 es
            precision mediump float;
            uniform sampler2D ColorMap;
            in vec2 frgKernelTexCoord[3];
            out vec4 glFragColor;
            void main()
            {
                vec4 sum = vec4(0.0);
                sum += texture(ColorMap, frgKernelTexCoord[0]) * -1.0;
                sum += texture(ColorMap, frgKernelTexCoord[2]) * +1.0;

                glFragColor = sum;
            }`;
    }


    /**
     * compile ou recompile le shader correspondant à la passe
     * @param pass : code à 2 bits de 0 à 3
     */
    compileShader(pass)
    {
        // supprimer l'ancien shader s'il y en avait un
        if (this.m_ShaderId[pass] != null) Utils.deleteShaderProgram(this.m_ShaderId[pass]);

        // compiler le shader
        let srcVertexShader = this.getVertexShader(pass);
        let srcFragmentShader = this.getFragmentShader(pass);
        this.m_ShaderId[pass] = Utils.makeShaderProgram(srcVertexShader, srcFragmentShader, "Sobel"+pass);

        // déterminer où sont les variables attribute
        this.m_VertexLoc[pass]   = gl.getAttribLocation(this.m_ShaderId[pass], "glVertex");
        this.m_TexCoordLoc[pass] = gl.getAttribLocation(this.m_ShaderId[pass], "glTexCoord");

        // déterminer où sont les variables uniform
        this.m_ColorMapLoc[pass]   = gl.getUniformLocation(this.m_ShaderId[pass], "ColorMap");
        this.m_TexelSizeLoc[pass] = gl.getUniformLocation(this.m_ShaderId[pass], "TexelSize");
    }


    /**
     * applique le traitement sur l'image fournie dans le FBO
     * @param fbo : FBO contenant l'image à traiter
     */
    process(fbo)
    {
        // désactiver le test du depth buffer
        gl.disable(gl.DEPTH_TEST);

        // 4 passages sur l'image, en alternant les FBO
        for (let pass=0; pass<4; pass++) {

            // dessiner dans un FBO intermédiaire sauf au dernier passage
            if (pass < 3) this.m_FBO[pass%2].enable();

            // activer le shader de cette passe
            gl.useProgram(this.m_ShaderId[pass]);

            // activer et lier le buffer contenant les coordonnées
            gl.enableVertexAttribArray(this.m_VertexLoc[pass]);
            gl.bindBuffer(gl.ARRAY_BUFFER, Process.m_VertexBufferId);
            gl.vertexAttribPointer(this.m_VertexLoc[pass], 2, gl.FLOAT, gl.FALSE, 0, null);

            // activer et lier le buffer contenant les coordonnées de texture
            gl.enableVertexAttribArray(this.m_TexCoordLoc[pass]);
            gl.bindBuffer(gl.ARRAY_BUFFER, Process.m_TexCoordBufferId);
            gl.vertexAttribPointer(this.m_TexCoordLoc[pass], 2, gl.FLOAT, gl.FALSE, 0, null);

            // fournir le color buffer selon la passe
            if (pass == 0) {
                // c'est le premier passage, on fournit le color buffer du fbo à traiter
                this.setTextureUnit(gl.TEXTURE0, this.m_ColorMapLoc[pass], fbo.getColorBuffer());
            } else {
                // fournir le FBO intermédiaire précédent
                this.setTextureUnit(gl.TEXTURE0, this.m_ColorMapLoc[pass], this.m_FBO[(pass-1)%2].getColorBuffer());
            }

            // dessiner un quadrilatère avec les quatre vertices
            gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);

            // désactiver les textures
            this.setTextureUnit(gl.TEXTURE0);

            // désactiver le buffer
            gl.bindBuffer(gl.ARRAY_BUFFER, null);
            gl.disableVertexAttribArray(this.m_VertexLoc[pass]);
            gl.disableVertexAttribArray(this.m_TexCoordLoc[pass]);

            // désactiver le shader
            gl.useProgram(null);

            // termine les dessins dans le FBO intermédiaire et remet le précédent FBO
            if (pass < 3) this.m_FBO[pass%2].disable();
        }

        // réactiver le test du depth buffer
        gl.enable(gl.DEPTH_TEST);
    }
}
