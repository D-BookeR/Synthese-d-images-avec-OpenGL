// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Misc/TurnTableScene.js");
Requires("libs/Mesh/MeshObjectFromObj");
Requires("LightMaterial1");
Requires("LightMaterial2");
Requires("LightMaterial3");


class Scene extends TurnTableScene
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(false);

        // créer les matériaux pour les pommes
        this.m_LightMaterial1 = new LightMaterial1(vec3.fromValues(0.1,0.9,0.0));
        this.m_LightMaterial2 = new LightMaterial2(vec3.fromValues(0.1,0.6,1.0), vec3.fromValues(0.5,0.5,0.5), 64);
        this.m_LightMaterial3 = new LightMaterial3(vec3.fromValues(0.7,0.2,0.1), vec3.fromValues(0.5,0.5,0.5), 64);

        // créer les objets
        this.m_Apple1 = new MeshObjectFromObj("data/models/Apple","apple.obj", this.m_LightMaterial1, 0.01);
        this.m_Apple2 = new MeshObjectFromObj("data/models/Apple","apple.obj", this.m_LightMaterial2, 0.01);
        this.m_Apple3 = new MeshObjectFromObj("data/models/Apple","apple.obj", this.m_LightMaterial3, 0.01);

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.enable(gl.CULL_FACE);

        // couleur du fond
        gl.clearColor(0.4, 0.4, 0.4, 0.0);

        // paramètres de la vue
        this.m_CameraAngleAzimut = 15.0;
        this.m_CameraAngleElevation = 10.0;
        vec3.set(this.m_CameraPivot, 0.0, -0.4, 0.0);
        this.m_CameraDistance = 8.0;
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
        mat4.perspective(this.m_Mat4Projection, Utils.radians(10.0), width / height, 0.1, 15.0);
    }


    /**
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    onDraw(mat4Projection, mat4ModelView)
    {
        let mat4ModelViewApple = mat4.create();

        // positionner et dessiner le premier objet
        mat4.translate(mat4ModelViewApple, mat4ModelView, vec3.fromValues(-0.5, 0.0, 0.3));
        this.m_Apple1.onDraw(mat4Projection, mat4ModelViewApple);

        // positionner et dessiner le deuxième objet
        mat4.translate(mat4ModelViewApple, mat4ModelView, vec3.fromValues(+0.5, 0.0, 0.3));
        this.m_Apple2.onDraw(mat4Projection, mat4ModelViewApple);

        // positionner et dessiner le troisième objet
        mat4.translate(mat4ModelViewApple, mat4ModelView, vec3.fromValues( 0.0, 0.0,-0.5));
        this.m_Apple3.onDraw(mat4Projection, mat4ModelViewApple);
    }


    /** supprime tous les objets alloués */
    destroy()
    {
        this.m_Apple3.destroy();
        this.m_Apple2.destroy();
        this.m_Apple1.destroy();
        this.m_LightMaterial3.destroy();
        this.m_LightMaterial2.destroy();
        this.m_LightMaterial1.destroy();
        super.destroy();
    }
}
