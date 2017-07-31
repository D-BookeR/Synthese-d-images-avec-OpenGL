// Définition de la classe Scene1

// superclasses et classes nécessaires
Requires("Tetraedre");


class Scene1
{
    /**
     * Constructeur
     */
    constructor()
    {
        // créer le tétraèdre qui est dessiné par cette scène
        this.m_Tetraedre = new Tetraedre();

        // paramètres de la vue
        this.m_Mat4Projection = mat4.create();
        this.m_CameraDistance = 13;
    }


    onSurfaceChanged(width, height)
    {
        // met en place le viewport
        gl.viewport(0, 0, width, height);

        // matrice de projection perspective
        mat4.perspective(this.m_Mat4Projection, Utils.radians(8.0), width / height, 0.1, 20.0);
    }


    /**
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    onDraw(mat4Projection, mat4ModelView)
    {
        // dessiner l'objet
        this.m_Tetraedre.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * dessine la scène
     */
    onDrawFrame()
    {
        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.enable(gl.CULL_FACE);

        // effacer l'écran avec une couleur grise claire
        gl.clearColor(0.9, 0.9, 0.9, 1.0);
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        // construire la matrice MV
        let mat4ModelView = mat4.create();

        // éloignement de la scène
        mat4.translate(mat4ModelView, mat4ModelView, vec3.fromValues(0.0, 0.0, -this.m_CameraDistance));

        // rotation du tétraèdre
        mat4.rotate(mat4ModelView, mat4ModelView, Utils.radians(25.0 * Utils.Time + 60.0), vec3.fromValues(1,1,0));

        this.onDraw(this.m_Mat4Projection, mat4ModelView);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Tetraedre.destroy();
    }
}
