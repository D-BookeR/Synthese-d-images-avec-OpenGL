// Définition de la classe TextureMaterial, une spécialisation de Material
// Ce matériau permet de dessiner en mode Multiple Render Targets


// superclasses et classes nécessaires
Requires("libs/Material/Material.js");


class TextureMaterial extends Material
{
    /**
     * constructeur
     * @param texture du matériau
     */
    constructor(texture)
    {
        super("TextureMaterial");

        // texture
        this.m_Texture = texture;

        // compiler le shader
        this.compileShader();
    }


    /**
     * retourne le source du Vertex Shader
     */
    getVertexShader()
    {
        return dedent
            `#version 300 es

            // paramètres uniform
            uniform mat4 mat4ModelView;
            uniform mat4 mat4Projection;
            uniform mat3 mat3Normal;

            // attributs de sommets
            in vec3 glVertex;
            in vec3 glNormal;
            in vec2 glTexCoord;

            // interpolation vers les fragments
            out vec4 frgPosition;
            out vec3 frgNormal;
            out vec2 frgTexCoord;

            void main()
            {
                frgPosition = mat4ModelView * vec4(glVertex, 1.0);
                gl_Position = mat4Projection * frgPosition;
                frgNormal   = mat3Normal * glNormal;
                frgTexCoord = glTexCoord;
            }`;
    }


    /**
     * retourne le source du Fragment Shader
     */
    getFragmentShader()
    {
        let srcFragmentShader = "";
        srcFragmentShader += "#version 300 es\n";
        srcFragmentShader += "\n";
        if (this.m_ClipPlaneOn) {
            srcFragmentShader += "\n";
            srcFragmentShader += "// plan de coupe\n";
            srcFragmentShader += "uniform vec4 ClipPlane;\n";
        }
        srcFragmentShader += "// interpolation vers les fragments\n";
        srcFragmentShader += "precision mediump float;\n";
        srcFragmentShader += "in vec4 frgPosition;\n";
        srcFragmentShader += "in vec3 frgNormal;\n";
        srcFragmentShader += "in vec2 frgTexCoord;\n";
        srcFragmentShader += "\n";
        srcFragmentShader += "uniform sampler2D txColor;\n";
        srcFragmentShader += "\n";
        srcFragmentShader += "out vec4 glFragData[4];\n";
        srcFragmentShader += "\n";
        srcFragmentShader += "void main()\n";
        srcFragmentShader += "{\n";
        if (this.m_ClipPlaneOn) {
            srcFragmentShader += "    // plan de coupe\n";
            srcFragmentShader += "    if (dot(frgPosition, ClipPlane) < 0.0) discard;\n";
            srcFragmentShader += "\n";
        }
        srcFragmentShader += "    glFragData[0] = texture(txColor, frgTexCoord);\n";
        srcFragmentShader += "    glFragData[1] = vec4(0.0);\n";
        srcFragmentShader += "    glFragData[2] = vec4(frgPosition.xyz, 1.0);\n";
        srcFragmentShader += "    glFragData[3] = vec4(frgNormal, 0.0);\n";
        srcFragmentShader += "}\n";
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
        this.m_TextureLoc = gl.getUniformLocation(this.m_ShaderId, "txColor");
    }


    /**
     * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
     * @return le VBOset du matériau
     */
    createVBOset()
    {
        // créer le VBOset et spécifier les noms des attribute nécessaires à ce matériau
        let vboset = super.createVBOset();
        vboset.addAttribute(MeshVertex.ID_ATTR_NORMAL,   Utils.VEC3, "glNormal");
        vboset.addAttribute(MeshVertex.ID_ATTR_TEXCOORD, Utils.VEC2, "glTexCoord");
        return vboset;
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

        // activer la texture sur l'unité 0
        this.m_Texture.setTextureUnit(gl.TEXTURE0, this.m_TextureLoc);
    }


    /**
     * Cette méthode désactive le matériau
     */
    disable()
    {
        // désactiver les textures
        this.m_Texture.setTextureUnit(gl.TEXTURE0);

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
