// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Misc/TurnTableScene.js");
Requires("libs/Material/OmniLight.js");
Requires("libs/Misc/SceneElement.js");
Requires("libs/Mesh/MeshObjectFromObj.js");


class Scene extends TurnTableScene
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(true);

        // créer les types d'objets à dessiner
        this.m_Ground   = new MeshObjectFromObj("data/models/TerrainSimple", "Terrain.obj", "Terrain.mtl", 4.0);
        this.m_Bee      = new MeshObjectFromObj("data/models/Bee", "bee.obj", "bee.mtl", 20.0);
        this.m_Lorry    = new MeshObjectFromObj("data/models/Camion", "camion.obj", "camion.mtl", 2.0);
        this.m_PalmTree = new MeshObjectFromObj("data/models/Palm_Tree", "Palm_Tree.obj", "Palm_Tree.mtl", 0.4);

        // définir les éléments de la scène et leur hiérarchie
        this.m_ElementGround    = new SceneElement(this.m_Ground, null);
        this.m_ElementLorry     = new SceneElement(this.m_Lorry,  this.m_ElementGround);
        this.m_ElementBee       = new SceneElement(this.m_Bee, this.m_ElementLorry);
        this.m_ElementPalmTree1 = new SceneElement(this.m_PalmTree, this.m_ElementGround);
        this.m_ElementPalmTree2 = new SceneElement(this.m_PalmTree, this.m_ElementGround);
        this.m_ElementPalmTree3 = new SceneElement(this.m_PalmTree, this.m_ElementGround);
        this.m_ElementPalmTree4 = new SceneElement(this.m_PalmTree, this.m_ElementGround);
        this.m_ElementPalmTree5 = new SceneElement(this.m_PalmTree, this.m_ElementGround);

        // définir la transformation du palmier n°1 par rapport à son parent
        this.m_ElementPalmTree1.translate(vec3.fromValues(2.5,0,-1.0));
        this.m_ElementPalmTree1.rotateY(Utils.radians(120.0));

        // définir la transformation du palmier n°2 par rapport à son parent
        this.m_ElementPalmTree2.translate(vec3.fromValues(-2.0,0,-3.0));

        // définir la transformation du palmier n°3 par rapport à son parent
        this.m_ElementPalmTree3.translate(vec3.fromValues(-1.5,0,1.5));
        this.m_ElementPalmTree3.rotateY(Utils.radians(30.0));

        // définir la transformation du palmier n°4 par rapport à son parent
        this.m_ElementPalmTree4.translate(vec3.fromValues(0.5,0,0.5));
        this.m_ElementPalmTree4.rotateY(Utils.radians(270.0));

        // définir la transformation du palmier n°5 par rapport à son parent
        this.m_ElementPalmTree5.translate(vec3.fromValues(2.5,0,2.0));

        // définir une lampe directionnelle au dessus du terrain
        this.m_Light0 = new OmniLight();
        this.m_Light0.setPosition(vec4.fromValues(-1.0, 2.0, 0.0, 0.0));
        this.m_Light0.setColor(vec3.fromValues(0.8,0.8,0.8));
        this.m_ElementLight0 = new SceneElement(this.m_Light0, this.m_ElementGround);
        this.addLight(this.m_Light0);

        // définir une lampe positionnelle placée devant le camion
        this.m_Light1 = new OmniLight();
        this.m_Light1.setPosition(vec4.fromValues(0.0, 0.5, 1.5, 1));
        this.m_Light1.setColor(vec3.fromValues(0.5,0.5,0.5));
        this.m_ElementLight1 = new SceneElement(this.m_Light1, this.m_ElementLorry);
        this.addLight(this.m_Light1);

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.enable(gl.CULL_FACE);

        // couleur du fond
        gl.clearColor(0.4, 0.5, 0.6, 0.0);

        // paramètres de la vue
        this.m_CameraAngleAzimut = 30.0;
        this.m_CameraAngleElevation = 20.0;
        this.m_CameraDistance = 20;

        /// élément attaché à la caméra, décommenter une seule ligne
        //this.ELEMENT_CAMERA = this.m_ElementGround;
        this.ELEMENT_CAMERA = this.m_ElementLorry;
        //this.ELEMENT_CAMERA = this.m_ElementBee;
        //this.ELEMENT_CAMERA = this.m_ElementPalmTree1;
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
        // définir la transformation du camion par rapport à son parent
        this.m_ElementLorry.identity();
        this.m_ElementLorry.translate(vec3.fromValues(-1.0,0,0.5));
        this.m_ElementLorry.rotateY(Utils.radians(Utils.Time * 30.0 + 180.0));
        this.m_ElementLorry.translate(vec3.fromValues(-2.5,0,0.0));

        // définir la transformation de l'abeille par rapport à son parent
        this.m_ElementBee.identity();
        this.m_ElementBee.rotateX(Utils.radians(30.0)); // son plan de rotation est incliné de 30°
        this.m_ElementBee.rotateY(Utils.radians(Utils.Time * 70.0 + 60.0));
        this.m_ElementBee.translate(vec3.fromValues(-1.0,1.0,0.0));

        // dessiner l'élément attaché à la caméra , il dessinera les autres objets
        this.ELEMENT_CAMERA.onDraw(mat4Projection, mat4View);
    }


    /**
     * dessin de la scène sur l'écran
     */
    onDrawFrame()
    {
        // effacer l'écran
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        // construire la matrice MV
        let mat4ModelView = this.getModelView();

        // positionner les lampes
        this.ELEMENT_CAMERA.transform(mat4ModelView);

        // dessiner les shadow map des lampes s'il y en a
        this.makeShadowMaps(mat4ModelView);

        // dessiner les objets dans le FBO mrt (ou sur l'écran)
        this.drawDeferredShading(this.m_Mat4Projection, mat4ModelView);

        // ajouter les éclairements des lampes
        this.addLightings();
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_ElementLight1.destroy();
        this.m_ElementLight0.destroy();
        this.m_ElementPalmTree5.destroy();
        this.m_ElementPalmTree4.destroy();
        this.m_ElementPalmTree3.destroy();
        this.m_ElementPalmTree2.destroy();
        this.m_ElementPalmTree1.destroy();
        this.m_ElementBee.destroy();
        this.m_ElementLorry.destroy();
        this.m_ElementGround.destroy();
        this.m_Light1.destroy();
        this.m_Light0.destroy();
        this.m_PalmTree.destroy();
        this.m_Lorry.destroy();
        this.m_Bee.destroy();
        this.m_Ground.destroy();
        super.destroy();
    }
}
