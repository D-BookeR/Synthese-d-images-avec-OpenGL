// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Misc/TurnTableScene.js");
Requires("Rectangle");


class Scene extends TurnTableScene
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(false);

        // créer les rectangles qui représentent les couches à fusionner
        this.m_Background = new Rectangle("data/textures/hill-meadow-tree-green-2048.jpg", 1.33, 1.0);
        this.m_Clouds = new Rectangle("data/textures/nuages.png", 1.33, 1.0);
        this.m_PalmTree = new Rectangle("data/textures/Arbres/treeM044.png", 1.0, 2.0);
        this.m_SmallTree = new Rectangle("data/textures/Arbres/plantM060.png", 1.0, 1.33);

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.disable(gl.CULL_FACE);      // laisser voir le dos du rectangle

        // couleur du fond
        gl.clearColor(0.4, 0.4, 0.4, 0.0);

        // paramètres de la vue
        this.m_CameraAngleAzimut = 0.0;
        this.m_CameraAngleElevation = 0.0;
        this.m_CameraDistance = 15;
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
        mat4.perspective(this.m_Mat4Projection, Utils.radians(8.0), width / height, 0.1, 20.0);
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

        // dessiner le fond
        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(0.0,0.0,-1.0));
        this.m_Background.onDraw(mat4Projection, mat4ModelView);

        // activer le mode blending
        gl.enable(gl.BLEND);
        gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);

        // dessiner les nuages
        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(-0.2,0.7,-0.5));
        this.m_Clouds.onDraw(mat4Projection, mat4ModelView);

        // dessiner le palmier
        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(0.2,0.2, 0.0));
        mat4.scale(mat4ModelView, mat4ModelView, vec3.fromValues(0.5,0.5,0.5));
        this.m_PalmTree.onDraw(mat4Projection, mat4ModelView);

        // dessiner l'arbuste
        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(-0.2,-0.3,0.5));
        mat4.scale(mat4ModelView, mat4ModelView, vec3.fromValues(0.5,0.5,0.5));
        this.m_SmallTree.onDraw(mat4Projection, mat4ModelView);

        // désactiver le mode blending
        gl.disable(gl.BLEND);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_SmallTree.destroy();
        this.m_PalmTree.destroy();
        this.m_Clouds.destroy();
        this.m_Background.destroy();
        super.destroy();
    }
}
