package com.dbooker.livreopengl;

import static android.opengl.GLES20.*;

import com.dbooker.livreopengl.mesh.MeshObject;
import com.dbooker.livreopengl.mesh.MeshObjectFromObj;
import com.dbooker.livreopengl.misc.TurnTableScene;
import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.glmatrix.vec4;
import com.dbooker.livreopengl.material.OmniLight;
import com.dbooker.livreopengl.material.SSAOLight;
import com.dbooker.livreopengl.utils.Utils;


class Scene extends TurnTableScene
{
    private MeshObject m_Buildings;
    private MeshObject m_Ground;
    private OmniLight m_Light0;
    private SSAOLight m_Light1;


    Scene() throws Exception
    {
        super(true);

        // créer les objets à dessiner
        m_Buildings = new MeshObjectFromObj("data/models/Immeubles", "immeubles.obj", "immeubles.mtl", 0.5f);
        m_Ground = new MeshObjectFromObj("data/models/Immeubles", "sol.obj", "sol.mtl", 0.5f);

        // définir une lampe omnidirectionnelle
        m_Light0 = new OmniLight();
        m_Light0.setPosition(vec4.fromValues(5, 5, 0, 1));
        m_Light0.setColor(vec3.fromValues(8.0,8.0,8.0));

        // définir une lampe ambiante avec ombres de type SSAO
        m_Light1 = new SSAOLight(0.05f, 1.0f, 4.0f);
        m_Light1.setColor(vec3.fromValues(0.8,0.8,0.8));

        // configurer les modes de dessin
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        // couleur du fond
        glClearColor(0.7f, 0.8f, 0.9f, 0.0f);

        // paramètres de la vue
        m_CameraAngleAzimut = 190.0f;
        m_CameraAngleElevation = 25.0f;
        vec3.set(m_CameraPivot, 0.0f,-1.0f,0.0f);
        m_CameraDistance = 24;
    }


    /**
     * appelée quand la taille de la vue OpenGL change
     * @param width : largeur en nombre de pixels de la fenêtre
     * @param height : hauteur en nombre de pixels de la fenêtre
     */
    @Override
    public void onSurfaceChanged(int width, int height)
    {
        // appeler la méthode de la superclasse
        super.onSurfaceChanged(width, height);

        // matrice de projection (champ de vision)
        mat4.perspective(m_Mat4Projection, Utils.radians(12.0f), (float)width / height, 1.0f, 100.0f);
    }


    /**
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4View : matrice de vue
     */
    @Override
    public void onDraw(mat4 mat4Projection, mat4 mat4View)
    {
        // dessiner les immeubles et le sol
        m_Buildings.onDraw(mat4Projection, mat4View);
        m_Ground.onDraw(mat4Projection, mat4View);
    }


    /**
     * dessine les shadow maps des lampes
     * @param mat4View : matrice de vue de la lampe
     */
    @Override
    protected void prepareLights(mat4 mat4View)
    {
        // rendre les lampes fixes par rapport à la scène
        m_Light0.transform(mat4View);

        // les cibles des lampes sont relatives au pivot des rotations
        m_Light0.transformTarget(mat4View);

        // calculer les shadow maps des lampes
        m_Light0.makeShadowMap(this, mat4View);
    }


    /**
     * Dessine l'image courante
     */
    @Override
    public void onDrawFrame()
    {
        // construire la matrice caméra
        mat4 mat4View = getModelView();

        // faire tourner la lampe autour de la scène
        mat4 mat4Light = mat4.create();
        mat4.rotateY(mat4Light, mat4View, Utils.Time*0.5f);

        // préparer les lampes (transformation et shadow maps)
        prepareLights(mat4Light);

        // dessiner les objets dans cette transformation
        drawDeferredShading(m_Mat4Projection, mat4View);

        // (debug) dessiner l'un des buffers sur l'écran
        //m_GBuffer.onDraw(GL_COLOR_ATTACHMENT0);
        //m_GBuffer.onDraw(GL_DEPTH_ATTACHMENT);

        // effacer l'écran
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // dessiner l'éclairement de la première lampe
        addLighting(m_Light0, true);

        // rajouter les éclairements des lampes suivantes
        addLighting(m_Light1);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    @Override
    public void destroy()
    {
        m_Light1.destroy();
        m_Light0.destroy();
        m_Ground.destroy();
        m_Buildings.destroy();
        super.destroy();
    }
}
