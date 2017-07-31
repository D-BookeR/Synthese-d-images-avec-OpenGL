/**
 * Définition de la classe TransparentMaterial, une spécialisation de DeferredShadingMaterial
 * Ce matériau est destiné à dessiner en mode Multiple Render Targets
 */

// superclasses et classes nécessaires
Requires("libs/Material/DeferredShadingMaterial.js");


class TransparentMaterial extends DeferredShadingMaterial
{
    /**
     * Constructeur de la classe TransparentMaterial. Ce matériau mémorise dans un FBO MRT les informations
     * nécessaires pour appliquer le modèle de Phong ultérieurement : position, normale, couleur.
     * @param Kd : un vec4(r,g,b,a) ou une texture donnant la couleur diffuse et la transparence
     * @param Ks : un vec3(r,g,b) donnant la couleur spéculaire
     * @param Ns : poli du matériau
     */
    constructor(Kd, Ks, Ns)
    {
        super(Kd, Ks, Ns);

        // identifiants des textures associées aux sampler2D DepthMap*
        this.m_DepthMapFar = null;
        this.m_DepthMapNear = null;

        // dimensions de la fenêtre pour normaliser gl_FragCoord.xy
        this.m_WindowSize = vec2.fromValues(1.0, 1.0);

        // recompiler le shader
        this.compileShader();
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

        // depth maps à prendre en compte (précédentes distances pour rester devant les opaques, mais derrière les transparents)
        srcFragmentShader += "uniform sampler2D DepthMapFar;\n";
        srcFragmentShader += "uniform sampler2D DepthMapNear;\n";
        srcFragmentShader += "uniform vec2 WindowSize;\n";

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

        // test du depth buffer pour le depth peeling
        srcFragmentShader += "    vec2 fragcoords = gl_FragCoord.xy/WindowSize;\n";
        srcFragmentShader += "    float depth = texture(DepthMapFar, fragcoords).x;\n";
        srcFragmentShader += "    if (gl_FragCoord.z >= depth) discard;\n";
        srcFragmentShader += "    depth = texture(DepthMapNear, fragcoords).x;\n";
        srcFragmentShader += "    if (gl_FragCoord.z <= depth) discard;\n";

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
        srcFragmentShader += "    glFragData[0] = vec4(Kd.rgb * Kd.a, Kd.a); // prémultiplication par alpha\n";
        if (this.m_Ns >= 0.0) {
            srcFragmentShader += "    glFragData[1] = vec4(Ks * Kd.a, Ns);\n";
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
        this.m_DepthMapFarLoc  = gl.getUniformLocation(this.m_ShaderId, "DepthMapFar");
        this.m_DepthMapNearLoc = gl.getUniformLocation(this.m_ShaderId, "DepthMapNear");
        this.m_WindowSizeLoc   = gl.getUniformLocation(this.m_ShaderId, "WindowSize");
    }


    /**
     * affecte la texture DepthMap du shader
     * @param far : depth buffer qui limite les dessins arrière de ce matériau
     * @param near : depth buffer qui limite les dessins avant de ce matériau
     */
    setDepthMaps(far, near)
    {
        this.m_DepthMapFar = far;
        this.m_DepthMapNear = near;
    }


    /**
     * spécifie la taille de la fenêtre
     * @param width : largeur en nombre de pixels de la fenêtre
     * @param height : hauteur en nombre de pixels de la fenêtre
     */
    setWindowDimensions(width, height)
    {
        this.m_WindowSize = vec2.fromValues(width, height);
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

        // fournir et activer la DepthMapFar dans l'unité 2
        gl.activeTexture(gl.TEXTURE2);
        gl.bindTexture(gl.TEXTURE_2D, this.m_DepthMapFar);
        gl.uniform1i(this.m_DepthMapFarLoc, gl.TEXTURE2-gl.TEXTURE0);

        // fournir et activer la DepthMapNear dans l'unité 3
        gl.activeTexture(gl.TEXTURE3);
        gl.bindTexture(gl.TEXTURE_2D, this.m_DepthMapNear);
        gl.uniform1i(this.m_DepthMapNearLoc, gl.TEXTURE3-gl.TEXTURE0);

        // fournir les dimensions de la fenêtre
        vec2.glUniform(this.m_WindowSizeLoc, this.m_WindowSize);
    }


    /**
     * Cette méthode désactive le matériau
     * @note le shader doit être activé
     */
    disable()
    {
        // désactiver les depth maps
        gl.activeTexture(gl.TEXTURE2);
        gl.bindTexture(gl.TEXTURE_2D, null);
        gl.activeTexture(gl.TEXTURE3);
        gl.bindTexture(gl.TEXTURE_2D, null);

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
