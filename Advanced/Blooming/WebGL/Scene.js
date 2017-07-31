// Définition de la classe Scene

// http://stackoverflow.com/questions/30419153/hdr-bloom-effect-rendering-pipeline-using-opengl-glsl
// http://prideout.net/archive/bloom/

// superclasses et classes nécessaires
Requires("libs/Material/DeferredShadingMaterial.js");
Requires("libs/Material/SimpleColorMaterial.js");
Requires("libs/Misc/MeshObjectFromObj.js");
Requires("libs/Material/SoftSpotLight.js");
Requires("libs/Process/FrameBufferObject.js");
Requires("libs/Process/GaussianBlur.js");
Requires("libs/Process/LuminosityContrast.js");
Requires("libs/Misc/TurnTableScene.js");


class Scene extends TurnTableScene
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(true);

        // matériau pour la vache
        let cowKd = vec4.fromValues(0.7, 0.3, 0.2, 1.0);
        let cowKs = vec3.fromValues(1.0, 1.0, 1.0);
        let cowNs = 120;
        this.m_CowMaterial = new DeferredShadingMaterial(cowKd, cowKs, cowNs);

        // matériau pour la sphère
        let sun_color = vec4.fromValues(100.0, 100.0, 100.0, 1.0);
        this.m_SunLightMateriau = new SimpleColorMaterial(sun_color);

        // créer les objets à dessiner
        this.m_Ground   = new MeshObjectFromObj("data/models/TerrainSimple", "Terrain.obj", "Terrain.mtl", 4.0);
        this.m_Cow      = new MeshObjectFromObj("data/models/Cow", "Cow.obj", this.m_CowMaterial, 0.1);
        this.m_PalmTree = new MeshObjectFromObj("data/models/Palm_Tree", "Palm_Tree.obj", "Palm_Tree.mtl", 0.3);
        this.m_SunLight   = new MeshObjectFromObj("data/models/Sphere", "sphere.obj", this.m_SunLightMateriau, 0.5);

        // définir une lampe spot qui projette de l'ombre
        this.m_Light = new SoftSpotLight(115.0, 115.0, 0.15, 512, 1.5, 10.0);
        this.m_Light.setPosition(vec4.fromValues(2, 4, 3, 1));
        this.m_Light.setTarget(vec4.fromValues(0, 0, 0, 1));
        this.m_Light.setColor(vec3.fromValues(20,20,20));
        this.addLight(this.m_Light);

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.enable(gl.CULL_FACE);

        // couleur du fond
        gl.clearColor(0.2, 0.3, 0.4, 0.0);

        // paramètres de la vue
        this.m_CameraAngleAzimut = -175.0;
        this.m_CameraAngleElevation = -7.5;
        vec3.set(this.m_CameraPivot, -0.5, -1.7, 0.1);
        this.m_CameraDistance = 9.4;

        // ressources
        this.m_FBOimage = null;
        this.m_FBO1 = null;
        this.m_FBO2 = null;
        this.m_GaussianBlur = null;
        this.m_LuminosityContrast = new LuminosityContrast();
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
        mat4.perspective(this.m_Mat4Projection, Utils.radians(25.0), width / height, 1.0, 50.0);

        // créer un FBO de cette taille pour dessiner hors écran
        const K = 6;
        if (this.m_FBOimage != null) this.m_FBOimage.destroy();
        this.m_FBOimage = new FrameBufferObject(width, height, gl.TEXTURE_2D, gl.RENDERBUFFER, 0, gl.LINEAR);
        if (this.m_FBO1 != null) this.m_FBO1.destroy();
        this.m_FBO1   = new FrameBufferObject(width/K, height/K, gl.TEXTURE_2D, gl.NONE, 0, gl.LINEAR);
        if (this.m_FBO2 != null) this.m_FBO2.destroy();
        this.m_FBO2   = new FrameBufferObject(width/K, height/K, gl.TEXTURE_2D, gl.NONE, 0, gl.LINEAR);

        // traitement d'image nécessaire
        if (this.m_GaussianBlur != null) this.m_GaussianBlur.destroy();
        this.m_GaussianBlur = new GaussianBlur(width/K, height/K);
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
        mat4.translate(mat4ModelView, mat4View, vec3.fromVec(this.m_Light.getPosition()));
        this.m_SunLight.onDraw(mat4Projection, mat4ModelView);

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

        // dessiner les vaches
        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(-2.0,0.0,0.5));
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
        //this.m_Light.onDrawShadowMap();/*

        // dessiner les objets dans cette transformation
        this.drawDeferredShading(this.m_Mat4Projection, mat4CameraScene);

        // rediriger les dessins vers le FBO net
        this.m_FBOimage.enable();
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        // dessiner l'éclairement de la lampe
        this.addLighting(this.m_Light, true);

        // revenir au dessin sur l'écran
        this.m_FBOimage.disable();
        //this.m_FBOimage.onDraw(gl.COLOR_ATTACHMENT0);/*


        /// à ce stade, on a une image nette dans m_FBOimage, on va la seuiller dans m_FBO1


        // rediriger les dessins vers le FBO1
        this.m_FBO1.enable();
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        // éclaicir et constraster l'image pour faire ressortir les zones brillantes
        this.m_LuminosityContrast.process(this.m_FBOimage, -0.1, 40.0);

        // revenir au dessin sur l'écran
        this.m_FBO1.disable();
        //this.m_FBO1.onDraw(gl.COLOR_ATTACHMENT0);/*


        /// à ce stade, on a une image en noir et blanc dans m_FBO1, on va la flouter dans m_FBO2


        // rediriger les dessins vers le FBO2
        this.m_FBO2.enable();
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        // appliquer un flou pour étaler les zones brillantes
        this.m_GaussianBlur.process(this.m_FBO1, 1.2);

        // revenir au dessin sur l'écran
        this.m_FBO2.disable();
        //this.m_FBO2.onDraw(gl.COLOR_ATTACHMENT0);/*


        /// à ce stade, on a une image en noir et blanc dans m_FBO2, on va la superposer à m_FBOimage


        // dessiner le color buffer net sur l'écran
        this.m_FBOimage.onDraw(gl.COLOR_ATTACHMENT0);

        // superposer l'image floue des zones brillantes
        gl.enable(gl.BLEND);
        gl.blendFunc(gl.ONE, gl.ONE);
        this.m_FBO2.onDraw(gl.COLOR_ATTACHMENT0);
        gl.disable(gl.BLEND);

        //*/
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Light.destroy();
        this.m_SunLight.destroy();
        this.m_PalmTree.destroy();
        this.m_Cow.destroy();
        this.m_Ground.destroy();
        this.m_CowMaterial.destroy();
        this.m_SunLightMateriau.destroy();
        this.m_LuminosityContrast.destroy();
        this.m_GaussianBlur.destroy();
        this.m_FBOimage.destroy();
        this.m_FBO1.destroy();
        this.m_FBO2.destroy();
        super.destroy();
    }
}
