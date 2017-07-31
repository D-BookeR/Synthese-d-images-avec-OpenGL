// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("RedTriangle");
Requires("GreenTriangle");


class Scene
{
    /** constructeur */
    constructor()
    {
        // créer les objets à dessiner
        this.m_RedTriangle = new RedTriangle();
        this.m_GreenTriangle = new GreenTriangle(0.75);

        // couleur du fond : gris foncé
        gl.clearColor(0.4, 0.4, 0.4, 0.0);
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
        gl.clear(gl.COLOR_BUFFER_BIT);

        // dessiner le triangle rouge
        this.m_RedTriangle.onDraw();

        // mettre en place le blending
        gl.enable(gl.BLEND);

        // formule de blending
        gl.blendEquation(gl.FUNC_ADD);
        //gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);
        gl.blendFunc(gl.ONE, gl.SRC_COLOR);

        // dessiner le triangle vert
        this.m_GreenTriangle.onDraw();

        // arrêter le blending
        gl.disable(gl.BLEND);
    }


    /** supprime tous les objets de cette scène */
    destroy()
    {
        this.m_GreenTriangle.destroy();
        this.m_RedTriangle.destroy();
    }
}
