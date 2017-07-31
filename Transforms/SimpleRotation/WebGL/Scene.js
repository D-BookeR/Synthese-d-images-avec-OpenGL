// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("TriangleGLSL");
Requires("TriangleProg");


class Scene
{
    /** constructeur */
    constructor()
    {
        // créer les objets à dessiner
        this.m_TriangleGLSL = new TriangleGLSL();
        this.m_TriangleProg = new TriangleProg();

        // couleur du fond : gris foncé
        gl.clearColor(0.4, 0.4, 0.4, 0.0);

        // viewport
        this.m_Width = 0;
        this.m_Height = 0;
    }


    /**
     * appelée quand la taille de la vue OpenGL change
     * @param width : largeur en nombre de pixels de la fenêtre
     * @param height : hauteur en nombre de pixels de la fenêtre
     */
    onSurfaceChanged(width, height)
    {
        // mémorise le viewport
        this.m_Width = width/2;
        this.m_Height = height;
    }


    /**
     * Dessine l'image courante
     */
    onDrawFrame()
    {
        // effacer l'écran
        gl.clear(gl.COLOR_BUFFER_BIT);

        // viewport pour la vue de gauche
        gl.viewport(0, 0, this.m_Width, this.m_Height);

        // dessiner le triangle géré par GLSL
        this.m_TriangleGLSL.onDraw();

        // viewport pour la vue de droite
        gl.viewport(this.m_Width, 0, this.m_Width, this.m_Height);

        // dessiner le triangle géré par programme
        this.m_TriangleProg.onDraw();
    }


    /** supprime tous les objets de cette scène */
    destroy()
    {
        this.m_TriangleGLSL.destroy();
        this.m_TriangleProg.destroy();
    }
}
