package com.dbooker.livreopengl;

import static android.opengl.GLES20.*;

import com.dbooker.livreopengl.mesh.MeshObject;
import com.dbooker.livreopengl.mesh.MeshObjectFromObj;
import com.dbooker.livreopengl.misc.TurnTableScene;
import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.glmatrix.vec4;
import com.dbooker.livreopengl.material.Light;
import com.dbooker.livreopengl.material.SpotLightDebug;
import com.dbooker.livreopengl.utils.Utils;


class Scene extends TurnTableScene
{
    private MeshObject m_Ground;
    private MeshObject m_Lorry;
    private MeshObject m_PalmTree;
    private Light m_Light0;
    private SpotLightDebug m_Light1;
    private SpotLightDebug m_Light2;


    Scene() throws Exception
    {
        super(true);

        // créer les objets à dessiner
        m_Ground   = new MeshObjectFromObj("data/models/TerrainSimple", "Terrain.obj", "Terrain.mtl", 4.0f);
        m_Lorry    = new MeshObjectFromObj("data/models/Camion", "camion.obj", "camion.mtl", 2.0f);
        m_PalmTree = new MeshObjectFromObj("data/models/Palm_Tree", "Palm_Tree.obj", "Palm_Tree.mtl", 0.3f);

        // définir une lampe ambiante
        m_Light0 = new Light();
        m_Light0.setColor(vec3.fromValues(0.4,0.4,0.4));

        // définir une lampe spot qui projette de l'ombre
        m_Light1 = new SpotLightDebug(20.0f, 15.0f, 512, 16, 25, true, GL_BACK);
        m_Light1.setPosition(vec4.fromValues(15, 15, 0, 1));
        m_Light1.setTarget(vec4.fromValues(0, 0, 0, 1));
        m_Light1.setColor(vec3.fromValues(400,400,400));

        // définir une lampe spot qui projette de l'ombre
        m_Light2 = new SpotLightDebug(20.0f, 15.0f, 512, 25, 45, true, GL_BACK);
        m_Light2.setPosition(vec4.fromValues(8, 3, 0, 1));
        m_Light2.setTarget(vec4.fromValues(0, 0, 0, 1));
        m_Light2.setColor(vec3.fromValues(600,600,600));

        // configurer les modes de dessin
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        // couleur du fond
        glClearColor(0.4f, 0.5f, 0.6f, 0.0f);

        // paramètres de la vue
        m_CameraAngleAzimut = 60.0f;
        m_CameraAngleElevation = 30.0f;
        m_CameraDistance = 35.0f;
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
        // matrice temporaire
        mat4 mat4ModelView = mat4.create();

        // dessiner le terrain
        m_Ground.onDraw(mat4Projection, mat4View);

        // dessiner les palmiers
        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(2.5,0,-1.0));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(120.0));
        m_PalmTree.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(3.0,0,1.5));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(20.0));
        m_PalmTree.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(-1.0,0.0,-2.0));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(80.0));
        m_PalmTree.onDraw(mat4Projection, mat4ModelView);

        // dessiner les camions
        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(-2.0,0.0,0.5));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(120.0));
        m_Lorry.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(1.0,0.0,1.0));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(-20.0));
        m_Lorry.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(-1.0,0.0,2.0));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(200.0));
        m_Lorry.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Dessine l'image courante
     */
    @Override
    public void onDrawFrame()
    {
        // effacer l'écran
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // construire la matrice caméra
        mat4 mat4ViewCamera = mat4.create();

        // la position de la lampe2 est relative à la caméra
        m_Light2.transformPosition(mat4ViewCamera);

        // éloignement de la scène
        mat4.translate(mat4ViewCamera, mat4ViewCamera, vec3.fromValues(0.0, 0.0, -m_CameraDistance));

        // orientation de la scène par rapport à la caméra
        mat4.rotateX(mat4ViewCamera, mat4ViewCamera, Utils.radians(m_CameraAngleElevation));
        mat4.rotateY(mat4ViewCamera, mat4ViewCamera, Utils.radians(m_CameraAngleAzimut));

        // définir le pivot des rotations
        mat4.translate(mat4ViewCamera, mat4ViewCamera, m_CameraPivot);

        // rendre la lampe 1 fixe par rapport à la scène
        m_Light1.transform(mat4ViewCamera);

        // la cible de la lampe 2 est relative à la scène
        m_Light2.transformTarget(mat4ViewCamera);

        // calculer les shadow maps des lampes
        m_Light1.makeShadowMap(this, mat4ViewCamera);
        m_Light2.makeShadowMap(this, mat4ViewCamera);

        // mise au point : décommenter l'une des lignes
        //m_Light1.onDrawShadowMap();/*
        //m_Light2.onDrawShadowMap();/*

        // dessiner les objets dans cette transformation
        drawDeferredShading(m_Mat4Projection, mat4ViewCamera);

        // dessiner l'éclairement de la première lampe
        addLighting(m_Light0, true);

        // rajouter les éclairements des lampes suivantes
        addLighting(m_Light1);
        addLighting(m_Light2);

        // dessiner les lampes
        //m_Light1.onDraw(m_Mat4Projection, mat4ViewCamera);
        //m_Light2.onDraw(m_Mat4Projection, mat4ViewCamera);

        //*/
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    @Override
    public void destroy()
    {
        m_Light2.destroy();
        m_Light1.destroy();
        m_Light0.destroy();
        m_PalmTree.destroy();
        m_Lorry.destroy();
        m_Ground.destroy();
        super.destroy();
    }
}
