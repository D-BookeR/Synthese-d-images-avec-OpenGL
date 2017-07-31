//// ce script fournit des fonctions utilitaires pour les programmes
//// du livre Synthèse d'images à l'aide d'OpenGL

// consulter http://blog.tojicode.com/2012/07/using-webgldepthtexture.html

package com.dbooker.livreopengl.process;

import android.util.Log;

import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.vec3;

import java.nio.IntBuffer;

import static android.opengl.GLES20.*;


@SuppressWarnings("unused")
public class ShadowMap extends FrameBufferObject
{
    private static final String TAG = "LivreOpenGL";

    // matrice de décalage des coordonnées de texture [-1,+1] -> [0,1]
    static public mat4 c_MatBias;

    // options de réduction de l'acné
    protected boolean m_OffsetFill = false;
    protected int m_CullFace = GL_NONE;

    // divers
    protected int m_CullFacePrec;


    /**
     * constructeur de la classe ShadowMap
     * @param shadowmapsize : largeur et hauteur de la carte d'ombre
     * @param offsetfill : true si la shadow map doit activer le décalage de polygones
     * @param cullface : mettre GL_FRONT ou GL_BACK pour éliminer les faces avant ou de dos, GL_NONE pour ne rien configurer
     */
    public ShadowMap(int shadowmapsize, boolean offsetfill, int cullface) throws Exception
    {
        // c'est un FBO ne contenant qu'une depth map de type texture
        super(shadowmapsize, shadowmapsize, GL_NONE, GL_TEXTURE_2D, 0, GL_LINEAR);
        init(offsetfill, cullface);
    }
    public ShadowMap(int shadowmapsize) throws Exception
    {
        // c'est un FBO ne contenant qu'une depth map de type texture
        super(shadowmapsize, shadowmapsize, GL_NONE, GL_TEXTURE_2D, 0, GL_LINEAR);
        init(false, GL_NONE);
    }

    /**
     * initialisations communes à tous les constructeurs
     * @param offsetfill : true si la shadow map doit activer le décalage de polygones
     * @param cullface : mettre GL_FRONT ou GL_BACK pour éliminer les faces avant ou de dos, GL_NONE pour ne rien configurer
     */
    private void init(boolean offsetfill, int cullface) throws Exception
    {
        // options de réduction de l'acné
        m_OffsetFill = offsetfill;
        m_CullFace   = cullface;
        m_CullFacePrec = 0;
    }


    /**
     * supprime cette shadowmap
     */
    public void destroy()
    {
        super.destroy();
    }


    /**
     * cette méthode change les modes de réduction de l'acné de surface
     * @param offsetfill : true si la shadow map doit activer le décalage de polygones
     * @param cullface : mettre GL_FRONT ou GL_BACK pour éliminer les faces avant ou de dos, GL_NONE pour ne rien configurer
     */
    public void setAcneReduction(boolean offsetfill, int cullface)
    {
        m_OffsetFill = offsetfill;
        m_CullFace   = cullface;
    }


    /**
     * redirige tous les tracés suivants vers la ShadowMap
     */
    public void enable()
    {
        // activer le FBO
        super.enable();

        // (optionnel) éliminer les faces avant, afin d'éviter l'acné de surface
        IntBuffer tmp = IntBuffer.allocate(4);
        glGetIntegerv(GL_CULL_FACE_MODE, tmp);
        m_CullFacePrec = tmp.get(0);
        if (m_CullFace != GL_NONE) {
            glEnable(GL_CULL_FACE);
            glCullFace(m_CullFace);
        }

        // (optionnel) décalage de polygones, afin d'éviter l'acné de surface
        if (m_OffsetFill) {
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(1.0f, 1.0f);
        }

        // effacer le depth buffer
        glClear(GL_DEPTH_BUFFER_BIT);
    }


    /**
     * cesser de rediriger les dessins dans la ShadowMap
     */
    @Override
    public void disable()
    {
        // désactiver le FBO
        super.disable();

        // remettre les modes tels qu'ils étaient avant
        glCullFace(m_CullFacePrec);

        // annuler le décalage
        glPolygonOffset(0.0f, 0.0f);
        glDisable(GL_POLYGON_OFFSET_FILL);
    }


    /**
     * cette fonction associe cette shadowmap en tant que texture à une unité de texture pour un shader
     * NB : le shader concerné doit être actif
     * @param unit : unité de texture concernée, par exemple GL_TEXTURE0
     * @param locSampler : emplacement de la variable uniform sampler* de cette texture dans le shader ou -1 pour désactiver la texture
     */
    public void setTextureUnit(int unit, int locSampler)
    {
        /*****DEBUG*****/
        if (unit < GL_TEXTURE0 || unit > GL_TEXTURE7) {
            Log.e(TAG, "FrameBufferObject.setTextureUnit: first parameter, unit is not GL_TEXTURE0 ... GL_TEXTURE7");
            return;
        }
        /*****DEBUG*****/

        // activer la shadow map en tant que texture
        glActiveTexture(unit);
        if (locSampler < 0) {
            glBindTexture(GL_TEXTURE_2D, 0);
        } else {
            glBindTexture(GL_TEXTURE_2D, m_DepthBufferId);
            glUniform1i(locSampler, unit-GL_TEXTURE0);
        }
    }
    @Override
    public void setTextureUnit(int unit)
    {
        setTextureUnit(unit, -1);
    }


    /**
     * dessine le depth buffer dans le viewport (pour la mise au point)
     */
    public void onDraw()
    {
        super.onDrawDepth();
    }


    /** initialise la matrice de biais */
    static {
        c_MatBias = mat4.create();
        mat4.translate(c_MatBias, c_MatBias, vec3.fromValues(0.5, 0.5, 0.5));
        mat4.scale(c_MatBias, c_MatBias, vec3.fromValues(0.5, 0.5, 0.5));
    }
}
