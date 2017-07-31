// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Misc/TurnTableScene.js");
Requires("Cube3Mats");
Requires("Pyramid");


class Scene extends TurnTableScene
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(false);

        // créer les objets
        this.m_Cube = new Cube3Mats();
        this.m_Pyramid = new Pyramid();

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.enable(gl.CULL_FACE);

        // couleur du fond
        gl.clearColor(0.4, 0.4, 0.4, 0.0);

        // paramètres de la vue
        this.m_CameraAngleAzimut = -15.0;
        this.m_CameraAngleElevation = 30.0;
        this.m_CameraDistance = 10.0;
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
        mat4.perspective(this.m_Mat4Projection, Utils.radians(20.0), width / height, 5.0, 20.0);
    }


    /**
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    onDraw(mat4Projection, mat4ModelView)
    {
        // matrice temporaire
        let mat4ModelViewTmp = mat4.create();

        // dessiner le cube
        mat4.translate(mat4ModelViewTmp, mat4ModelView, vec3.fromValues(-1, 0, 0));
        mat4.scale(mat4ModelViewTmp, mat4ModelViewTmp, vec3.fromValues(0.75, 0.75, 0.75));
        this.m_Cube.onDraw(mat4Projection, mat4ModelViewTmp);

        // dessiner la pyramide
        mat4.translate(mat4ModelViewTmp, mat4ModelView, vec3.fromValues(+1, 0, 0));
        mat4.scale(mat4ModelViewTmp, mat4ModelViewTmp, vec3.fromValues(0.75, 0.75, 0.75));
        this.m_Pyramid.onDraw(mat4Projection, mat4ModelViewTmp);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Pyramid.destroy();
        this.m_Cube.destroy();
        super.destroy();
    }
}
