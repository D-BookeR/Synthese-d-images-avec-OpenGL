// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Misc/TurnTableScene.js");
Requires("libs/Misc/MeshObjectFromObj.js");
Requires("libs/Material/OmniLight.js");


class Scene extends TurnTableScene
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(true);

        // créer les objets à dessiner
        this.m_Ground   = new MeshObjectFromObj("data/models/TerrainSimple", "Terrain.obj", "Terrain.mtl", 4.0);
        this.m_Lorry    = new MeshObjectFromObj("data/models/Camion", "camion.obj", "camion.mtl", 2.0);
        this.m_PalmTree = new MeshObjectFromObj("data/models/Palm_Tree", "Palm_Tree.obj", "Palm_Tree.mtl", 0.3);

        // définir une lampe
        this.m_Light = new OmniLightDebug();
        this.m_Light.setPosition(vec4.fromValues(5, 5, 0, 1));
        this.m_Light.setColor(vec3.fromValues(60,60,60));

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.enable(gl.CULL_FACE);

        // couleur du fond
        gl.clearColor(0.4, 0.5, 0.6, 0.0);

        // paramètres de la vue
        this.m_CameraAngleAzimut = 10.0;
        this.m_CameraAngleElevation = 35.0;
        vec3.set(this.m_CameraPivot, 0.0,-2.5, 0.0);
        this.m_CameraDistance = 47;
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
        // matrice temporaire
        let mat4ModelView = mat4.create();

        // dessiner le terrain
        this.m_Ground.onDraw(mat4Projection, mat4View);

        // dessiner les palmiers
        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(2.5,0,-1.0));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(120.0));
        this.m_PalmTree.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(3.0,0,1.5));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(20.0));
        this.m_PalmTree.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(-1.0,0.0,-2.0));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(80.0));
        this.m_PalmTree.onDraw(mat4Projection, mat4ModelView);


        // dessiner les camions
        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(-2.0,0.0,0.5));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(120.0));
        this.m_Lorry.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(1.0,0.0,1.0));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(-20.0));
        this.m_Lorry.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(-1.0,0.0,2.0));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(200.0));
        this.m_Lorry.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Dessine l'image courante
     */
    onDrawFrame()
    {
        // effacer l'écran
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        // construire la matrice caméra
        let mat4View = this.getModelView();

        // faire tourner la lampe autour de la scène
        let mat4Light = mat4.create();
        mat4.rotateY(mat4Light, mat4View, Utils.Time*0.5);

        // rendre la lampe fixe par rapport au pivot des rotations
        this.m_Light.transform(mat4Light);

        // dessiner les objets dans cette transformation
        this.drawDeferredShading(this.m_Mat4Projection, mat4View);

        // dessiner l'éclairement de la lampe
        this.addLighting(this.m_Light, true);

        // dessiner la lampe
        this.m_Light.onDraw(this.m_Mat4Projection, mat4View);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Light.destroy();
        this.m_PalmTree.destroy();
        this.m_Lorry.destroy();
        this.m_Ground.destroy();
        super.destroy();
    }
}
