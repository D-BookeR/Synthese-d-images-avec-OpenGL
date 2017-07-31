// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Misc/TurnTableScene.js");
Requires("libs/Misc/MeshObjectFromObj.js");
Requires("libs/Material/Light.js");
Requires("libs/Material/SoftSpotLight.js");


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

        // définir une lampe ambiante
        this.m_Light0 = new Light();
        this.m_Light0.setColor(vec3.fromValues(0.4,0.4,0.4));

        // définir une lampe spot douce
        // pour PCSS : mettre true, false, gl.FRONT
        // pour PCF : mettre false, true, gl.BACK
        this.m_Light1 = new SoftSpotLight(20.0, 14.0, 0.1, 512, 16, 25, true, false, gl.FRONT);
        //this.m_Light1 = new SoftSpotLight(20.0, 14.0, 0.1, 512, 16, 25, false, false, gl.FRONT);
        this.m_Light1.setPosition(vec4.fromValues(15, 15, 0, 1));
        this.m_Light1.setTarget(vec4.fromValues(0, 0, 0, 1));
        this.m_Light1.setColor(vec3.fromValues(400,400,400));

        // définir une lampe spot douce
        // pour PCSS : mettre true, false, gl.FRONT
        // pour PCF : mettre false, true, gl.BACK
        this.m_Light2 = new SoftSpotLight(20.0, 14.0, 0.1, 512, 4, 60, true, false, gl.FRONT);
        //this.m_Light2 = new SoftSpotLight(20.0, 14.0, 0.1, 512, 4, 60, false, false, gl.FRONT);
        this.m_Light2.setPosition(vec4.fromValues(5, 5, 0, 1));
        this.m_Light2.setTarget(vec4.fromValues(0, 0, 0, 1));
        this.m_Light2.setColor(vec3.fromValues(600,600,600));

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.enable(gl.CULL_FACE);

        // couleur du fond
        gl.clearColor(0.4, 0.5, 0.6, 0.0);

        // paramètres de la vue
        this.m_CameraAngleAzimut = -40.0;
        this.m_CameraAngleElevation = 35.0;
        this.m_CameraDistance = 30;
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
        let mat4ViewCamera = mat4.create();

        // la position de la lampe2 est relative à la caméra
        this.m_Light2.transformPosition(mat4ViewCamera);

        // éloignement de la scène
        mat4.translate(mat4ViewCamera, mat4ViewCamera, vec3.fromValues(0.0, 0.0, -this.m_CameraDistance));

        // orientation de la scène par rapport à la caméra
        mat4.rotateX(mat4ViewCamera, mat4ViewCamera, Utils.radians(this.m_CameraAngleElevation));
        mat4.rotateY(mat4ViewCamera, mat4ViewCamera, Utils.radians(this.m_CameraAngleAzimut));

        // définir le pivot des rotations
        mat4.translate(mat4ViewCamera, mat4ViewCamera, this.m_CameraPivot);

        // rendre la lampe 1 fixe par rapport à la scène
        this.m_Light1.transform(mat4ViewCamera);

        // la cible de la lampe 2 est relative à la scène
        this.m_Light2.transformTarget(mat4ViewCamera);

        // calculer les shadow maps des lampes
        this.m_Light1.makeShadowMap(this, mat4ViewCamera);
        this.m_Light2.makeShadowMap(this, mat4ViewCamera);

        // dessiner les objets dans cette transformation
        this.drawDeferredShading(this.m_Mat4Projection, mat4ViewCamera);

        // dessiner l'éclairement de la première lampe
        this.addLighting(this.m_Light0, true);

        // rajouter les éclairements des lampes suivantes
        this.addLighting(this.m_Light1);
        this.addLighting(this.m_Light2);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
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
