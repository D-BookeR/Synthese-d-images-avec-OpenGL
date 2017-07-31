// Définition de la classe DeferredShadingMaterial, une spécialisation de Material
// Ce matériau est destiné à dessiner en mode Multiple Render Targets


// superclasses et classes nécessaires
Requires("libs/Material/Material.js");


class DeferredShadingMaterial extends Material
{
    /**
     * Constructeur de la classe DeferredShadingMaterial. Ce matériau mémorise dans un FBO MRT les informations
     * nécessaires pour appliquer le modèle de Phong ultérieurement : position, normale, couleur.
     * @param Kd : plusieurs possibilités : null => la couleur diffuse est fournie par un VBO, un vec4(r,g,b,a) ou un nom de texture
     * @param Ks : plusieurs possibilités : null => la couleur spéculaire est fournie par un VBO, un vec3(r,g,b) ou un nom de texture
     * @param Ns : poli du matériau
     */
    constructor(Kd=null, Ks=null, Ns=null)
    {
        super("DeferredShadingMaterial");

        // couleur diffuse
        if (typeof(Kd) == "string") {
            this.m_TxDiffuse = new Texture2D(Kd, gl.LINEAR, gl.REPEAT);
            this.m_KdIsInterpolated = false;
        } else {
            this.m_TxDiffuse = null;
            if (Kd == null) {
                this.m_KdIsInterpolated = true;
            } else {
                this.m_KdIsInterpolated = false;
                this.m_Kd = Kd;
            }
        }

        // couleur spéculaire
        this.m_Ks = Ks;
        this.m_Ns = Ns;
        if (Ks == null || Ns == null) {
            this.m_TxSpecular = null;
            this.m_Ns = -1.0;
        } else {
            if (typeof(Ks) == "string") {
                this.m_TxSpecular = new Texture2D(Ks, gl.LINEAR, gl.REPEAT);
            } else {
                this.m_TxSpecular = null;
            }
        }

        // compiler le shader
        this.compileShader();
    }


    /** destructeur */
    destroy()
    {
        // libérer les textures qui ont été chargées
        if (this.m_TxDiffuse != null) this.m_TxDiffuse.destroy();
        if (this.m_TxSpecular != null) this.m_TxSpecular.destroy();
        super.destroy();
    }


    /**
     * retourne le source du Vertex Shader
     */
    getVertexShader()
    {
        // début du vertex shader
        let srcVertexShader = "";
        srcVertexShader += "#version 300 es\n";
        srcVertexShader += "\n";
        srcVertexShader += "// paramètres uniform\n";
        srcVertexShader += "uniform mat4 mat4ModelView;\n";
        srcVertexShader += "uniform mat4 mat4Projection;\n";
        srcVertexShader += "uniform mat3 mat3Normal;\n";
        srcVertexShader += "\n";
        srcVertexShader += "// attributs de sommets\n";
        srcVertexShader += "in vec3 glVertex;\n";
        srcVertexShader += "in vec3 glNormal;\n";
        srcVertexShader += "\n";
        srcVertexShader += "// interpolation vers les fragments\n";
        srcVertexShader += "out vec4 frgPosition;\n";
        srcVertexShader += "out vec3 frgNormal;\n";

        // interpoler l'attribut couleur diffuse ?
        if (this.m_KdIsInterpolated) {
            // la couleur diffuse est un attribut de vertex
            srcVertexShader += "in vec4 glColor;\n";
            srcVertexShader += "out vec4 frgColor;\n";
        }

        // interpoler les coordonnées de texture ?
        if (this.m_TxDiffuse != null || this.m_TxSpecular != null) {
            // passer les coordonnées de texture
            srcVertexShader += "in vec2 glTexCoord;\n";
            srcVertexShader += "out vec2 frgTexCoord;\n";
        }

        srcVertexShader += "\n";
        srcVertexShader += "void main()\n";
        srcVertexShader += "{\n";
        srcVertexShader += "    frgPosition = mat4ModelView * vec4(glVertex, 1.0);\n";
        srcVertexShader += "    gl_Position = mat4Projection * frgPosition;\n";
        srcVertexShader += "    frgNormal = mat3Normal * glNormal;\n";
        if (this.m_KdIsInterpolated) {
            // interpoler la couleur diffuse
            srcVertexShader += "    frgColor = glColor;\n";
        } else
        if (this.m_TxDiffuse != null || this.m_TxSpecular != null) {
            // interpoler les coordonnées de texture
            srcVertexShader += "    frgTexCoord = glTexCoord;\n";
        }
        srcVertexShader += "}\n";
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

        // coordonnées de texture s'il y a une texture
        if (this.m_TxDiffuse != null || this.m_TxSpecular != null) {
            // coordonnées de texture interpolées
            srcFragmentShader += "in vec2 frgTexCoord;\n";
        }

        srcFragmentShader += "\n";
        srcFragmentShader += "// caractéristiques du matériau\n";

        // couleur ou texture diffuse
        if (this.m_TxDiffuse != null) {
            srcFragmentShader += "uniform sampler2D txDiffuse;\n";
        } else if (this.m_KdIsInterpolated) {
            srcFragmentShader += "in vec4 frgColor;\n";
        } else {
            srcFragmentShader += "const vec4 Kd = "+vec4.str(this.m_Kd)+";\n";
        }

        // couleur ou texture spéculaire
        if (this.m_Ns >= 0.0) {
            if (this.m_TxSpecular != null) {
                srcFragmentShader += "uniform sampler2D txSpecular;\n";
            } else {
                srcFragmentShader += "const vec3 Ks = "+vec3.str(this.m_Ks)+";\n";
            }
            srcFragmentShader += "const float Ns = "+this.m_Ns.toFixed(2)+";\n";
        }

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

        // couleur diffuse du fragment
        if (this.m_TxDiffuse != null) {
            srcFragmentShader += "    vec4 Kd = texture(txDiffuse, frgTexCoord);\n";
        } else if (this.m_KdIsInterpolated) {
            srcFragmentShader += "    vec4 Kd = frgColor;\n";
        }

        // couleur spéculaire du fragment
        if (this.m_Ns >= 0.0 && this.m_TxSpecular != null) {
            srcFragmentShader += "    vec3 Ks = texture(txSpecular, frgTexCoord).rgb;\n";
        }

        // remplir les buffers MRT avec les informations nécessaires pour Phong plus tard
        srcFragmentShader += "    // remplir les buffers avec les informations\n";
        srcFragmentShader += "    glFragData[0] = Kd;\n";
        if (this.m_Ns >= 0.0) {
            srcFragmentShader += "    glFragData[1] = vec4(Ks, Ns);\n";
        } else {
            srcFragmentShader += "    glFragData[1] = vec4(0.0);\n";
        }
        srcFragmentShader += "    glFragData[2] = vec4(frgPosition.xyz, 1.0);\n";
        srcFragmentShader += "    glFragData[3] = vec4(normalize(frgNormal), 0.0);\n";
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
        this.m_TxDiffuseLoc  = gl.getUniformLocation(this.m_ShaderId, "txDiffuse");
        this.m_TxSpecularLoc = gl.getUniformLocation(this.m_ShaderId, "txSpecular");
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

        // prochaine unité de texture à utiliser
        let unit = gl.TEXTURE0;

        // activer la texture diffuse si elle est utilisée
        if (this.m_TxDiffuseLoc != null) {
            this.m_TxDiffuse.setTextureUnit(unit, this.m_TxDiffuseLoc);
            unit += 1;
        }

        // activer la texture spéculaire si elle est utilisée
        if (this.m_TxSpecularLoc != null) {
            this.m_TxSpecular.setTextureUnit(unit, this.m_TxSpecularLoc);
            unit += 1;
        }
    }


    /**
     * Cette méthode désactive le matériau
     * NB : le shader doit être activé
     */
    disable()
    {
        // prochaine unité de texture à utiliser
        let unit = gl.TEXTURE0;

        // désactiver la texture diffuse si elle est utilisée
        if (this.m_TxDiffuseLoc >= 0 && this.m_TxDiffuse != null) {
            this.m_TxDiffuse.setTextureUnit(unit);
            unit += 1;
        }

        // désactiver la texture spéculaire si elle est utilisée
        if (this.m_TxSpecularLoc >= 0 && this.m_TxSpecular != null) {
            this.m_TxSpecular.setTextureUnit(unit);
            unit += 1;
        }

        // appeler la méthode de la superclasse
        super.disable();
    }
}
