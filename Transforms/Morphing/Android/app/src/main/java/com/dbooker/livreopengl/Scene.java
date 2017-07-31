package com.dbooker.livreopengl;

import static android.opengl.GLES20.*;

import com.dbooker.livreopengl.mesh.MeshObject;
import com.dbooker.livreopengl.mesh.MeshObjectFromObj;
import com.dbooker.livreopengl.misc.SceneElement;
import com.dbooker.livreopengl.misc.TurnTableScene;
import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.glmatrix.vec4;
import com.dbooker.livreopengl.material.OmniLight;
import com.dbooker.livreopengl.material.SpotLight;
import com.dbooker.livreopengl.utils.Utils;

public class Scene extends TurnTableScene
{
    // objets de la scène
    private MeshObject m_Ground;
    private MeshObject m_DeadTree;
    private Pumpkin m_Pumpkin1;
    private Pumpkin m_Pumpkin2;
    private Pumpkin m_Pumpkin3;

    // lampes de la scène
    private SpotLight m_PumpkinSpotLight1;
    private OmniLight m_PumpkinOmniLight1;
    private SpotLight m_PumpkinSpotLight2;
    private OmniLight m_PumpkinOmniLight2;
    private SpotLight m_PumpkinSpotLight3;
    private OmniLight m_PumpkinOmniLight3;

    // éléments 3D pour former une hiérarchie
    SceneElement m_GroundElement;
    SceneElement m_DeadTreeElement1;
    SceneElement m_DeadTreeElement2;
    SceneElement m_DeadTreeElement3;
    SceneElement m_PumpkinElement1;
    SceneElement m_PumpkinElement2;
    SceneElement m_PumpkinElement3;
    SceneElement m_PumpkinSpotLightElement1;
    SceneElement m_PumpkinOmniLightElement1;
    SceneElement m_PumpkinSpotLightElement2;
    SceneElement m_PumpkinOmniLightElement2;
    SceneElement m_PumpkinSpotLightElement3;
    SceneElement m_PumpkinOmniLightElement3;



    /**
     * Constructeur
     */
    public Scene() throws Exception
    {
        super(true);

        // créer les objets à dessiner
        m_Ground   = new MeshObjectFromObj("data/models/TerrainSimple", "Terrain.obj", "Terrain.mtl", 3.0f);
        m_DeadTree = new MeshObjectFromObj("data/models/DeadTrees", "deadtree1.obj", "deadtree1.mtl", 3.0f);
        m_Pumpkin1 = new Pumpkin();
        m_Pumpkin2 = new Pumpkin();
        m_Pumpkin3 = new Pumpkin();

        // définir les éléments de la scène et leur hiérarchie
        m_GroundElement = new SceneElement(m_Ground, null);
        m_DeadTreeElement1 = new SceneElement(m_DeadTree, m_GroundElement);
        m_DeadTreeElement2 = new SceneElement(m_DeadTree, m_GroundElement);
        m_DeadTreeElement3 = new SceneElement(m_DeadTree, m_GroundElement);

        // arbre n°1
        m_DeadTreeElement1.translate(vec3.fromValues(2.5,0,-1.0));
        m_DeadTreeElement1.rotateY(Utils.radians(120.0));

        // arbre n°2
        m_DeadTreeElement2.translate(vec3.fromValues(1.0,0,1.5));
        m_DeadTreeElement2.rotateY(Utils.radians(20.0));

        // arbre n°3
        m_DeadTreeElement3.translate(vec3.fromValues(-1.0,0.0,-2.0));
        m_DeadTreeElement3.rotateY(Utils.radians(80.0));

        // citrouille n°1
        m_PumpkinElement1 = new SceneElement(m_Pumpkin1, m_GroundElement);
        m_PumpkinElement1.translate(vec3.fromValues(-2.0,0.0,0.5));
        m_PumpkinElement1.rotateY(Utils.radians(60.0));

        // définir une lampe spot à l'intérieur de la citrouille n°1
        m_PumpkinSpotLight1 = new SpotLight(90.0f, 75.0f, 512, 0.2f, 7.0f);
        m_PumpkinSpotLight1.setColor(vec3.fromValues(10,7,5));
        m_PumpkinSpotLight1.setPosition(vec4.fromValues(0.0,0.3,0.0, 1));
        m_PumpkinSpotLight1.setTarget(vec4.fromValues(0.0,0.3,1.0, 1));
        addLight(m_PumpkinSpotLight1);
        m_PumpkinSpotLightElement1 = new SceneElement(m_PumpkinSpotLight1, m_PumpkinElement1);

        // définir une lampe omni à l'intérieur de la citrouille n°1
        m_PumpkinOmniLight1 = new OmniLight();
        m_PumpkinOmniLight1.setColor(vec3.fromValues(0.05,0.025,0.0));
        m_PumpkinOmniLight1.setPosition(vec4.fromValues(0.0,0.3,0.0, 1));
        addLight(m_PumpkinOmniLight1);
        m_PumpkinOmniLightElement1 = new SceneElement(m_PumpkinOmniLight1, m_PumpkinElement1);

        // citrouille n°2
        m_PumpkinElement2 = new SceneElement(m_Pumpkin2, m_GroundElement);
        m_PumpkinElement2.translate(vec3.fromValues(-1.0,0.0,2.0));
        m_PumpkinElement2.rotateY(Utils.radians(160.0));

        // définir une lampe spot à l'intérieur de la citrouille n°2
        m_PumpkinSpotLight2 = new SpotLight(90.0f, 75.0f, 512, 0.2f, 7.0f);
        m_PumpkinSpotLight2.setColor(vec3.fromValues(10,7,5));
        m_PumpkinSpotLight2.setPosition(vec4.fromValues(0.0,0.3,0.0, 1));
        m_PumpkinSpotLight2.setTarget(vec4.fromValues(0.0,0.3,1.0, 1));
        addLight(m_PumpkinSpotLight2);
        m_PumpkinSpotLightElement2 = new SceneElement(m_PumpkinSpotLight2, m_PumpkinElement2);

        // définir une lampe omni à l'intérieur de la citrouille n°2
        m_PumpkinOmniLight2 = new OmniLight();
        m_PumpkinOmniLight2.setColor(vec3.fromValues(0.05,0.025,0.0));
        m_PumpkinOmniLight2.setPosition(vec4.fromValues(0.0,0.3,0.0, 1));
        addLight(m_PumpkinOmniLight2);
        m_PumpkinOmniLightElement2 = new SceneElement(m_PumpkinOmniLight2, m_PumpkinElement2);

        // citrouille n°3
        m_PumpkinElement3 = new SceneElement(m_Pumpkin3, m_GroundElement);
        m_PumpkinElement3.translate(vec3.fromValues(1.0,0.0,-1.0));
        m_PumpkinElement3.rotateY(Utils.radians(-50.0));

        // définir une lampe spot à l'intérieur de la citrouille n°3
        m_PumpkinSpotLight3 = new SpotLight(90.0f, 75.0f, 512, 0.2f, 7.0f);
        m_PumpkinSpotLight3.setColor(vec3.fromValues(10,7,5));
        m_PumpkinSpotLight3.setPosition(vec4.fromValues(0.0,0.3,0.0, 1));
        m_PumpkinSpotLight3.setTarget(vec4.fromValues(0.0,0.3,1.0, 1));
        addLight(m_PumpkinSpotLight3);
        m_PumpkinSpotLightElement3 = new SceneElement(m_PumpkinSpotLight3, m_PumpkinElement3);

        // définir une lampe omni à l'intérieur de la citrouille n°3
        m_PumpkinOmniLight3 = new OmniLight();
        m_PumpkinOmniLight3.setColor(vec3.fromValues(0.05,0.025,0.0));
        m_PumpkinOmniLight3.setPosition(vec4.fromValues(0.0,0.3,0.0, 1));
        addLight(m_PumpkinOmniLight3);
        m_PumpkinOmniLightElement3 = new SceneElement(m_PumpkinOmniLight3, m_PumpkinElement3);

            // configurer les modes de dessin
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        // couleur du fond
        glClearColor(0.1f, 0.2f, 0.3f, 0.0f);

        // paramètres de la vue
        m_CameraAngleAzimut = 20.0f;
        m_CameraAngleElevation = 15.0f;
        m_CameraDistance = 20f;
        vec3.set(m_CameraPivot, -0.1f, -0.7f, 0.3f);
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
        // modifier les citrouilles
        m_Pumpkin1.setMorphCoef((float)(Math.pow(0.5*Math.sin(Utils.Time * 10.0)+0.5, 2.5)));
        m_Pumpkin2.setMorphCoef((float)(0.5*Math.sin(Utils.Time * 12.0)+0.5));
        m_Pumpkin3.setMorphCoef((float)(Math.pow(0.5*Math.sin(Utils.Time * 15.0)+0.5, 0.5)));

        // dessiner le terrain, les palmiers, les citrouilles et les lampes
        m_GroundElement.onDraw(mat4Projection, mat4View);
    }


    /**
     * dessine les objets de la scène
     */
    public void onDrawFrame()
    {
        // effacer l'écran
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // construire la matrice caméra
        mat4 mat4ViewCamera = getModelView();

        // positionner les lampes
        m_GroundElement.transform(mat4ViewCamera);

        // préparer les shadow maps (s'il y en a)
        makeShadowMaps(mat4ViewCamera);

        // dessiner les objets dans cette transformation
        drawDeferredShading(m_Mat4Projection, mat4ViewCamera);

        // rajouter les éclairements des lampes
        addLightings();
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    @Override
    public void destroy()
    {
        m_PumpkinOmniLightElement3.destroy();
        m_PumpkinSpotLightElement3.destroy();
        m_PumpkinElement3.destroy();
        m_PumpkinOmniLightElement2.destroy();
        m_PumpkinSpotLightElement2.destroy();
        m_PumpkinElement2.destroy();
        m_PumpkinOmniLightElement1.destroy();
        m_PumpkinSpotLightElement1.destroy();
        m_PumpkinElement1.destroy();
        m_DeadTreeElement3.destroy();
        m_DeadTreeElement2.destroy();
        m_DeadTreeElement1.destroy();
        m_GroundElement.destroy();
        m_PumpkinOmniLight3.destroy();
        m_PumpkinSpotLight3.destroy();
        m_PumpkinOmniLight2.destroy();
        m_PumpkinSpotLight2.destroy();
        m_PumpkinOmniLight1.destroy();
        m_PumpkinSpotLight1.destroy();
        m_Pumpkin3.destroy();
        m_Pumpkin2.destroy();
        m_Pumpkin1.destroy();
        m_DeadTree.destroy();
        m_Ground.destroy();
        super.destroy();
    }
}
