// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Misc/TurnTableScene.js");
Requires("libs/Misc/MeshObjectFromObj.js");
Requires("libs/Material/SoftSpotLight.js");
Requires("libs/Material/SkyBackground.js");
Requires("libs/Process/FrameBufferObject.js");


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

        // définir une lampe spot qui projette de l'ombre
        this.m_Light0 = new SoftSpotLight(24.0, 20.0, 0.5, 512, 14, 26);
        this.m_Light0.setPosition(vec4.fromValues(5, 15, 15, 1));
        this.m_Light0.setTarget(vec4.fromValues(0, 0, 0, 1));
        this.m_Light0.setColor(vec3.fromValues(400,400,400));
        this.addLight(this.m_Light0);

        // définir une lampe spot qui projette de l'ombre
        this.m_Light1 = new SoftSpotLight(40.0, 40.0, 0.2, 512, 12, 23);
        this.m_Light1.setPosition(vec4.fromValues(8, 15, -6, 1));
        this.m_Light1.setTarget(vec4.fromValues(0, 0, 0, 1));
        this.m_Light1.setColor(vec3.fromValues(300,300,300));
        this.addLight(this.m_Light1);

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.enable(gl.CULL_FACE);

        // couleur du fond
        gl.clearColor(0.4, 0.5, 0.6, 0.0);
        this.m_SkyBackground = new SkyBackground();

        // paramètres de la vue
        this.m_CameraAngleAzimut = -40.0;
        this.m_CameraAngleElevation = 35.0;
        vec3.set(this.m_CameraPivot, 0.0, -0.5, 0.0);
        this.m_CameraDistance = 30;

        // FBO nécessaires
        this.m_FBOimages = [];
        this.IMAGES_NUMBER = 12;
        this.m_CurrentFBOnumber = 0;
    }


    /**
     * appelée quand la taille de la vue OpenGL change
     * @param width : largeur en nombre de pixels de la fenêtre
     * @param height : hauteur en nombre de pixels de la fenêtre
     */
    onSurfaceChanged(width, height)
    {
        // facteur d'agrandissement du FBO pour simuler un antialiasing global
        const scale = 2;

        // appeler la méthode de la superclasse
        super.onSurfaceChanged(width, height);

        // matrice de projection (champ de vision)
        mat4.perspective(this.m_Mat4Projection, Utils.radians(12.0), width / height, 1.0, 100.0);

        // créer plusieurs FBO de cette taille pour mémoriser les images successives
        for (let i=0; i<this.m_FBOimages.length; i++) this.m_FBOimages[i].destroy();
        this.m_FBOimages = [];
        for (let i=0; i<this.IMAGES_NUMBER; i++) {
            this.m_FBOimages.push(new FrameBufferObject(width * scale, height * scale, gl.TEXTURE_2D, gl.RENDERBUFFER));
        }
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
        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(0.5,0,0.5));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(120.0));
        this.m_PalmTree.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(3.0,0,1.5));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(20.0));
        this.m_PalmTree.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(2.0,0,-3.5));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(20.0));
        this.m_PalmTree.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(-1.0,0.0,-2.0));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(80.0));
        this.m_PalmTree.onDraw(mat4Projection, mat4ModelView);

        // dessiner les camions
        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(-2.0,0.0,0.5));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(120.0));
        this.m_Lorry.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(1.0,0.0,2.0));
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
        // construire la matrice caméra <- scène (fait passer de la scène à la caméra)
        let mat4CameraScene = this.getModelView();

        // préparer les lampes (transformation et shadow maps)
        this.prepareLights(mat4CameraScene);

        // dessiner les objets dans cette transformation
        this.drawDeferredShading(this.m_Mat4Projection, mat4CameraScene);

        // rediriger les dessins vers le FBO courant
        this.m_FBOimages[this.m_CurrentFBOnumber].enable();

        // effacer l'écran
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        // dessiner le fond de ciel
        this.m_SkyBackground.process(this.m_GBuffer);

        // dessiner l'éclairement de la première lampe
        this.addLighting(this.m_Light0, true);

        // rajouter les éclairements des lampes suivantes
        this.addLighting(this.m_Light1);

        // revenir au dessin sur l'écran
        this.m_FBOimages[this.m_CurrentFBOnumber].disable();

        // (debug) dessiner le FBO sur l'écran
        //this.m_FBOimages[this.m_CurrentFBOnumber].onDraw(gl.COLOR_ATTACHMENT0);/*

        // effacer l'écran en noir pour additionner les images
        gl.clearColor(0.0, 0.0, 0.0, 0.0);
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        // superposer les images des FBO, avec un facteur de 1/m_NbImages
        this.m_FBOimages[0].onDraw(gl.COLOR_ATTACHMENT0);
        gl.blendFunc(gl.CONSTANT_ALPHA, gl.ONE_MINUS_CONSTANT_ALPHA);
        gl.blendColor(0.0, 0.0, 0.0, 1.0/(this.IMAGES_NUMBER-1));
        gl.enable(gl.BLEND);
        for (let i=1; i<this.IMAGES_NUMBER; i++) {
            this.m_FBOimages[i].onDraw(gl.COLOR_ATTACHMENT0);
        }
        gl.disable(gl.BLEND);

        // passer à l'image suivante
        this.m_CurrentFBOnumber = (this.m_CurrentFBOnumber + 1) % this.IMAGES_NUMBER;
        //*/
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_SkyBackground.destroy();
        this.m_Light1.destroy();
        this.m_Light0.destroy();
        this.m_PalmTree.destroy();
        this.m_Lorry.destroy();
        this.m_Ground.destroy();

        for (let i=0; i<this.IMAGES_NUMBER; i++) this.m_FBOimages[i].destroy();
        super.destroy();
    }
}
