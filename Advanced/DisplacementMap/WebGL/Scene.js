// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Misc/TrackBallScene.js");
Requires("Rectangle");


class Scene extends TrackBallScene
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(false);

        // créer l'objet
        this.m_Object = new Rectangle();

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.disable(gl.CULL_FACE);   // pour voir l'arrière du rectangle

        // couleur du fond
        gl.clearColor(0.2, 0.2, 0.2, 0.0);

        // paramètres de la vue
        this.m_CameraDistance = 12.0;
        quat.setAxisAngle(this.m_QuatP1P2, vec3.fromValues(-1,0.5,0), Utils.radians(30.0));
        quat.multiply(this.m_QuatOrientation, this.m_QuatP1P2, this.m_QuatOrientation);
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
     * @param mat4ModelView : matrice de vue
     */
    onDraw(mat4Projection, mat4ModelView)
    {
        // dessiner l'objet
        this.m_Object.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Object.destroy();
        super.destroy();
    }
}
