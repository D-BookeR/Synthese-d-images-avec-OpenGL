// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Material/DeferredShadingMaterial.js");
Requires("libs/Material/SimpleColorMaterial.js");
Requires("libs/Misc/MeshObjectFromObj.js");
Requires("libs/Material/SoftSpotLight.js");
Requires("libs/Process/FrameBufferObject.js");
Requires("libs/Misc/TurnTableScene.js");
Requires("ToneMapping");


class Scene extends TurnTableScene
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(true);

        // matériau pour la vache
        let cowKd = vec4.fromValues(0.9, 0.1, 0.8, 1.0);
        let cowKs = vec3.fromValues(1.0, 1.0, 1.0);
        let cowNs = 120;
        this.vacheMateriau = new DeferredShadingMaterial(cowKd, cowKs, cowNs);

        // matériau pour la sphère
        let sun_color = vec4.fromValues(100.0, 100.0, 100.0, 1.0);
        this.soleilMateriau = new SimpleColorMaterial(sun_color);

        // créer les objets à dessiner
        this.m_Ground   = new MeshObjectFromObj("data/models/TerrainSimple", "Terrain.obj", "Terrain.mtl", 4.0);
        this.m_Cow      = new MeshObjectFromObj("data/models/Cow", "Cow.obj", this.vacheMateriau, 0.1);
        this.m_PalmTree = new MeshObjectFromObj("data/models/Palm_Tree", "Palm_Tree.obj", "Palm_Tree.mtl", 0.5);
        this.m_SunLight   = new MeshObjectFromObj("data/models/Sphere", "sphere.obj", this.soleilMateriau, 0.5);

        // définir une lampe spot très puissante qui projette de l'ombre
        this.m_Light0 = new SoftSpotLight(100.0, 100.0, 0.15, 1024, 3.0, 11.0);
        this.m_Light0.setPosition(vec4.fromValues(3, 6, 3, 1));
        this.m_Light0.setTarget(vec4.fromValues(0, 0, 0, 1));
        this.m_Light0.setColor(vec3.fromValues(3000.0, 3000.0, 3000.0));
        this.addLight(this.m_Light0);

        // définir une lampe omni peu intense à l'opposé du soleil
        this.m_Light1 = new OmniLight();
        this.m_Light1.setPosition(vec4.fromValues(-3, 6, -3, 1));
        this.m_Light1.setColor(vec3.fromValues(500.0, 500.0, 500.0));
        this.addLight(this.m_Light1);

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.enable(gl.CULL_FACE);

        // couleur du fond
        gl.clearColor(20.0, 30.0, 40.0, 0.0);

        // paramètres de la vue
        this.m_CameraAngleAzimut = -200.0;
        this.m_CameraAngleElevation = 4.0;
        vec3.set(this.m_CameraPivot, 0.0, -1.0, 0.0);
        this.m_CameraDistance = 11.0;

        // ressources
        this.m_FBOhdr = null;
        this.m_ToneMapping = new ToneMapping();
    }


    /**
     * appelée quand la taille de la vue OpenGL change
     * @param width : largeur en nombre de pixels de la fenêtre
     * @param height : hauteur en nombre de pixels de la fenêtre
     */
    onSurfaceChanged(width, height)
    {
        // appeler la méthode de la superclasse
        super.onSurfaceChanged(width, height, 1);

        // matrice de projection (champ de vision)
        mat4.perspective(this.m_Mat4Projection, Utils.radians(25.0), width / height, 1.0, 50.0);

        // créer un FBO de cette taille pour dessiner hors écran
        if (this.m_FBOhdr != null) this.m_FBOhdr.destroy();
        this.m_FBOhdr = new FrameBufferObject(width, height, gl.TEXTURE_2D, gl.RENDERBUFFER, 0, gl.LINEAR);
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

        // dessiner le globe éclairant
        mat4.translate(mat4ModelView, mat4View, vec3.fromVec(this.m_Light0.getPosition()));
        this.m_SunLight.onDraw(mat4Projection, mat4ModelView);

        // dessiner les palmiers
        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(0.5,0,0.5));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(120.0));
        this.m_PalmTree.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(3.0,0,1.5));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(20.0));
        this.m_PalmTree.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(2.0,0,-1.0));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(20.0));
        this.m_PalmTree.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(-1.0,0.0,-1.5));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(80.0));
        this.m_PalmTree.onDraw(mat4Projection, mat4ModelView);

        // dessiner les vaches
        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(0.0,0.0,-0.5));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(120.0));
        this.m_Cow.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(1.0,0.0,2.0));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(-20.0));
        this.m_Cow.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(-1.0,0.0,2.0));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(200.0));
        this.m_Cow.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Dessine l'image courante
     */
    onDrawFrame()
    {
        // effacer l'écran
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        // construire la matrice caméra <- scène (fait passer de la scène à la caméra)
        let mat4CameraScene = this.getModelView();

        // préparer les lampes (transformation et shadow maps)
        this.prepareLights(mat4CameraScene);

        // dessiner les objets dans cette transformation
        this.drawDeferredShading(this.m_Mat4Projection, mat4CameraScene);

        // rediriger les dessins vers le FBO hdr
        this.m_FBOhdr.enable();
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        // dessiner l'éclairement des lampes
        this.addLightings();

        // revenir au dessin sur l'écran
        this.m_FBOhdr.disable();

        // (DEBUG) dessiner le color buffer net sur l'écran
        //this.m_FBOhdr.onDraw(gl.COLOR_ATTACHMENT0);/*

        // à ce stade, on a une image hdr dans this.m_FBOhdr, on va réduire sa dynamique
        this.m_ToneMapping.process(this.m_FBOhdr, 5, 3);


        //*/
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Light0.destroy();
        this.m_Light1.destroy();
        this.m_SunLight.destroy();
        this.m_PalmTree.destroy();
        this.m_Cow.destroy();
        this.m_Ground.destroy();

        this.m_ToneMapping.destroy();
        this.m_FBOhdr.destroy();
        super.destroy();
    }
}
