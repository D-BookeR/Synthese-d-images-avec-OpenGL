// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Process/FrameBufferObject.js");
Requires("libs/Material/OmniLight.js");
Requires("libs/Material/Light.js");
Requires("libs/Misc/TurnTableScene.js");
Requires("libs/Misc/MeshObjectFromObj.js");


class Scene extends TurnTableScene
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(false);

        // créer les objets à dessiner
        this.m_Ground = new MeshObjectFromObj("data/models/TerrainSimple", "Terrain.obj", "Terrain.mtl", 4.0);
        this.m_Lorry  = new MeshObjectFromObj("data/models/Camion", "camion.obj", "camion.mtl", 4.0);
        this.m_Apple  = new MeshObjectFromObj("data/models/Apple", "apple.obj", "apple.mtl", 0.02);

        // définir une lampe omnidirectionnelle
        this.m_Light0 = new OmniLight();
        this.m_Light0.setPosition(vec4.fromValues(5, 5, 0, 1));
        this.m_Light0.setColor(vec3.fromValues(20.0,20.0,20.0));
        this.addLight(this.m_Light0);

        // définir une lampe ambiante
        this.m_Light1 = new Light();
        this.m_Light1.setColor(vec3.fromValues(0.4,0.4,0.4));
        this.addLight(this.m_Light1);

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.enable(gl.CULL_FACE);

        // couleur du fond
        gl.clearColor(0.7, 0.8, 0.9, 0.0);

        // paramètres de la vue
        this.m_CameraAngleAzimut = 30.0;
        this.m_CameraAngleElevation = 15.0;
        vec3.set(this.m_CameraPivot, 0.0,-0.7,0.0);
        this.m_CameraDistance = 22;

        // taille des sous-fenêtres : 1/3 et 1/2 de la fenêtre principale
        this.m_WindowWidth3 = 0;
        this.m_WindowHeight2 = 0;
    }


    /**
     * appelée quand la taille de la vue OpenGL change
     * @param width : largeur en nombre de pixels de la fenêtre
     * @param height : hauteur en nombre de pixels de la fenêtre
     */
    onSurfaceChanged(width, height)
    {
        // taille des sous-fenêtres : 1/3 et 1/2 de la fenêtre principale
        this.m_WindowWidth3 = width/3.0;
        this.m_WindowHeight2 = height/2.0;

        // matrice de projection (champ de vision)
        mat4.perspective(this.m_Mat4Projection, Utils.radians(12.0), this.m_WindowWidth3 / this.m_WindowHeight2, 15.0, 30.0);

        // créer un FBO pour dessiner hors écran avec plusieurs buffers pour stocker toutes les informations nécessaires
        if (this.m_GBuffer != null) this.m_GBuffer.destroy();
        this.m_GBuffer = new FrameBufferObject(this.m_WindowWidth3, this.m_WindowHeight2, gl.TEXTURE_2D, gl.TEXTURE_2D, 3);
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

        // dessiner le camion
        mat4.rotateY(mat4ModelView, mat4View, Utils.radians(20.0));
        this.m_Lorry.onDraw(mat4Projection, mat4ModelView);

        // dessiner la pomme
        mat4.translate(mat4ModelView, mat4ModelView, vec3.fromValues(0.0,1.0,-0.2));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(120.0));
        this.m_Apple.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Dessine l'image courante
     */
    onDrawFrame()
    {
        // construire la matrice caméra
        let mat4View = this.getModelView();

        // faire tourner la lampe autour de la scène
        let mat4Light = mat4.create();
        mat4.rotateY(mat4Light, mat4View, Utils.Time*0.5+3.0);

        // préparer les lampes (transformation et shadow maps)
        this.prepareLights(mat4Light);

        // rediriger les dessins vers le FBO, attention tout doit être dessiné avec le matériau DeferredShadingMaterial
        this.m_GBuffer.enable();

        // effacer l'écran
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        // dessiner les objets de la scène
        this.onDraw(this.m_Mat4Projection, mat4View);

        // revenir au dessin sur l'écran
        this.m_GBuffer.disable();

        // effacer l'écran
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        // dessiner le diffuse buffer dans le coin haut gauche
        gl.viewport(0, this.m_WindowHeight2, this.m_WindowWidth3, this.m_WindowHeight2);
        this.m_GBuffer.onDraw(gl.COLOR_ATTACHMENT0);

        // dessiner le specular buffer au milieu haut
        gl.viewport(this.m_WindowWidth3, this.m_WindowHeight2, this.m_WindowWidth3, this.m_WindowHeight2);
        this.m_GBuffer.onDraw(gl.COLOR_ATTACHMENT0+1); // gl.COLOR_ATTACHMENT1 n'existe pas encore en WebGL

        // dessiner le position buffer dans le coin bas gauche
        gl.viewport(0, 0, this.m_WindowWidth3, this.m_WindowHeight2);
        this.m_GBuffer.onDraw(gl.COLOR_ATTACHMENT0+2); // gl.COLOR_ATTACHMENT2 n'existe pas encore en WebGL

        // dessiner le normal buffer au milieu bas
        gl.viewport(this.m_WindowWidth3, 0, this.m_WindowWidth3, this.m_WindowHeight2);
        this.m_GBuffer.onDraw(gl.COLOR_ATTACHMENT0+3); // gl.COLOR_ATTACHMENT3 n'existe pas encore en WebGL

        // dessiner le depth buffer dans le coin haut droit
        gl.viewport(this.m_WindowWidth3*2, this.m_WindowHeight2, this.m_WindowWidth3, this.m_WindowHeight2);
        this.m_GBuffer.onDraw(gl.DEPTH_ATTACHMENT);

        // préparer le dessin de l'image résultante dans le coin bas droite
        gl.viewport(this.m_WindowWidth3*2, 0, this.m_WindowWidth3, this.m_WindowHeight2);

        // dessiner l'éclairement de la première lampe
        this.m_Light0.process(this.m_GBuffer);

        // passer en mode blending pour additionner les éclairements
        gl.enable(gl.BLEND);
        gl.blendFunc(gl.ONE, gl.ONE);

        // rajouter les éclairements des lampes suivantes
        this.m_Light1.process(this.m_GBuffer);

        // revenir en mode normal
        gl.disable(gl.BLEND);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Light1.destroy();
        this.m_Light0.destroy();
        this.m_Apple.destroy();
        this.m_Lorry.destroy();
        this.m_Ground.destroy();
        super.destroy();
    }
}
