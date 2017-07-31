// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Misc/TurnTableScene.js");
Requires("libs/Process/FrameBufferObject.js");
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
        this.m_Clouds = new Rectangle("data/textures/nuages_pre.png", 1.33, 1.0);
        this.m_PalmTree = new Rectangle("data/textures/Arbres/treeM044_pre.png", 1.0, 2.0);
        this.m_SmallTree = new Rectangle("data/textures/Arbres/plantM060_pre.png", 1.0, 1.33);

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

        // FBO pour accumuler la deuxième passe du dessin des objets transparents
        this.m_FBOlayers = new FrameBufferObject(width, height, gl.TEXTURE_2D, gl.NONE);
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

        // activer le FBO
        this.m_FBOlayers.enable();

        // effacer le FBO rgb=0, alpha=1
        gl.clearColor(0.0, 0.0, 0.0, 1.0);
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        // activer le mode blending permettant de fusionner d'avant en arrière
        gl.enable(gl.BLEND);
        gl.blendFuncSeparate(gl.DST_ALPHA, gl.ONE, gl.ZERO, gl.ONE_MINUS_SRC_ALPHA);

        // dessiner l'arbuste
        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(-0.2,-0.3,0.5));
        mat4.scale(mat4ModelView, mat4ModelView, vec3.fromValues(0.5,0.5,0.5));
        this.m_SmallTree.onDraw(mat4Projection, mat4ModelView);

        // dessiner le palmier
        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(0.2,0.2, 0.0));
        mat4.scale(mat4ModelView, mat4ModelView, vec3.fromValues(0.5,0.5,0.5));
        this.m_PalmTree.onDraw(mat4Projection, mat4ModelView);

        // dessiner les nuages
        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(-0.2,0.7,-0.5));
        this.m_Clouds.onDraw(mat4Projection, mat4ModelView);

        // dessiner le fond
        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(0.0,0.0,-1.0));
        this.m_Background.onDraw(mat4Projection, mat4ModelView);

        // fin du dessin dans le FBO
        this.m_FBOlayers.disable();

        // changer l'équation de mélange
        gl.blendFunc(gl.ONE_MINUS_SRC_ALPHA, gl.SRC_ALPHA);

        // dessiner le résultat
        this.m_FBOlayers.onDraw(gl.COLOR_ATTACHMENT0);

        // désactiver le mode blending
        gl.disable(gl.BLEND);

        // remettre la couleur d'effacement
        gl.clearColor(0.4, 0.4, 0.4, 0.0);
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
        this.m_FBOlayers.destroy();
        super.destroy();
    }
}
