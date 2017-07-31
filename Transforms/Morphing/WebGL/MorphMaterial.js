// Définition de la classe MorphMaterial, une spécialisation de Material
// Ce matériau permet d'appliquer une déformation par cible
// C'est une simplification de DeferredShadingMaterial


// superclasses et classes nécessaires
Requires("libs/Material/Material.js");


class MorphMaterial extends Material
{
    /**
     * constructeur
     * @param diffuse : nom d'une texture
     * @param Ks : vec3
     * @param Ns : poli du matériau
     */
    constructor(diffuse, Ks, Ns)
    {
        super("MorphMaterial");

        // texture diffuse
        this.m_TxDiffuse = new Texture2D(diffuse, gl.LINEAR, gl.REPEAT);

        // couleur spéculaire
        this.m_Ks = Ks;
        this.m_Ns = Ns;

        // coefficient de morphing
        this.m_MorphCoef = 0.0;

        // compiler le shader
        this.compileShader();
    }


    /** destructeur */
    destroy()
    {
        // libérer les textures qui ont été chargées
        if (this.m_TxDiffuse != null) this.m_TxDiffuse.destroy();
        super.destroy();
    }


    /**
     * retourne le source du Vertex Shader
     */
    getVertexShader()
    {
        // début du vertex shader
        let srcVertexShader = dedent
            `#version 300 es

            // paramètres uniform
            uniform mat4 mat4ModelView;
            uniform mat4 mat4Projection;
            uniform mat3 mat3Normal;

            // attributs de sommets
            in vec3 glVertex;
            in vec3 glNormal;
            in vec2 glTexCoord;

            // morph target
            uniform float morphcoef;
            in vec3 glVertex1;
            in vec3 glNormal1;

            // interpolation vers les fragments
            out vec4 frgPosition;
            out vec3 frgNormal;
            out vec2 frgTexCoord;

            void main()
            {
                // interpolation entre référence et morph target
                vec3 vertex = mix(glVertex, glVertex1, morphcoef);
                vec3 normal = mix(glNormal, glNormal1, morphcoef);

                // transformations pour l'affichage
                frgPosition = mat4ModelView * vec4(vertex, 1.0);
                gl_Position = mat4Projection * frgPosition;
                frgNormal = mat3Normal * normal;
                frgTexCoord = glTexCoord;
            }`;
        return srcVertexShader;
    }


    /**
     * retourne le source du Fragment Shader
     */
    getFragmentShader()
    {
        let srcFragmentShader = "";
        srcFragmentShader += "#version 300 es\n";
        srcFragmentShader += "precision mediump float;\n";
        srcFragmentShader += "\n";
        srcFragmentShader += "// coordonnées et normale du fragment\n";
        srcFragmentShader += "in vec4 frgPosition;\n";
        srcFragmentShader += "in vec3 frgNormal;\n";
        srcFragmentShader += "in vec2 frgTexCoord;\n";
        srcFragmentShader += "\n";
        srcFragmentShader += "// caractéristiques du matériau\n";
        srcFragmentShader += "uniform sampler2D txDiffuse;\n";
        srcFragmentShader += "const vec3 Ks = "+vec3.str(this.m_Ks)+";\n";
        srcFragmentShader += "const float Ns = "+this.m_Ns.toFixed(2)+";\n";
        // plan de coupe
        if (this.m_ClipPlaneOn) {
            srcFragmentShader += "\n";
            srcFragmentShader += "// plan de coupe\n";
            srcFragmentShader += "uniform vec4 ClipPlane;\n";
        }
        srcFragmentShader += "\n";
        srcFragmentShader += "out vec4 glFragData[4];\n";
        srcFragmentShader += "\n";
        srcFragmentShader += "void main()\n";
        srcFragmentShader += "{\n";
        // plan de coupe
        if (this.m_ClipPlaneOn) {
            srcFragmentShader += "    if (dot(frgPosition, ClipPlane) < 0.0) discard;\n";
        }
        srcFragmentShader += "    vec4 Kd = texture(txDiffuse, frgTexCoord);\n";
        srcFragmentShader += "    // remplir les buffers avec les informations\n";
        srcFragmentShader += "    glFragData[0] = Kd;\n";
        srcFragmentShader += "    glFragData[1] = vec4(Ks, Ns);\n";
        srcFragmentShader += "    glFragData[2] = vec4(frgPosition.xyz, 1.0);\n";
        srcFragmentShader += "    glFragData[3] = vec4(normalize(frgNormal), 0.0);\n";
        srcFragmentShader += "}";
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
        this.m_TxDiffuseLoc = gl.getUniformLocation(this.m_ShaderId, "txDiffuse");
        this.m_MorphCoefLoc = gl.getUniformLocation(this.m_ShaderId, "morphcoef");
    }


    /**
     * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
     * @return le VBOset du matériau
     */
    createVBOset()
    {
        // créer le VBOset et spécifier les noms des attribute nécessaires à ce matériau
        let vboset = super.createVBOset();
        vboset.addAttribute(MeshVertex.ID_ATTR_COLOR,    Utils.VEC4, "glColor");
        vboset.addAttribute(MeshVertex.ID_ATTR_TEXCOORD, Utils.VEC2, "glTexCoord");
        vboset.addAttribute(MeshVertex.ID_ATTR_VERTEX1,  Utils.VEC3, "glVertex1");
        vboset.addAttribute(MeshVertex.ID_ATTR_NORMAL,   Utils.VEC3, "glNormal");
        vboset.addAttribute(MeshVertex.ID_ATTR_NORMAL1,  Utils.VEC3, "glNormal1");
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

        // activer la texture diffuse
        this.m_TxDiffuse.setTextureUnit(gl.TEXTURE0, this.m_TxDiffuseLoc);

        // fournir le coefficient de morphing
        gl.uniform1f(this.m_MorphCoefLoc, this.m_MorphCoef);
    }


    /**
     * Cette méthode désactive le matériau
     * NB : le shader doit être activé
     */
    disable()
    {
        // désactiver la texture diffuse
        this.m_TxDiffuse.setTextureUnit(gl.TEXTURE0);

        // appeler la méthode de la superclasse
        super.disable();
    }


    /**
     * définit la valeur du morph pour changer la forme de l'objet
     * @param coef entre 0 et 1
     */
    setMorphCoef(coef)
    {
        this.m_MorphCoef = coef;
    }
}
