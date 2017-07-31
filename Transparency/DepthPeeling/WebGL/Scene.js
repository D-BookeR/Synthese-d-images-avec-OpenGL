// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Misc/TurnTableScene.js");
Requires("libs/Mesh/MeshObjectFromObj.js");
Requires("libs/Material/OmniLight.js");
Requires("libs/Material/SoftSpotLight.js");
Requires("libs/Material/SkyBackground.js");
Requires("libs/Process/FrameBufferObject.js");
Requires("TransparentMaterial");


class Scene extends TurnTableScene
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(false);

        // créer le matériau de dessin de la pomme (couleurs forcées sinon trop sombres)
        let Kd = vec4.fromValues(2.0, 0.4, 0.2, 0.5);
        let Ks = vec3.fromValues(2.0, 2.0, 2.0);
        let Ns = 128.0;
        this.m_TransparentMaterial = new TransparentMaterial(Kd, Ks, Ns);

        // créer les objets à dessiner
        this.m_Ground   = new MeshObjectFromObj("data/models/TerrainSimple", "Terrain.obj", "TerrainHerbe.mtl", 4.0);
        this.m_PalmTree = new MeshObjectFromObj("data/models/Palm_Tree", "Palm_Tree.obj", "Palm_Tree.mtl", 0.4);
        this.m_Lorry    = new MeshObjectFromObj("data/models/Camion", "camion.obj", "camion.mtl", 2.0);
        this.m_Apple    = new MeshObjectFromObj("data/models/Apple","apple.obj", this.m_TransparentMaterial, 0.02);

        // définir une lampe
        this.m_Light0 = new OmniLight();
        //this.m_Light0 = new SoftSpotLight(24.0, 20.0, 0.5, 512, 14, 26);
        //this.m_Light0.setTarget(vec4.fromValues(0, 0, 0, 1));
        this.m_Light0.setPosition(vec4.fromValues(5, 15, 15, 1));
        this.m_Light0.setColor(vec3.fromValues(500,500,500));
        this.addLight(this.m_Light0);

        // définir une lampe
        this.m_Light1 = new OmniLight();
        //this.m_Light1 = new SoftSpotLight(40.0, 40.0, 0.2, 512, 12, 23);
        //this.m_Light1.setTarget(vec4.fromValues(0, 0, 0, 1));
        this.m_Light1.setPosition(vec4.fromValues(8, 15, -6, 1));
        this.m_Light1.setColor(vec3.fromValues(300,300,300));
        this.addLight(this.m_Light1);

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);

        // couleur du fond
        gl.clearColor(0.5, 0.6, 0.7, 0.0);
        this.m_SkyBackground = new SkyBackground();

        // paramètres de la vue
        this.m_CameraAngleAzimut = -30.0;
        this.m_CameraAngleElevation = 20.0;
        vec3.set(this.m_CameraPivot, 0.0, -0.5, 0.0);
        this.m_CameraDistance = 40;

        // ressources
        this.m_GBuffer = null;
        this.m_PreviousGBuffer = null;
        this.m_CurrentGBuffer = null;
        this.m_FBOlights  = null;
        this.m_FBOlayers = null;
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
        mat4.perspective(this.m_Mat4Projection, Utils.radians(12.0), width / height, 20.0, 60.0);

        // supprimer les anciens FBO
        if (this.m_GBuffer != null) this.m_GBuffer.destroy();
        if (this.m_PreviousGBuffer != null) this.m_PreviousGBuffer.destroy();
        if (this.m_CurrentGBuffer != null) this.m_CurrentGBuffer.destroy();
        if (this.m_FBOlights != null) this.m_FBOlights.destroy();
        if (this.m_FBOlayers != null) this.m_FBOlayers.destroy();

        // agrandissement des FBO pour améliorer la qualité d'image
        const K = 2;

        // FBO pour la première passe du dessin des objets opaques
        this.m_GBuffer = new FrameBufferObject(width*K, height*K, gl.TEXTURE_2D, gl.TEXTURE_2D, 3);

        // FBO pour la première passe du dessin des objets transparents (alternances entre les deux)
        this.m_PreviousGBuffer = new FrameBufferObject(width*K, height*K, gl.TEXTURE_2D, gl.TEXTURE_2D, 3);
        this.m_CurrentGBuffer = new FrameBufferObject(width*K, height*K, gl.TEXTURE_2D, gl.TEXTURE_2D, 3);

        // FBO pour additionner les éclairements des lampes
        this.m_FBOlights = new FrameBufferObject(width*K, height*K, gl.TEXTURE_2D, gl.NONE);

        // FBO pour accumuler la deuxième passe du dessin des objets transparents
        this.m_FBOlayers = new FrameBufferObject(width*K, height*K, gl.TEXTURE_2D, gl.NONE);

        // fournir la taille de la fenêtre au matériau transparent
        this.m_TransparentMaterial.setWindowDimensions(width*K, height*K);
    }


    /**
     * dessine les objets opaques de la scène
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
        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(2.0,0.0,0.0));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(30.0));
        this.m_PalmTree.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(0.0,0.0,2.0));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(210.0));
        this.m_PalmTree.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(-0.5,0.0,-2.0));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(140.0));
        this.m_PalmTree.onDraw(mat4Projection, mat4ModelView);

        // dessiner les camions
        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(-2.0,0.0,-0.5));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(120.0));
        this.m_Lorry.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(1.0,0.0,1.0));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(-20.0));
        this.m_Lorry.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(-1.0,0.0,2.5));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(200.0));
        this.m_Lorry.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * dessine les objets transparents de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4View : matrice de vue
     */
    onDrawTranspa(mat4Projection, mat4View)
    {
        // matrice temporaire
        let mat4ModelView = mat4.create();

        // dessiner les pommes
        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(0.0,-3.0,0.0));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(120.0));
        mat4.scale(mat4ModelView, mat4ModelView, vec3.fromValues(5.0, 5.0, 5.0));
        this.m_Apple.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(2.5,-0.5,2.0));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(20.0));
        mat4.scale(mat4ModelView, mat4ModelView, vec3.fromValues(2.0, 2.0, 2.0));
        this.m_Apple.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(2.0,0,-1.5));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(20.0));
        this.m_Apple.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(-3.0,0.0,-3.0));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(80.0));
        this.m_Apple.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(-1.0,1.0,2.0));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(80.0));
        this.m_Apple.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(1.0,1.5,0.5));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(80.0));
        this.m_Apple.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(-3.0,0.5,0.0));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(80.0));
        this.m_Apple.onDraw(mat4Projection, mat4ModelView);
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

        // effacer le depth buffer du 2e g-buffer transparents
        gl.clearDepth(0.0);
        this.m_PreviousGBuffer.enable();
        gl.clear(gl.DEPTH_BUFFER_BIT);
        this.m_PreviousGBuffer.disable();
        gl.clearDepth(1.0);

        // effacer le FBO qui contiendra les couches transparentes
        this.m_FBOlayers.enable();
        gl.clearColor(0.0, 0.0, 0.0, 1.0);
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
        this.m_FBOlayers.disable();


        /// dessiner les objets opaques sur l'écran

        // ne dessiner que les faces avant
        gl.enable(gl.CULL_FACE);

        // dessiner les objets opaques dans le g-buffer
        this.m_GBuffer.enable();
        gl.clearColor(0.5, 0.6, 0.7, 0.0);
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
        this.onDraw(this.m_Mat4Projection, mat4CameraScene);
        this.m_GBuffer.disable();
        //this.m_GBuffer.onDraw(gl.DEPTH_ATTACHMENT);/*

        // dessiner les éclairements des objets opaques directement à l'écran
        gl.clearColor(0.0, 0.0, 0.0, 0.0);
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
        this.m_SkyBackground.process(this.m_GBuffer);
        gl.enable(gl.BLEND);
        gl.blendFunc(gl.SRC_ALPHA, gl.SRC_ALPHA);
        this.m_Light0.process(this.m_GBuffer);
        this.m_Light1.process(this.m_GBuffer);
        gl.disable(gl.BLEND);


        /// Cycles d'épluchage des transparences

        // dessiner toutes les faces des objets
        gl.disable(gl.CULL_FACE);

        // NumLayers est définie par un input dans le document HTML, voir onCouchesChanged() dans main.html
        for (let i=0; i<NumLayers; i++) {

            // fournir le depth buffer du g-buffer opaques au matériau transparent
            this.m_TransparentMaterial.setDepthMaps(this.m_GBuffer.getDepthBuffer(), this.m_PreviousGBuffer.getDepthBuffer());

            // rediriger les dessins vers le g-buffer première passe des transparents
            this.m_CurrentGBuffer.enable();
            gl.clearColor(0.0, 0.0, 0.0, 0.0);
            gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
            this.onDrawTranspa(this.m_Mat4Projection, mat4CameraScene);
            this.m_CurrentGBuffer.disable();
            //if (i==NumLayers-1){this.m_GBuffer_courant.onDraw(gl.COLOR_ATTACHMENT0);return;}
            //if (i==NumLayers-1){this.m_GBuffer_courant.onDraw(gl.DEPTH_ATTACHMENT);return;}
            //if (i==NumLayers-1){this.m_GBuffer_courant.onDrawAlpha(gl.COLOR_ATTACHMENT0);return;}

            // ajouter les éclairements des lampes sur les objets transparents
            this.m_FBOlights.enable();
            gl.clearColor(0.0, 0.0, 0.0, 0.0);
            gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
            gl.enable(gl.BLEND);
            gl.blendFuncSeparate(gl.ONE, gl.ONE, gl.ONE, gl.ZERO);
            this.m_Light0.process(this.m_CurrentGBuffer);
            this.m_Light1.process(this.m_CurrentGBuffer);
            gl.disable(gl.BLEND);
            this.m_FBOlights.disable();
            //if (i==NumLayers-1){this.m_FBOlights.onDraw(gl.COLOR_ATTACHMENT0);return;
            //if (i==NumLayers-1){this.m_FBOlights.onDrawAlpha(gl.COLOR_ATTACHMENT0);return

            // mélanger cette couche à l'arrière du FBO des couches
            this.m_FBOlayers.enable();
            gl.enable(gl.BLEND);
            gl.blendEquation(gl.FUNC_ADD);
            gl.blendFuncSeparate(gl.DST_ALPHA, gl.ONE, gl.ZERO, gl.ONE_MINUS_SRC_ALPHA);
            this.m_FBOlights.onDraw(gl.COLOR_ATTACHMENT0);
            gl.disable(gl.BLEND);
            this.m_FBOlayers.disable();
            //if (i==NumLayers-1){this.m_FBOlayers.onDraw(gl.COLOR_ATTACHMENT0);return;}
            //if (i==NumLayers-1){this.m_FBOlayers.onDrawAlpha(gl.COLOR_ATTACHMENT0);return;}

            // échanger les g-buffers
            let tmp = this.m_CurrentGBuffer;
            this.m_CurrentGBuffer = this.m_PreviousGBuffer;
            this.m_PreviousGBuffer = tmp;
        }
        //this.m_FBOlayers.onDraw(gl.COLOR_ATTACHMENT0);/*
        //this.m_FBOlayers.onDrawAlpha(gl.COLOR_ATTACHMENT0);/*

        // superposer le FBO des couches transparentes sur l'écran
        gl.enable(gl.BLEND);
        gl.blendFunc(gl.ONE_MINUS_SRC_ALPHA, gl.SRC_ALPHA);
        this.m_FBOlayers.onDraw(gl.COLOR_ATTACHMENT0);
        gl.disable(gl.BLEND);

        //si on décommente l'une des lignes de mise au point*/
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_SkyBackground.destroy();
        this.m_Light1.destroy();
        this.m_Light0.destroy();
        this.m_Apple.destroy();
        this.m_Lorry.destroy();
        this.m_PalmTree.destroy();
        this.m_Ground.destroy();
        this.m_TransparentMaterial.destroy();
        this.m_PreviousGBuffer.destroy();
        this.m_CurrentGBuffer.destroy();
        this.m_FBOlights.destroy();
        this.m_FBOlayers.destroy();
        super.destroy();
    }
}
