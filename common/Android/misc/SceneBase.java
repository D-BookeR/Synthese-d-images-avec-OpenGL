package com.dbooker.livreopengl.misc;

import android.util.Log;
import static android.opengl.GLES20.*;

import java.util.ArrayList;

import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.material.Light;
import com.dbooker.livreopengl.material.Material;
import com.dbooker.livreopengl.process.FrameBufferObject;
import com.dbooker.livreopengl.utils.Utils;


/**
 * Définit une base pour dessiner une scène
 */
@SuppressWarnings("unused")
public class SceneBase
{
    private static final String TAG = "LivreOpenGL";

    // matrice de projection et de transformation
    protected mat4 m_Mat4Projection;
    protected mat4 m_Mat4ModelView;

    // lampes de la scène
    protected ArrayList<Light> m_Lights;

    protected boolean m_DeferredShading;
    protected FrameBufferObject m_GBuffer;


    /**
     * Définit une base pour dessiner une scène
     * @param deferredShading : true si cette classe gère le FBO m_GBuffer pour un dessin en deux passes
     */
    public SceneBase(boolean deferredShading)
    {
        // configurer les modes de dessin
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        // couleur du fond
        glClearColor(0.4f, 0.4f, 0.4f, 0.0f);

        // lampes
        m_Lights = new ArrayList<>();

        // FBO pour dessiner en mode Multiple Render Targets
        m_DeferredShading = deferredShading;
        m_GBuffer = null;     // sera initialisé dans onSurfaceChanged

        // matrice de projection et de transformation
        m_Mat4Projection = mat4.create();
        m_Mat4ModelView = mat4.create();
    }


    /** destructeur */
    public void destroy()
    {
        if (m_GBuffer != null) m_GBuffer.destroy();
    }


    /** affiche les informations sur la caméra **/
    protected void updateInfos()
    {
    }


    /**
     * appelée quand la taille de la vue OpenGL change
     * @param width : largeur en nombre de pixels de la fenêtre
     * @param height : hauteur en nombre de pixels de la fenêtre
     * @param scale : facteur d'agrandissement du FBO pour simuler un antialiasing global
     */
    public void onSurfaceChanged(int width, int height, int scale)
    {
        // met en place le viewport
        glViewport(0, 0, width, height);

        // créer un FBO pour dessiner hors écran avec plusieurs buffers pour stocker toutes les informations nécessaires
        if (m_GBuffer != null) m_GBuffer.destroy();
        m_GBuffer = null;
        if (m_DeferredShading) {
            try {
                m_GBuffer = new FrameBufferObject(width * scale, height * scale, GL_TEXTURE_2D, GL_TEXTURE_2D, 3);
            } catch (Exception e) {
                Log.e(TAG, "Impossible de créer le FrameBufferObject pour dessiner");
            }
        }
    }
    /**
     * @see #onSurfaceChanged avec une valeur par défaut de 2 pour l'échelle
     * @param width : largeur en nombre de pixels de la fenêtre
     * @param height : hauteur en nombre de pixels de la fenêtre
     */
    public void onSurfaceChanged(int width, int height)
    {
        onSurfaceChanged(width, height, 1);
    }


    /**
     * appelée quand on enfonce le bouton du milieu et qu'on déplace la souris
     * @param dz : nombre de pixels parcourus
     */
    public void onZoom(float dz)
    {
    }


    /**
     * appelée quand on enfonce le bouton droit et qu'on déplace la souris
     * @param dx : nombre de pixels parcourus horizontalement
     * @param dy : nombre de pixels parcourus verticalement
     */
    public void onPan(float dx, float dy)
    {
    }


    /**
     * appelée quand on enfonce le bouton gauche et qu'on déplace la souris
     * @param oldX : ancienne absisse de la souris
     * @param oldY : ancienne ordonnée de la souris
     * @param x : nouvelle absisse de la souris
     * @param y : nouvelle ordonnée de la souris
     * @param width : largeur de la vue OpenGL
     * @param height : hauteur de la vue OpenGL
     */
    public void onRotate(float oldX, float oldY, float x, float y, int width, int height)
    {
    }


    /**
     * appelée quand on appuie sur une touche du clavier
     * @param code : touche enfoncée
     */
    public void onKeyDown(char code)
    {
        switch (code) {
        case 'p':
            // mettre en pause
            Utils.PauseTime();
            break;
        default:
        }
    }


    /**
     * retourne la matrice ModelView pour dessiner la scène
     * @return matrice ModelView
     */
    protected mat4 getModelView()
    {
        return m_Mat4ModelView;
    }


    /**
     * ajoute une lampe pour éclairer le matériau du renderer
     * @param light : lampe à ajouter
     */
    public void addLight(Light light)
    {
        m_Lights.add(light);
    }


    /**
     * transforme toutes les lampes
     * c'est à dire calcule la position relative à la caméra m_PositionCamera
     * à partir de la position relative à la scène m_Position
     * @param mat4View matrice de positionnement de la scène par rapport à la caméra
     */
    protected void transformLights(mat4 mat4View)
    {
        for (Light light: m_Lights) {
            light.transform(mat4View);
        }
    }


    /**
     * dessine les shadow maps des lampes
     * @param mat4View : matrice de positionnement de la scène par rapport à la caméra
     */
    protected void makeShadowMaps(mat4 mat4View)
    {
        for (Light light: m_Lights) {
            // calculer sa shadow map (si la lampe en gère une)
            light.makeShadowMap(this, mat4View);
        }
    }


    /**
     * transforme les lampes puis calcule leur shadow map s'il y en a
     * @param mat4View : matrice de positionnement de la scène par rapport à la caméra
     */
    protected void prepareLights(mat4 mat4View)
    {
        transformLights(mat4View);
        makeShadowMaps(mat4View);
    }


    /**
     * appelée pour dessiner en mode MRT
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    protected void drawDeferredShading(mat4 mat4Projection, mat4 mat4ModelView)
    {
        if (m_DeferredShading && m_GBuffer != null) {
            // dessin différé en mode multiple render targets (MRT)

            /*** MODE MRT SIMULÉ ***/
            // tester si on a le vrai mode MRT
            if (m_GBuffer.isMRT()) {
            /*** MODE MRT SIMULÉ ***/

                // rediriger les dessins vers le FBO
                m_GBuffer.enable();

                // effacer l'écran
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                // dessiner les objets de la scène
                onDraw(mat4Projection, mat4ModelView);

                // revenir au dessin sur l'écran
                m_GBuffer.disable();

            /*** MODE MRT SIMULÉ ***/
            } else {

                for (int i=0; i<Material.GBufferSize; i++) {

                    // indiquer à tous les matériaux MRT qu'on dessine le buffer n°i
                    Material.setMRTBuffer(i);

                    // rediriger les dessins vers le color buffer n°i FBO
                    m_GBuffer.enable(i);

                    // effacer l'écran
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                    // dessiner les objets de la scène
                    onDraw(mat4Projection, mat4ModelView);

                    // revenir au dessin sur l'écran
                    m_GBuffer.disable();
                }
            }
            /*** MODE MRT SIMULÉ ***/

            /*****DEBUG*****/
            // dessiner l'un des buffers sur l'écran
            //m_GBuffer.onDraw(GL_COLOR_ATTACHMENT0, false);
            //m_GBuffer.onDraw(GL_COLOR_ATTACHMENT1, false);
            //m_GBuffer.onDraw(GL_COLOR_ATTACHMENT2, false);
            //m_GBuffer.onDraw(GL_COLOR_ATTACHMENT3, false);
            //m_GBuffer.onDraw(GL_DEPTH_ATTACHMENT);
            /*****DEBUG*****/

            // si le FBO ne peut pas faire du MRT (multiple render target), alors afficher le color buffer
            //if (! m_GBuffer.isMRT()) m_GBuffer.onDraw(GL_COLOR_ATTACHMENT0, false);


        } else {
            // dessin en mode direct

            // effacer l'écran
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // dessiner les objets de la scène
            onDraw(mat4Projection, mat4ModelView);
        }
    }


    /**
     * rajoute l'éclairement d'une lampe
     * @param light : lampe à ajouter
     * @param is_first : mettre true si c'est la première qu'on ajoute ainsi, false sinon
     */
    protected void addLighting(Light light, boolean is_first)
    {
        if (!m_DeferredShading || m_GBuffer == null) return;

        if (is_first) {

            // dessiner l'éclairement de la première lampe
            light.process(m_GBuffer);

        } else {

            // passer en mode blending, en tenant compte du canal alpha des éclairements ajoutés
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);

            // rajouter l'éclairement de la lampe
            light.process(m_GBuffer);

            // revenir en mode normal
            glDisable(GL_BLEND);
        }
    }
    /**
     * rajoute l'éclairement d'une lampe, sauf la première
     * @param light : lampe à ajouter
     */
    protected void addLighting(Light light)
    {
        addLighting(light, false);
    }


    /**
     * rajoute les éclairements de toutes les lampes
     */
    protected void addLightings()
    {
        if (m_DeferredShading && m_GBuffer != null) {
            boolean is_first = true;
            for (Light light: m_Lights) {
                addLighting(light, is_first);
                is_first = false;
            }
        }
    }


    /**
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    public void onDraw(mat4 mat4Projection, mat4 mat4ModelView)
    {
    }


    /**
     * dessin de la scène sur l'écran
     * @throws Exception
     */
    public void onDrawFrame() throws Exception
    {
        // effacer l'écran
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // construire la matrice MV
        mat4 mat4ModelView = getModelView();

        // positionner les lampes et dessiner leur shadow map s'il y en a
        prepareLights(mat4ModelView);

        // dessiner les objets dans le FBO mrt (ou sur l'écran)
        drawDeferredShading(m_Mat4Projection, mat4ModelView);

        // ajouter les éclairements des lampes
        addLightings();
    }
}
