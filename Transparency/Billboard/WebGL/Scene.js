// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Misc/TurnTableScene.js");
Requires("Texture360");
Requires("Billboard");
Requires("Ground");


class Scene extends TurnTableScene
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(false);

        // crée les objets à dessiner
        this.m_Ground = new Ground(2.0);

        // créer un billboard
        this.m_TxTree = new Texture360("data/textures/Billboards/bb_arbre2", 8);
        this.m_Tree = new Billboard(this.m_TxTree);

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.disable(gl.CULL_FACE);      // laisser voir le dos des billboards

        // formule de blending
        gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);

        // couleur du fond
        gl.clearColor(0.6, 0.7, 1.0, 0.0);

        // paramètres de la vue
        this.m_CameraAngleAzimut = 40.0;
        this.m_CameraAngleElevation = 11.0;
        this.m_CameraDistance = 6;
        vec3.set(this.m_CameraPivot, 0.0, -0.4, 0.0);
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
        mat4.perspective(this.m_Mat4Projection, Utils.radians(10.0), width / height, 0.1, 40.0);
    }


    /**
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4View : matrice de vue
     */
    onDraw(mat4Projection, mat4View)
    {
        // dessiner le terrain
        this.m_Ground.onDraw(mat4Projection, mat4View);

        // dessiner l'arbre
        this.m_Tree.onDraw(mat4Projection, mat4View);

        // variante pour dessiner (commenter la ligne précédente)
        //this.m_Tree.onDraw2(mat4Projection, mat4View);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Ground.destroy();
        this.m_Tree.destroy();
        this.m_TxTree.destroy();
        super.destroy();
    }
}
