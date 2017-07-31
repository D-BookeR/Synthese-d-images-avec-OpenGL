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
        this.m_GreenTriangle = new GreenTriangle();

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
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT );

        // créer une matrice de rotation
        let rotation = mat4.create();
        mat4.rotateY(rotation, mat4.create(), Utils.radians(160.0));

        // dessiner les triangles
        this.m_RedTriangle.onDraw(rotation);
        this.m_GreenTriangle.onDraw(rotation);
    }


    /** supprime tous les objets de cette scène */
    destroy()
    {
        this.m_GreenTriangle.destroy();
        this.m_RedTriangle.destroy();
    }
}
