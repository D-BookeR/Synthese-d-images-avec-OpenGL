// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Misc/TurnTableScene.js");
Requires("libs/Mesh/MeshObjectFromObj.js");
Requires("libs/Material/Light.js");
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
        this.m_Buildings = new MeshObjectFromObj("data/models/Immeubles", "immeubles.obj", "immeubles.mtl", 0.5);
        this.m_Ground    = new MeshObjectFromObj("data/models/Immeubles", "sol.obj", "sol.mtl", 0.5);

        // définir une lampe omnidirectionnelle
        this.m_Light0 = new OmniLight();
        this.m_Light0.setPosition(vec4.fromValues(5, 5, 0, 1));
        this.m_Light0.setColor(vec3.fromValues(8.0,8.0,8.0));

        // définir une lampe ambiante
        this.m_Light1 = new Light();
        this.m_Light1.setColor(vec3.fromValues(0.9,0.9,0.9));

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.enable(gl.CULL_FACE);

        // couleur du fond
        gl.clearColor(0.7, 0.8, 0.9, 0.0);

        // paramètres de la vue
        this.m_CameraAngleAzimut = 190.0;
        this.m_CameraAngleElevation = 25.0;
        vec3.set(this.m_CameraPivot, 0.0,-1.0,0.0);
        this.m_CameraDistance = 24;
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
        // dessiner les immeubles et le sol
        this.m_Buildings.onDraw(mat4Projection, mat4View);
        this.m_Ground.onDraw(mat4Projection, mat4View);
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
        this.m_Light0.transform(mat4Light);

        // dessiner les objets dans cette transformation
        this.drawDeferredShading(this.m_Mat4Projection, mat4View);

        // dessiner l'éclairement de la première lampe
        this.addLighting(this.m_Light0, true);

        // rajouter les éclairements des lampes suivantes
        this.addLighting(this.m_Light1);

        // dessiner la lampe (debug)
        //this.m_Light0.onDrawDebug(this.m_Mat4Projection, mat4View);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Light1.destroy();
        this.m_Light0.destroy();
        this.m_Ground.destroy();
        this.m_Buildings.destroy();
        super.destroy();
    }
}
