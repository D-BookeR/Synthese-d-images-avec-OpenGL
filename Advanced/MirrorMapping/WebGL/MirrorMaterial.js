// Définition de la classe MirrorMaterial

// superclasses et classes nécessaires
Requires("libs/Material/Material.js");
Requires("libs/Material/VBOset.js");


class MirrorMaterial extends Material
{
    /**
     * Cette fonction définit un matériau pour dessiner le maillage
     * @param texture : instance de TextureCube
     */
    constructor(texture)
    {
        super("MirrorMaterial");

        // mémoriser la texture
        this.m_Texture = texture;
        this.m_TextureLoc = -1;

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
            out vec3 frgDirection;
            uniform mat4 mat4ModelViewTeapot;
            uniform mat3 mat3NormalTeapot;
            uniform mat3 mat3NormalScene;
            uniform mat4 mat4Projection;
            void main()
            {
                gl_Position = mat4Projection * mat4ModelViewTeapot * vec4(glVertex, 1.0);
                vec3 Vue = vec3(0,0,-1);
                frgDirection = mat3NormalScene * reflect(Vue, mat3NormalTeapot * glNormal);
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
            in vec3 frgDirection;
            uniform samplerCube txCube;
            out vec4 glFragColor;
            void main()
            {
                //DEBUG glFragColor = vec4(frgDirection, 1);return;
                // couleur extraite de la texture cube
                glFragColor = texture(txCube, frgDirection);
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
        this.m_MatProjectionLoc       = gl.getUniformLocation(this.m_ShaderId, "mat4Projection");
        this.m_MatModelViewTeapotLoc = gl.getUniformLocation(this.m_ShaderId, "mat4ModelViewTeapot");
        this.m_MatNormalSceneLoc      = gl.getUniformLocation(this.m_ShaderId, "mat3NormalScene");
        this.m_MatNormalTeapotLoc    = gl.getUniformLocation(this.m_ShaderId, "mat3NormalTeapot");
        this.m_TextureLoc             = gl.getUniformLocation(this.m_ShaderId, "txCube");
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
     * Cette méthode active le matériau : met en place son shader,
     * fournit les variables uniform qu'il demande
     * @param mat4Projection : mat4 contenant la projection
     * @param mat4ModelViewTeapot : mat4 contenant la transformation vers la caméra
     */
    enable(mat4Projection, mat4ModelViewTeapot)
    {
        // activer le shader
        gl.useProgram(this.m_ShaderId);

        // fournir les matrices MV et P
        mat4.glUniformMatrix(this.m_MatModelViewTeapotLoc, mat4ModelViewTeapot);
        mat4.glUniformMatrix(this.m_MatProjectionLoc, mat4Projection);

        // calculer et fournir la matrice mat3NormalTheiere au shader
        let mat3NormalTeapot = mat3.create();
        mat3.fromMat4(mat3NormalTeapot, mat4ModelViewTeapot);
        mat3.transpose(mat3NormalTeapot, mat3NormalTeapot);
        mat3.invert(mat3NormalTeapot, mat3NormalTeapot);
        mat3.glUniformMatrix(this.m_MatNormalTeapotLoc, mat3NormalTeapot);

        // activer la texture sur l'unité 0
        this.m_Texture.setTextureUnit(gl.TEXTURE0, this.m_TextureLoc);
    }


    /**
     * indique au matériau quelle est la transformation de la scène afin
     * de viser le point correct dans la skybox qui se réfléchit sur le matériau
     * @param mat4ModelViewScene : matrice ModelView utilisée pour dessiner la skybox
     */
    setModelViewScene(mat4ModelViewScene)
    {
        // calculer la matrice mat3NormalScene
        let mat3NormalScene = mat3.create();
        mat3.fromMat4(mat3NormalScene, mat4ModelViewScene);
        mat3.transpose(mat3NormalScene, mat3NormalScene);
        // pas d'inversion car c'est une matrice pour aller dans le repère de la scène

        // fournir la matrice mat3NormalScene au shader
        gl.useProgram(this.m_ShaderId);
        mat3.glUniformMatrix(this.m_MatNormalSceneLoc, mat3NormalScene);
    }



    /**
     * Cette méthode désactive le matériau
     */
    disable()
    {
        // désactiver les textures
        this.m_Texture.setTextureUnit(gl.TEXTURE0, null);

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
