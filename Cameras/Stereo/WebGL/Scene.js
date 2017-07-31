// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Mesh/MeshObjectFromObj.js");
Requires("libs/Material/OmniLight.js");
Requires("libs/Misc/TurnTableScene.js");


/**
 * écart entre les deux yeux
 *   négatif pour une visée parallèle (Loreo Pixi 3D Viewer)
 *   positif pour une visée croisée (loucher et se faire mal aux yeux)
 */
var EcartOculaire = 0.5;


class Scene extends TurnTableScene
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(true);

        // créer les objets à dessiner
        this.m_Ground = new MeshObjectFromObj("data/models/TerrainSimple", "Terrain.obj", "Terrain.mtl", 2.0);
        this.m_Lorry  = new MeshObjectFromObj("data/models/Camion", "camion.obj", "camion.mtl", 2.0);
        this.m_PalmTree = new MeshObjectFromObj("data/models/Palm_Tree", "Palm_Tree.obj", "Palm_Tree.mtl", 2.0);

        // définir une lampe directionnelle
        this.m_Light0 = new OmniLight();
        this.m_Light0.setPosition(vec4.fromValues(10, 5, 10, 0));
        this.m_Light0.setColor(vec3.fromValues(0.9,0.9,0.9));
        this.addLight(this.m_Light0);

        // définir une lampe positionnelle
        this.m_Light1 = new OmniLight();
        this.m_Light1.setPosition(vec4.fromValues(-10, 5, 5, 1));
        this.m_Light1.setColor(vec3.fromValues(80,80,80));
        this.addLight(this.m_Light1);

        // définir une lampe directionnelle
        this.m_Light2 = new OmniLight();
        this.m_Light2.setPosition(vec4.fromValues(-5, 5, -10, 0));
        this.m_Light2.setColor(vec3.fromValues(1.0,1.0,1.0));
        this.addLight(this.m_Light2);

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
        // dimensions réduites, deux vues dans la fenêtre
        this.m_WindowWidth2 = width/2;
        this.m_WindowHeight = height;

        // appeler la méthode de la superclasse
        super.onSurfaceChanged(this.m_WindowWidth2, this.m_WindowHeight);

        // matrice de projection (champ de vision)
        mat4.perspective(this.m_Mat4Projection, Utils.radians(12.0), this.m_WindowWidth2 / this.m_WindowHeight, 1.0, 100.0);
    }


    /**
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4View : matrice de vue
     */
    onDraw(mat4Projection, mat4View)
    {
        // définir le pivot des rotations
        mat4.translate(mat4View, mat4View, this.m_CameraPivot);

        // dessiner le terrain
        let mat4ModelView = mat4.create();
        mat4.scale(mat4ModelView, mat4View, vec3.fromValues(2.0, 2.0, 2.0));
        this.m_Ground.onDraw(mat4Projection, mat4ModelView);

        // dessiner les palmiers
        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(2.5,0,-1.0));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(120.0));
        mat4.scale(mat4ModelView, mat4ModelView, vec3.fromValues(0.2, 0.2, 0.2));
        this.m_PalmTree.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(3.0,0,1.5));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(20.0));
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
     * Dessine l'image courante
     */
    onDrawFrame()
    {
        // effacer l'écran
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        // construire la matrice caméra
        let mat4View = mat4.create();
        let mat4ViewTmp = mat4.create();

        // éloignement de la scène
        mat4.translate(mat4View, mat4View, vec3.fromValues(0.0, 0.0, -this.m_CameraDistance));

        // orientation de la scène par rapport à la caméra
        mat4.rotateX(mat4View, mat4View, Utils.radians(this.m_CameraAngleElevation));
        mat4.rotateY(mat4View, mat4View, Utils.radians(this.m_CameraAngleAzimut));

        // transformer les lampes
        this.prepareLights(mat4View);

        // vue gauche
        gl.viewport(0, 0, this.m_WindowWidth2, this.m_WindowHeight);
        mat4.fromTranslation(mat4ViewTmp, vec3.fromValues(-EcartOculaire, 0.0, 0.0));
        mat4.multiply(mat4ViewTmp, mat4ViewTmp, mat4View);

        // dessiner les objets dans cette transformation
        this.drawDeferredShading(this.m_Mat4Projection, mat4ViewTmp);

        // dessiner les éclairements des lampes
        this.addLightings();

        // vue droite
        gl.viewport(this.m_WindowWidth2, 0, this.m_WindowWidth2, this.m_WindowHeight);
        mat4.fromTranslation(mat4ViewTmp, vec3.fromValues(+EcartOculaire, 0.0, 0.0));
        mat4.multiply(mat4ViewTmp, mat4ViewTmp, mat4View);

        // dessiner les objets dans cette transformation
        this.drawDeferredShading(this.m_Mat4Projection, mat4ViewTmp);

        // dessiner les éclairements des lampes
        this.addLightings();
    }


    /** supprime tous les objets créés */
    destroy()
    {
        this.m_Light2.destroy();
        this.m_Light1.destroy();
        this.m_Light0.destroy();
        this.m_PalmTree.destroy();
        this.m_Lorry.destroy();
        this.m_Ground.destroy();
        super.destroy();
    }
}
