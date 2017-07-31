package com.dbooker.livreopengl;

import static android.opengl.GLES20.*;

import com.dbooker.livreopengl.mesh.MeshObject;
import com.dbooker.livreopengl.mesh.MeshObjectFromObj;
import com.dbooker.livreopengl.misc.TurnTableScene;
import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.glmatrix.vec4;
import com.dbooker.livreopengl.material.OmniLight;
import com.dbooker.livreopengl.utils.Utils;

public class Scene extends TurnTableScene
{
    // objets de la scène
    private MeshObject m_Ground;
    private MeshObject m_Lorry;
    private MeshObject m_PalmTree;

    private OmniLight m_Light0;
    private OmniLight m_Light1;


    /** créer les objets et initialise OpenGL */
    public Scene() throws Exception
    {
        super(true);

        // créer les objets à dessiner
        m_Ground   = new MeshObjectFromObj("data/models/TerrainSimple", "Terrain.obj", "Terrain.mtl", 2.0f);
        m_Lorry    = new MeshObjectFromObj("data/models/Camion", "camion.obj", "camion.mtl", 2.0f);
        m_PalmTree = new MeshObjectFromObj("data/models/Palm_Tree", "Palm_Tree.obj", "Palm_Tree.mtl", 2.0f);

        // définir une lampe directionnelle
        m_Light0 = new OmniLight();
        m_Light0.setPosition(vec4.fromValues(-1, 2, 2, 0));
        m_Light0.setColor(vec3.fromValues(0.8,0.8,0.8));
        addLight(m_Light0);

        // définir une lampe positionnelle
        m_Light1 = new OmniLight();
        m_Light1.setPosition(vec4.fromValues(-1, 5, -1, 1));
        m_Light1.setColor(vec3.fromValues(20,20,20));
        addLight(m_Light1);

        // configurer les modes de dessin
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        // couleur du fond
        glClearColor(0.4f, 0.5f, 0.6f, 0.0f);

        // paramètres de la vue
        m_CameraAngleAzimut = 60.0f;
        m_CameraAngleElevation = 30.0f;
        m_CameraDistance = 35f;
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
        mat4.perspective(m_Mat4Projection, Utils.radians(12.0), (float)width/height, 1.0f, 100.0f);
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
        mat4.scale(mat4ModelView, mat4View, vec3.fromValues(2.0, 2.0, 2.0));
        this.m_Ground.onDraw(mat4Projection, mat4ModelView);

        // dessiner les palmiers
        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(2.5,0,-1.0));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(120.0));
        mat4.rotateX(mat4ModelView, mat4ModelView, Utils.radians(7.0 * Math.sin(Utils.Time)));
        mat4.scale(mat4ModelView, mat4ModelView, vec3.fromValues(0.2, 0.2, 0.2));
        this.m_PalmTree.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(3.0,0,1.5));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(20.0));
        mat4.rotateX(mat4ModelView, mat4ModelView, Utils.radians(7.0 * Math.sin(Utils.Time)));
        mat4.scale(mat4ModelView, mat4ModelView, vec3.fromValues(0.2, 0.2, 0.2));
        this.m_PalmTree.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(-1,0,-2.0));
        mat4.rotate(mat4ModelView, mat4ModelView, Utils.radians(80.0), vec3.fromValues(-1,0,-1));
        mat4.scale(mat4ModelView, mat4ModelView, vec3.fromValues(0.2, 0.2, 0.2));
        this.m_PalmTree.onDraw(mat4Projection, mat4ModelView);


        // dessiner les camions
        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(1.5,0.5,-1.5));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(-70.0));
        mat4.rotate(mat4ModelView, mat4ModelView, Utils.radians(82.0), vec3.fromValues(0,0,1));
        this.m_Lorry.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(-2,0,0.5));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(120.0));
        this.m_Lorry.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(-1,0,-1));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(320.0));
        mat4.scale(mat4ModelView, mat4ModelView, vec3.fromValues(1.5,3.0,0.5));
        this.m_Lorry.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(1.15,1.0,0.7));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(60.0));
        this.m_Lorry.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(1,0,1));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(-20.0));
        mat4.scale(mat4ModelView, mat4ModelView, vec3.fromValues(2.0, 2.0, 2.0));
        this.m_Lorry.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(-1,0,2));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(200.0));
        this.m_Lorry.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    @Override
    public void destroy()
    {
        m_Light1.destroy();
        m_Light0.destroy();
        m_PalmTree.destroy();
        m_Lorry.destroy();
        m_Ground.destroy();
        super.destroy();
    }
}
