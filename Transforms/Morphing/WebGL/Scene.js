// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Misc/TurnTableScene.js");
Requires("libs/Mesh/MeshObjectFromObj.js");
Requires("libs/Misc/SceneElement.js");
Requires("libs/Material/Light.js");
Requires("libs/Material/OmniLightDebug.js");
Requires("libs/Material/SpotLight.js");
Requires("libs/Material/SoftSpotLight.js");
Requires("Pumpkin");


class Scene extends TurnTableScene
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(true);

        // créer les objets à dessiner
        this.m_Ground   = new MeshObjectFromObj("data/models/TerrainSimple", "Terrain.obj", "Terrain.mtl", 3.0);
        this.m_DeadTree = new MeshObjectFromObj("data/models/DeadTrees", "deadtree1.obj", "deadtree1.mtl", 3.0);
        this.m_Pumpkin1 = new Pumpkin();
        this.m_Pumpkin2 = new Pumpkin();
        this.m_Pumpkin3 = new Pumpkin();

        // définir les éléments de la scène et leur hiérarchie
        this.m_GroundElement = new SceneElement(this.m_Ground, null);
        this.m_DeadTreeElement1 = new SceneElement(this.m_DeadTree, this.m_GroundElement);
        this.m_DeadTreeElement2 = new SceneElement(this.m_DeadTree, this.m_GroundElement);
        this.m_DeadTreeElement3 = new SceneElement(this.m_DeadTree, this.m_GroundElement);

        // arbre n°1
        this.m_DeadTreeElement1.translate(vec3.fromValues(2.5,0,-1.0));
        this.m_DeadTreeElement1.rotateY(Utils.radians(120.0));

        // arbre n°2
        this.m_DeadTreeElement2.translate(vec3.fromValues(1.0,0,1.5));
        this.m_DeadTreeElement2.rotateY(Utils.radians(20.0));

        // arbre n°3
        this.m_DeadTreeElement3.translate(vec3.fromValues(-1.0,0.0,-2.0));
        this.m_DeadTreeElement3.rotateY(Utils.radians(80.0));

        // citrouille n°1
        this.m_PumpkinElement1 = new SceneElement(this.m_Pumpkin1, this.m_GroundElement);
        this.m_PumpkinElement1.translate(vec3.fromValues(-2.0,0.0,0.5));
        this.m_PumpkinElement1.rotateY(Utils.radians(60.0));

        // définir une lampe spot à l'intérieur de la citrouille n°1
        this.m_PumpkinSpotLight1 = new SpotLight(90.0, 75.0, 512, 0.2, 7.0);
        this.m_PumpkinSpotLight1.setColor(vec3.fromValues(10,7,5));
        this.m_PumpkinSpotLight1.setPosition(vec4.fromValues(0.0,0.3,0.0, 1));
        this.m_PumpkinSpotLight1.setTarget(vec4.fromValues(0.0,0.3,1.0, 1));
        this.addLight(this.m_PumpkinSpotLight1);
        this.m_PumpkinSpotLightElement1 = new SceneElement(this.m_PumpkinSpotLight1, this.m_PumpkinElement1);

        // définir une lampe omni à l'intérieur de la citrouille n°1
        this.m_PumpkinOmniLight1 = new OmniLight();
        this.m_PumpkinOmniLight1.setColor(vec3.fromValues(0.05,0.025,0.0));
        this.m_PumpkinOmniLight1.setPosition(vec4.fromValues(0.0,0.3,0.0, 1));
        this.addLight(this.m_PumpkinOmniLight1);
        this.m_PumpkinOmniLightElement1 = new SceneElement(this.m_PumpkinOmniLight1, this.m_PumpkinElement1);

        // citrouille n°2
        this.m_PumpkinElement2 = new SceneElement(this.m_Pumpkin2, this.m_GroundElement);
        this.m_PumpkinElement2.translate(vec3.fromValues(-1.0,0.0,2.0));
        this.m_PumpkinElement2.rotateY(Utils.radians(160.0));

        // définir une lampe spot à l'intérieur de la citrouille n°2
        this.m_PumpkinSpotLight2 = new SpotLight(90.0, 75.0, 512, 0.2, 7.0);
        this.m_PumpkinSpotLight2.setColor(vec3.fromValues(10,7,5));
        this.m_PumpkinSpotLight2.setPosition(vec4.fromValues(0.0,0.3,0.0, 1));
        this.m_PumpkinSpotLight2.setTarget(vec4.fromValues(0.0,0.3,1.0, 1));
        this.addLight(this.m_PumpkinSpotLight2);
        this.m_PumpkinSpotLightElement2 = new SceneElement(this.m_PumpkinSpotLight2, this.m_PumpkinElement2);

        // définir une lampe omni à l'intérieur de la citrouille n°2
        this.m_PumpkinOmniLight2 = new OmniLight();
        this.m_PumpkinOmniLight2.setColor(vec3.fromValues(0.05,0.025,0.0));
        this.m_PumpkinOmniLight2.setPosition(vec4.fromValues(0.0,0.3,0.0, 1));
        this.addLight(this.m_PumpkinOmniLight2);
        this.m_PumpkinOmniLightElement2 = new SceneElement(this.m_PumpkinOmniLight2, this.m_PumpkinElement2);

        // citrouille n°3
        this.m_PumpkinElement3 = new SceneElement(this.m_Pumpkin3, this.m_GroundElement);
        this.m_PumpkinElement3.translate(vec3.fromValues(1.0,0.0,-1.0));
        this.m_PumpkinElement3.rotateY(Utils.radians(-50.0));

        // définir une lampe spot à l'intérieur de la citrouille n°3
        this.m_PumpkinSpotLight3 = new SpotLight(90.0, 75.0, 512, 0.2, 7.0);
        this.m_PumpkinSpotLight3.setColor(vec3.fromValues(10,7,5));
        this.m_PumpkinSpotLight3.setPosition(vec4.fromValues(0.0,0.3,0.0, 1));
        this.m_PumpkinSpotLight3.setTarget(vec4.fromValues(0.0,0.3,1.0, 1));
        this.addLight(this.m_PumpkinSpotLight3);
        this.m_PumpkinSpotLightElement3 = new SceneElement(this.m_PumpkinSpotLight3, this.m_PumpkinElement3);

        // définir une lampe omni à l'intérieur de la citrouille n°3
        this.m_PumpkinOmniLight3 = new OmniLight();
        this.m_PumpkinOmniLight3.setColor(vec3.fromValues(0.05,0.025,0.0));
        this.m_PumpkinOmniLight3.setPosition(vec4.fromValues(0.0,0.3,0.0, 1));
        this.addLight(this.m_PumpkinOmniLight3);
        this.m_PumpkinOmniLightElement3 = new SceneElement(this.m_PumpkinOmniLight3, this.m_PumpkinElement3);

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.enable(gl.CULL_FACE);

        // couleur du fond
        gl.clearColor(0.1, 0.2, 0.3, 0.0);

        // paramètres de la vue
        this.m_CameraAngleAzimut = 20.0;
        this.m_CameraAngleElevation = 15.0;
        this.m_CameraDistance = 20;
        vec3.set(this.m_CameraPivot, -0.1, -0.7, 0.3);
    }


    /**
     * appelée quand la taille de la vue OpenGL change
     * @param width : largeur en nombre de pixels de la fenêtre
     * @param height : hauteur en nombre de pixels de la fenêtre
     */
    onSurfaceChanged(width, height)
    {
        // appeler la méthode de la superclasse
        super.onSurfaceChanged(width, height);

        // matrice de projection (champ de vision)
        mat4.perspective(this.m_Mat4Projection, Utils.radians(12.0), width / height, 1.0, 100.0);
    }


    /**
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4View : matrice de vue
     */
    onDraw(mat4Projection, mat4View)
    {
        // modifier les citrouilles
        this.m_Pumpkin1.setMorphCoef(Math.pow(0.5*Math.sin(Utils.Time * 10.0)+0.5, 2.5));
        this.m_Pumpkin2.setMorphCoef(0.5*Math.sin(Utils.Time * 12.0)+0.5);
        this.m_Pumpkin3.setMorphCoef(Math.pow(0.5*Math.sin(Utils.Time * 15.0)+0.5, 0.5));

        // dessiner le terrain, les palmiers, les citrouilles et les lampes
        this.m_GroundElement.onDraw(mat4Projection, mat4View);
    }


    /**
     * dessine les objets de la scène
     */
    onDrawFrame()
    {
        // effacer l'écran
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        // construire la matrice caméra
        let mat4ViewCamera = this.getModelView();

        // positionner les lampes
        this.m_GroundElement.transform(mat4ViewCamera);

        // préparer les shadow maps (s'il y en a)
        this.makeShadowMaps(mat4ViewCamera);

        // dessiner les objets dans cette transformation
        this.drawDeferredShading(this.m_Mat4Projection, mat4ViewCamera);

        // rajouter les éclairements des lampes
        this.addLightings();
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_PumpkinOmniLightElement3.destroy();
        this.m_PumpkinSpotLightElement3.destroy();
        this.m_PumpkinElement3.destroy();
        this.m_PumpkinOmniLightElement2.destroy();
        this.m_PumpkinSpotLightElement2.destroy();
        this.m_PumpkinElement2.destroy();
        this.m_PumpkinOmniLightElement1.destroy();
        this.m_PumpkinSpotLightElement1.destroy();
        this.m_PumpkinElement1.destroy();
        this.m_DeadTreeElement3.destroy();
        this.m_DeadTreeElement2.destroy();
        this.m_DeadTreeElement1.destroy();
        this.m_GroundElement.destroy();
        this.m_PumpkinOmniLight3.destroy();
        this.m_PumpkinSpotLight3.destroy();
        this.m_PumpkinOmniLight2.destroy();
        this.m_PumpkinSpotLight2.destroy();
        this.m_PumpkinOmniLight1.destroy();
        this.m_PumpkinSpotLight1.destroy();
        this.m_Pumpkin3.destroy();
        this.m_Pumpkin2.destroy();
        this.m_Pumpkin1.destroy();
        this.m_DeadTree.destroy();
        this.m_Ground.destroy();
        super.destroy();
    }
}
