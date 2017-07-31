// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Material/Texture2D.js");
Requires("libs/Misc/TurnTableScene.js");
Requires("libs/Misc/MeshObjectFromObj.js");
Requires("Texture3LightsMaterial");


class Scene extends TurnTableScene
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(false);

        // créer le matériau pour les pommes
        this.m_Texture = new Texture2D("data/models/Apple/skin.jpg", gl.LINEAR, gl.CLAMP_TO_EDGE);
        this.m_Material = new Texture3LightsMaterial(this.m_Texture, vec3.fromValues(0.5,0.5,0.5), 64);

        // créer les objets
        this.m_Apple = new MeshObjectFromObj("data/models/Apple", "apple.obj", this.m_Material, 0.01);

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

        mat4.translate(mat4ModelViewApple, mat4ModelView, vec3.fromValues(-0.5, 0.0, 0.3));
        this.m_Apple.onDraw(mat4Projection, mat4ModelViewApple);

        mat4.translate(mat4ModelViewApple, mat4ModelView, vec3.fromValues(+0.5, 0.0, 0.3));
        mat4.rotateY(mat4ModelViewApple, mat4ModelViewApple, Utils.radians(160.0));
        this.m_Apple.onDraw(mat4Projection, mat4ModelViewApple);

        mat4.translate(mat4ModelViewApple, mat4ModelView, vec3.fromValues( 0.0, 0.0,-0.5));
        mat4.rotateY(mat4ModelViewApple, mat4ModelViewApple, Utils.radians(90.0));
        this.m_Apple.onDraw(mat4Projection, mat4ModelViewApple);
    }


    /** supprime tous les objets alloués */
    destroy()
    {
        this.m_Apple.destroy();
        this.m_Texture.destroy();
        super.destroy();
    }
}
