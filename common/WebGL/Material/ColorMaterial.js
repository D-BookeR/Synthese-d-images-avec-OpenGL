// Définition de la classe ColorMaterial, une spécialisation de Material
// Ce matériau permet de dessiner en mode Multiple Render Targets


// superclasses et classes nécessaires
Requires("libs/Material/Material.js");


/**
 * Une instance de cette classe dessine d'une couleur uniforme
 */
class ColorMaterial extends Material
{
    /**
     * constructeur
     * @param color : vec4(r,v,b,a) qui donne la couleur du matériau
     */
    constructor(color=null)
    {
        super("ColorMaterial");

        // couleur propre imposée ou aléatoire
        this.m_Color = vec4.create();
        if (color != null) {
            vec4.copy(this.m_Color, color);
        } else {
            vec4.random(this.m_Color, 1.0);
        }

        // compiler le shader
        this.compileShader();
    }


    /** destructeur */
    destroy()
    {
        super.destroy();
    }


    /**
     * retourne le source du Vertex Shader
     */
    getVertexShader()
    {
        let srcVertexShader = dedent
            `#version 300 es

            // paramètres uniform
            uniform mat4 mat4ModelView;
            uniform mat4 mat4Projection;
            uniform mat3 mat3Normal;

            // attributs de sommets
            in vec3 glVertex;
            in vec3 glNormal;

            // interpolation vers les fragments
            out vec4 frgPosition;
            out vec3 frgNormal;

            void main()
            {
                frgPosition = mat4ModelView * vec4(glVertex, 1.0);
                gl_Position = mat4Projection * frgPosition;
                frgNormal   = mat3Normal * glNormal;
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
        if (this.m_ClipPlaneOn) {
            srcFragmentShader += "\n";
            srcFragmentShader += "// plan de coupe\n";
            srcFragmentShader += "uniform vec4 ClipPlane;\n";
        }
        srcFragmentShader += "\n";
        srcFragmentShader += "in vec4 frgPosition;\n";
        srcFragmentShader += "in vec3 frgNormal;\n";
        srcFragmentShader += "out vec4 glFragData[4];\n";
        srcFragmentShader += "\n";
        srcFragmentShader += "void main()\n";
        srcFragmentShader += "{\n";
        if (this.m_ClipPlaneOn) {
            srcFragmentShader += "    // plan de coupe\n";
            srcFragmentShader += "    if (dot(frgPosition, ClipPlane) < 0.0) discard;\n";
            srcFragmentShader += "\n";
        }
        srcFragmentShader += "    glFragData[0] = "+vec4.str(this.m_Color)+";\n";
        srcFragmentShader += "    glFragData[1] = vec4(0.0);\n";
        srcFragmentShader += "    glFragData[2] = vec4(frgPosition.xyz, 1.0);\n";
        srcFragmentShader += "    glFragData[3] = vec4(frgNormal, 0.0);\n";
        srcFragmentShader += "}";
        return srcFragmentShader;
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
}
