// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Mesh/MeshObjectFromObj");
Requires("libs/Material/OmniLight.js");
Requires("libs/Misc/TurnTableScene.js");


class Scene extends TurnTableScene
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(true);

        // créer les objets à dessiner
        this.m_Ground   = new MeshObjectFromObj("data/models/TerrainSimple", "Terrain.obj", "Terrain.mtl", 2.0);
        this.m_Lorry    = new MeshObjectFromObj("data/models/Camion", "camion.obj", "camion.mtl", 2.0);
        this.m_PalmTree = new MeshObjectFromObj("data/models/Palm_Tree", "Palm_Tree.obj", "Palm_Tree.mtl", 2.0);

        // définir une lampe directionnelle
        this.m_Light0 = new OmniLight();
        this.m_Light0.setPosition(vec4.fromValues(-1, 2, 2, 0));
        this.m_Light0.setColor(vec3.fromValues(0.8,0.8,0.8));
        this.addLight(this.m_Light0);

        // définir une lampe positionnelle
        this.m_Light1 = new OmniLight();
        this.m_Light1.setPosition(vec4.fromValues(-1, 5, -1, 1));
        this.m_Light1.setColor(vec3.fromValues(20,20,20));
        this.addLight(this.m_Light1);

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.enable(gl.CULL_FACE);

        // couleur du fond
        gl.clearColor(0.4, 0.5, 0.6, 0.0);

        // paramètres de la vue
        this.m_CameraAngleAzimut = 60.0;
        this.m_CameraAngleElevation = 30.0;
        this.m_CameraDistance = 35;
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
    destroy()
    {
        this.m_Light1.destroy();
        this.m_Light0.destroy();
        this.m_PalmTree.destroy();
        this.m_Lorry.destroy();
        this.m_Ground.destroy();
        super.destroy();
    }
}
