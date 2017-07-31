// Définition de la classe MultipleTextureMaterial

// superclasses et classes nécessaires
Requires("libs/Material/Material.js");
Requires("libs/Material/VBOset.js");


class MultipleTextureMaterial extends Material
{
    /**
     * Constructeur
     * @param texture sur laquelle est basé le matériau
     * @param columns nombre de sous-images horizontalement = nombre de colonnes
     * @param lines nombre de sous-images verticalement = nombre de lignes
     * @param delay temps entre deux sous-images pour l'animation
     */
    constructor(texture, columns=1, lines=1, delay=0.05)
    {
        super("MultipleTextureMaterial");

        // initialisations
        this.m_Texture = texture;
        this.m_TextureLoc = -1;

        // caractéristiques de l'image
        this.m_LineCount = lines;
        this.m_ColumnCount = columns;
        this.m_TotalNumber = columns * lines;
        this.m_Size = vec2.fromValues(columns, lines);
        this.m_Offset = vec2.create();
        this.m_Delay = delay;

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
            in vec2 glTexCoord;
            uniform mat4 mat4ModelView;
            uniform mat4 mat4Projection;
            uniform vec2 offset;
            uniform vec2 size;

            // interpolation vers les fragments
            out vec4 frgPosition;
            out vec2 frgTexCoord;

            void main()
            {
                frgPosition = mat4ModelView * vec4(glVertex, 1.0);
                gl_Position = mat4Projection * frgPosition;
                // coordonnées de texture altérées pour viser une sous-image
                frgTexCoord = (glTexCoord + offset) / size;
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
            in vec4 frgPosition;
            in vec2 frgTexCoord;

            uniform sampler2D txColor;

            // g-buffer
            out vec4 glFragData[4];

            void main()
            {
                // accès à la texture
                glFragData[0] = texture(txColor, frgTexCoord);
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

        // déterminer où sont les variables uniform
        this.m_TextureLoc = gl.getUniformLocation(this.m_ShaderId, "txColor");
        this.m_OffsetLoc  = gl.getUniformLocation(this.m_ShaderId, "offset");
        this.m_SizeLoc    = gl.getUniformLocation(this.m_ShaderId, "size");
    }


    /**
     * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
     * @return le VBOset du matériau
     */
    createVBOset()
    {
        // créer le VBOset et spécifier les noms des attribute nécessaires à ce matériau
        let vboset = super.createVBOset();
        vboset.addAttribute(MeshVertex.ID_ATTR_TEXCOORD, Utils.VEC2, "glTexCoord");
        return vboset;
    }


    /**
     * sélectionne la partie de la Texture2D correspondant au temps
     */
    update()
    {
        // déterminer le numéro de l'image d'après le temps
        let number = Math.floor(Utils.Time / this.m_Delay) % this.m_TotalNumber;

        // déterminer la ligne et la colonne d'après le numéro
        let column = number % this.m_ColumnCount;
        let line = this.m_LineCount-1-Math.floor(number / this.m_ColumnCount);

        // changer le décalage d'image pour le shader
        vec2.set(this.m_Offset, column, line);
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

        // mettre à jour l'image
        this.update();

        // spécifier le coefficient de teinte et le coefficient de transparence
        vec2.glUniform(this.m_OffsetLoc, this.m_Offset);
        vec2.glUniform(this.m_SizeLoc,  this.m_Size);

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
