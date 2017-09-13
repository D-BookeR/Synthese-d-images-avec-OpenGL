// Cette classe permet d'appliquer un traitement sur une image dans un FBO

Requires("libs/Process/FrameBufferObject.js");


class Process
{
    /**
     * crée les shaders et VBO de dessin
     */
    static staticinit()
    {
        // créer et remplir les VBOs des coordonnées
        let vertices =  [ -1,-1,  +1,-1,  +1,+1,  -1,+1 ];
        Process.m_VertexBufferId = Utils.makeFloatVBO(vertices, gl.ARRAY_BUFFER, gl.STATIC_DRAW);
        let texcoords = [  0, 0,   1, 0,   1, 1,   0, 1 ];
        Process.m_TexCoordBufferId = Utils.makeFloatVBO(texcoords, gl.ARRAY_BUFFER, gl.STATIC_DRAW);
    }


    /**
     * supprime les shaders et VBO de dessin
     */
    static staticdestroy()
    {
        Utils.deleteVBO(Process.m_VertexBufferId);
        Utils.deleteVBO(Process.m_TexCoordBufferId);
    }


    /**
     * constructeur
     * @param nom du traitement
     */
    constructor(nom)
    {
        // initialisation
        this.m_Name = nom;
        this.m_ShaderId = null;

        // initialisation statique ?
        if (typeof Process.m_VertexBufferId === 'undefined') Process.staticinit();
    }


    /**
     * destructeur
     */
    destroy()
    {
        Utils.deleteShaderProgram(this.m_ShaderId);
    }


    /**
     * retourne le source du Vertex Shader commun à tous les traitements
     * NB : le Fragment Shader est spécifique à chaque traitement et défini dans la sous-classe
     */
    getVertexShader()
    {
        let srcVertexShader = dedent
            `#version 300 es
            in vec2 glVertex;
            in vec2 glTexCoord;
            out vec2 frgTexCoord;
            void main()
            {
                gl_Position = vec4(glVertex, 0.0, 1.0);
                frgTexCoord = glTexCoord;
            }`;
        return srcVertexShader;
    }


    /**
     * retourne le source du Vertex Shader commun à tous les traitements
     * @note le Fragment Shader est spécifique à chaque traitement et défini dans la sous-classe
     */
    getFragmentShader()
    {
        throw "Method Process.getFragmentShader should be overloaded by subclass";
    }


    /**
     * compile ou recompile le shader
     */
    compileShader()
    {
        // supprimer l'ancien shader s'il y en avait un
        if (this.m_ShaderId != null) Utils.deleteShaderProgram(this.m_ShaderId);

        // compiler le shader
        let srcVertexShader = this.getVertexShader();
        let srcFragmentShader = this.getFragmentShader();
        this.m_ShaderId = Utils.makeShaderProgram(srcVertexShader, srcFragmentShader, this.m_Name);

        // déterminer où sont les variables attribute
        this.m_VertexLoc   = gl.getAttribLocation(this.m_ShaderId, "glVertex");
        this.m_TexCoordLoc = gl.getAttribLocation(this.m_ShaderId, "glTexCoord");

        // déterminer où sont les variables uniform
        this.findUniformLocations();
    }


    /**
     * détermine où sont les variables uniform du traitement
     */
    findUniformLocations()
    {
        this.m_ColorMapLoc = gl.getUniformLocation(this.m_ShaderId, "ColorMap");
    }


    /**
     * active le shader et les VBO pour appliquer le traitement défini dans le shader
     */
    startProcess()
    {
        // activer le shader
        gl.useProgram(this.m_ShaderId);

        // activer et lier le VBO contenant les coordonnées
        gl.enableVertexAttribArray(this.m_VertexLoc);
        gl.bindBuffer(gl.ARRAY_BUFFER, Process.m_VertexBufferId);
        gl.vertexAttribPointer(this.m_VertexLoc, Utils.VEC2, gl.FLOAT, gl.FALSE, 0, null);

        // activer et lier le VBO contenant les coordonnées de texture
        gl.enableVertexAttribArray(this.m_TexCoordLoc);
        gl.bindBuffer(gl.ARRAY_BUFFER, Process.m_TexCoordBufferId);
        gl.vertexAttribPointer(this.m_TexCoordLoc, Utils.VEC2, gl.FLOAT, gl.FALSE, 0, null);
    }


    /**
     * applique le traitement sur le FBO fourni
     * @param fbo FrameBufferObject à traiter
     */
    process(fbo)
    {
        throw "Method Process.process should be overloaded by subclass";
    }


    /**
     * désactive le shader et les VBO
     */
    endProcess()
    {
        // désactiver les VBOs
        gl.bindBuffer(gl.ARRAY_BUFFER, null);
        gl.disableVertexAttribArray(this.m_VertexLoc);
        gl.disableVertexAttribArray(this.m_TexCoordLoc);

        // désactiver le shader
        gl.useProgram(null);
    }


    /**
     * cette fonction associe l'une des textures de ce FBO à une unité pour un shader
     * NB : le shader concerné doit être actif
     * @param unit : unité de texture concernée, par exemple gl.TEXTURE0
     * @param locSampler : emplacement de la variable uniform sampler* de cette texture dans le shader ou null pour désactiver la texture
     * @param bufferId : m_ColorBuffer ou m_DepthBuffer ou null pour désactiver
     */
    setTextureUnit(unit, locSampler=null, bufferId=null)
    {
        gl.activeTexture(unit);
        if (locSampler == null || bufferId == null) {
            gl.bindTexture(gl.TEXTURE_2D, null);
        } else {
            gl.bindTexture(gl.TEXTURE_2D, bufferId);
            gl.uniform1i(locSampler, unit-gl.TEXTURE0);
        }
    }
}
