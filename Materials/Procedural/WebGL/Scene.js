// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Mesh/MeshObjectFromObj.js");
Requires("libs/Misc/TurnTableScene.js");
Requires("AppleMaterial");
Requires("GroundMaterial");


class Scene extends TurnTableScene
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(false);

        // créer les matériaux
        this.m_AppleMaterial = new AppleMaterial();
        this.m_GroundMaterial = new GroundMaterial();

        // créer les objets
        this.m_Apple = new MeshObjectFromObj("data/models/Apple","apple.obj", this.m_AppleMaterial, 0.01);
        this.m_Ground  = new MeshObjectFromObj("data/models/Terrain","Terrain.obj", this.m_GroundMaterial, 1.0);

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.disable(gl.CULL_FACE);   // laisser voir le dos des facettes

        // couleur du fond
        gl.clearColor(0.4, 0.4, 0.4, 0.0);

        // paramètres de la vue
        this.m_CameraAngleAzimut = 15.0;
        this.m_CameraAngleElevation = 50.0;
        vec3.set(this.m_CameraPivot, 0.0, -0.05, 0.0);
        this.m_CameraDistance = 9.5;
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
        // dessiner le terrain
        this.m_Ground.onDraw(mat4Projection, mat4ModelView);

        // mettre la pomme au dessus
        let mat4ModelViewApple = mat4.create();
        mat4.translate(mat4ModelViewApple, mat4ModelView, vec3.fromValues(0.0, 0.1, 0.2));
        this.m_Apple.onDraw(mat4Projection, mat4ModelViewApple);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Apple.destroy();
        this.m_Ground.destroy();
        this.m_AppleMaterial.destroy();
        this.m_GroundMaterial.destroy();
        super.destroy();
    }
}
