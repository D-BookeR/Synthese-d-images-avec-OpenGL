// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Process/FrameBufferObject.js");
Requires("libs/Misc/TurnTableScene.js");
Requires("libs/Misc/Cube");
Requires("Scene1");
Requires("FBOMaterial");


class Scene extends TurnTableScene
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(false);

        // créer la sous-scène Scene1
        this.m_Scene1 = new Scene1();

        // lui donner sa taille d'affichage hors écran
        let width = 512;
        let height = 512;
        this.m_Scene1.onSurfaceChanged(width, height);

        // FBO nécessaire pour le dessin hors écran de la scène imbriquée
        this.m_FBO = new FrameBufferObject(width, height, gl.TEXTURE_2D);

        // créer le matériau de dessin du cube
        this.m_Material = new FBOMaterial(this.m_FBO.getColorBuffer());

        // créer le cube sur lequel on va projeter la sous-scène
        this.m_Cube = new Cube(this.m_Material);

        // paramètres de la vue
        this.m_CameraAngleAzimut = 30.0;
        this.m_CameraAngleElevation = 20.0;
        this.m_CameraDistance = 18;
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
        mat4.perspective(this.m_Mat4Projection, Utils.radians(10.0), width / height, 0.1, 20.0);
    }


    /**
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4View : matrice de vue
     */
    onDraw(mat4Projection, mat4View)
    {
        // dessiner la sous-scène dans le FBO
        this.m_FBO.enable();
        this.m_Scene1.onDrawFrame();
        this.m_FBO.disable();

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.enable(gl.CULL_FACE);

        // couleur du fond
        gl.clearColor(0.4, 0.4, 0.4, 0.0);

        // effacer l'écran
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        // rotation du cube
        let mat4ModelView = mat4.create();
        mat4.rotateY(mat4ModelView, mat4View, Utils.radians(35.0 * Utils.Time));

        // dessiner le cube
        this.m_Cube.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Cube.destroy();
        this.m_Material.destroy();
        this.m_FBO.destroy();
        this.m_Scene1.destroy();
        super.destroy();
    }
}
