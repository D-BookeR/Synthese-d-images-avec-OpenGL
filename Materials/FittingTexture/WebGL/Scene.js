// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Misc/TurnTableScene.js");
Requires("Rectangle1");
Requires("Rectangle2");
Requires("Rectangle3");


class Scene extends TurnTableScene
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(false);

        // créer les deux rectangles
        this.m_Rectangle1 = new Rectangle1();
        this.m_Rectangle2 = new Rectangle2();
        this.m_Rectangle3 = new Rectangle3();

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.disable(gl.CULL_FACE);      // laisser voir le dos du rectangle

        // couleur du fond
        gl.clearColor(0.4, 0.4, 0.4, 0.0);

        // paramètres de la vue
        this.m_CameraAngleAzimut = 15.0;
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
        mat4.perspective(this.m_Mat4Projection, Utils.radians(10.0), width / height, 0.1, 100.0);
    }


    /**
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    onDraw(mat4Projection, mat4ModelView)
    {
        let mat4ModelViewTmp = mat4.create();

        // dessiner le premier rectangle à gauche
        mat4.translate(mat4ModelViewTmp, mat4ModelView, vec3.fromValues(-1.0, 0.0, 0.0));
        mat4.rotateY(mat4ModelViewTmp, mat4ModelViewTmp, Utils.radians(+40.0*Math.cos(Utils.Time)));
        this.m_Rectangle1.onDraw(mat4Projection, mat4ModelViewTmp);

        // dessiner le deuxième rectangle au milieu
        this.m_Rectangle2.onDraw(mat4Projection, mat4ModelView);

        // dessiner le troisième rectangle à droite
        mat4.translate(mat4ModelViewTmp, mat4ModelView, vec3.fromValues(+1.0, 0.0, 0.0));
        mat4.rotateY(mat4ModelViewTmp, mat4ModelViewTmp, Utils.radians(-40.0*Math.cos(Utils.Time)));
        this.m_Rectangle3.onDraw(mat4Projection, mat4ModelViewTmp);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Rectangle3.destroy();
        this.m_Rectangle2.destroy();
        this.m_Rectangle1.destroy();
        super.destroy();
    }
}
