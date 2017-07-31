//// ce script fournit des fonctions utilitaires pour les programmes
//// du livre Synthèse d'images à l'aide d'OpenGL

// consulter http://blog.tojicode.com/2012/07/using-webgldepthtexture.html

// superclasses et classes nécessaires
Requires("libs/Process/FrameBufferObject.js");


/**
 * C'est un FBO particulier, il n'y a qu'un depth buffer
 * On l'utilise pour dessiner les ombres dans une scène
 */
class ShadowMap extends FrameBufferObject
{
    /**
     * constructeur de la classe ShadowMap
     * @param shadowmapsize : largeur et hauteur de la carte d'ombre
     * @param offsetfill : true si la shadow map doit activer le décalage de polygones
     * @param cullface : mettre gl.FRONT ou gl.BACK pour éliminer les faces avant ou de dos, gl.NONE pour ne rien configurer
     */
    constructor(shadowmapsize, offsetfill=false, cullface=gl.NONE)
    {
        // c'est un FBO ne contenant qu'une depth map de type texture
        super(shadowmapsize, shadowmapsize, gl.TEXTURE_2D, gl.TEXTURE_2D, 0, gl.NEAREST);

        // options de réduction de l'acné
        this.m_OffsetFill = offsetfill;
        this.m_CullFace   = cullface;
    }


    /**
     * supprime cette shadowmap
     */
    destroy()
    {
        super.destroy();
    }


    /**
     * cette méthode change les modes de réduction de l'acné de surface
     * @param offsetfill : true si la shadow map doit activer le décalage de polygones
     * @param cullface : mettre GL_FRONT ou GL_BACK pour éliminer les faces avant ou de dos, GL_NONE pour ne rien configurer
     */
    setAcneReduction(offsetfill, cullface)
    {
        this.m_OffsetFill = offsetfill;
        this.m_CullFace   = cullface;
    }


    /**
     * redirige tous les tracés suivants vers la ShadowMap
     */
    enable()
    {
        // activer le FBO
        super.enable();

        // (optionnel) éliminer les faces avant, afin d'éviter l'acné de surface
        this.m_CullFacePrec = gl.getParameter(gl.CULL_FACE_MODE);
        if (this.m_CullFace != gl.NONE) {
            gl.enable(gl.CULL_FACE);
            gl.cullFace(this.m_CullFace);
        }

        // (optionnel) décalage de polygones, afin d'éviter l'acné de surface
        if (this.m_OffsetFill) {
            gl.enable(gl.POLYGON_OFFSET_FILL);
            gl.polygonOffset(1.0, 1.0);
        }

        // effacer le depth buffer
        gl.clear(gl.DEPTH_BUFFER_BIT);
    }


    /**
     * cesser de rediriger les dessins dans la ShadowMap
     */
    disable()
    {
        // désactiver le FBO
        super.disable();

        // remettre les modes tels qu'ils étaient avant
        gl.cullFace(this.m_CullFacePrec);

        // annuler le décalage
        gl.polygonOffset(0.0, 0.0);
        gl.disable(gl.POLYGON_OFFSET_FILL);
    }


    /**
     * cette fonction associe cette shadowmap en tant que texture2D à une unité de texture pour un shader
     * NB: le shader concerné doit être actif
     * @param unit : unité de texture concernée, par exemple gl.TEXTURE0
     * @param locSampler : emplacement de la variable uniform sampler* de cette texture dans le shader ou null pour désactiver la texture
     */
    setTextureUnit(unit, locSampler=null)
    {
        /*****DEBUG*****/
        if (unit < gl.TEXTURE0 || unit > gl.TEXTURE7) {
            error("ShadowMap.setTextureUnit: first parameter, unit is not gl.TEXTURE0 ... gl.TEXTURE7");
        }
        if (locSampler != null && locSampler.constructor !== WebGLUniformLocation) {
            error("ShadowMap.setTextureUnit: third parameter, locSampler is not either null or the location of a uniform variable : "+locSampler);
        }
        /*****DEBUG*****/

        // activer la shadow map en tant que texture
        gl.activeTexture(unit);
        if (locSampler == null) {
            gl.bindTexture(gl.TEXTURE_2D, null);
        } else {
            gl.bindTexture(gl.TEXTURE_2D, this.m_DepthBufferId);
            gl.uniform1i(locSampler, unit-gl.TEXTURE0);
        }
    }


    /**
     * dessine le depth buffer dans le viewport (pour la mise au point)
     */
    onDraw()
    {
        super.onDrawDepth();
    }
}


/** initialise la matrice de biais */
ShadowMap.c_MatBias = mat4.create();
mat4.translate(ShadowMap.c_MatBias, ShadowMap.c_MatBias, vec3.fromValues(0.5, 0.5, 0.5));
mat4.scale(ShadowMap.c_MatBias, ShadowMap.c_MatBias, vec3.fromValues(0.5, 0.5, 0.5));
