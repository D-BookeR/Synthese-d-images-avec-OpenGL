// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Misc/TurnTableScene.js");
Requires("libs/Misc/MeshObjectFromObj.js");
Requires("libs/Material/Texture2D.js");
Requires("libs/Material/OmniLight.js");
Requires("Billboard");


class Scene extends TurnTableScene
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(true);

        // crée les objets à dessiner
        this.m_Ground   = new MeshObjectFromObj("data/models/TerrainSimple", "Terrain.obj", "Terrain.mtl", 3.0);
        this.m_DeadTree = new MeshObjectFromObj("data/models/DeadTrees", "deadtree1.obj", "deadtree1.mtl", 3.0);

        // créer le matériau du rectangle
        this.m_Texture = new Texture2D("data/textures/Multiples/feu.png");
        this.m_Material = new MultipleTextureMaterial(this.m_Texture, 16, 4);

        // créer un billboard à partir de la texture du feu
        this.m_Fire = new Billboard(this.m_Material, 0.5, 1.0);

        // définir une lampe positionnelle
        this.m_Light0 = new OmniLight();
        this.m_Light0.setPosition(vec4.fromValues(0.0, 0.6, 0.0, 1.0));
        this.m_Light0.setColor(vec3.fromValues(0.5,0.5,0.5));
        this.addLight(this.m_Light0);

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.disable(gl.CULL_FACE);      // laisser voir le dos des billboards

        // formule de blending
        gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);

        // couleur du fond
        gl.clearColor(0.3, 0.4, 0.5, 0.0);

        // paramètres de la vue
        this.m_CameraAngleAzimut = 40.0;
        this.m_CameraAngleElevation = 0.5;
        this.m_CameraDistance = 10;
        vec3.set(this.m_CameraPivot, -0.2, -0.6, -0.2);
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
        mat4.perspective(this.m_Mat4Projection, Utils.radians(10.0), width / height, 0.1, 40.0);
    }


    /**
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4View : matrice de vue
     */
    onDraw(mat4Projection, mat4View)
    {
        // modifier l'intensité de la lampe d'une manière irrégulière
        let k = 0.1*Math.sin(Utils.Time*23.0)*Math.sin(Utils.Time*17.0)+0.7;
        this.m_Light0.setColor(vec3.fromValues(k,k,k));

        // transformation des objets
        let mat4ModelView = mat4.create();

        // dessiner le terrain
        this.m_Ground.onDraw(mat4Projection, mat4View);

        // dessiner l'arbre
        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(1.5, 0.1, -1.0));
        this.m_DeadTree.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * dessin de la scène sur l'écran
     */
    onDrawFrame()
    {
        // appeler la méthode de la superclasse, elle dessine tout
        super.onDrawFrame();

        // construire la matrice MV
        let mat4ModelView = this.getModelView();

        // dessiner le feu par dessus le tout
        this.m_Fire.onDraw(this.m_Mat4Projection, mat4ModelView);
    }

    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Light0.destroy();
        this.m_Fire.destroy();
        this.m_Material.destroy();
        this.m_Texture.destroy();
        this.m_Ground.destroy();
        super.destroy();
    }
}
