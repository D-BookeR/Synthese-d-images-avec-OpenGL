// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("Tetraedre");


class Scene
{
    /** constructeur */
    constructor()
    {
        // créer les objets à dessiner
        this.m_Tetraedre = new Tetraedre();

        // couleur du fond : gris foncé
        gl.clearColor(0.4, 0.4, 0.4, 0.0);

        // activer le depth buffer
        gl.enable(gl.DEPTH_TEST);
        gl.depthFunc(gl.LESS);
    }


    /**
     * appelée quand la taille de la vue OpenGL change
     * @param width : largeur en nombre de pixels de la fenêtre
     * @param height : hauteur en nombre de pixels de la fenêtre
     */
    onSurfaceChanged(width, height)
    {
        // met en place le viewport
        gl.viewport(0, 0, width, height);
    }


    /**
     * Dessine l'image courante
     */
    onDrawFrame()
    {
        // effacer l'écran
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        // créer une matrice de rotation
        let rotation = mat4.create();
        mat4.rotate(rotation, mat4.create(), Utils.radians(25.0 * Utils.Time), vec3.fromValues(1,2,0));

        // dessiner le tétraèdre
        this.m_Tetraedre.onDraw(rotation);
    }


    /** supprime tous les objets de cette scène */
    destroy()
    {
        this.m_Tetraedre.destroy();
    }
}
