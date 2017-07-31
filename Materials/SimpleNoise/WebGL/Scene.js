// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Misc/SceneBase.js");
Requires("Rectangle");


class Scene
{
    /**
     * Constructeur
     */
    constructor()
    {
        // créer l'objet
        this.m_Object = new Rectangle();

        // matrices de projection et vue = identité
        this.m_Identity = mat4.create();
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
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    onDraw(mat4Projection, mat4ModelView)
    {
    }


    /**
     * Dessine la scène
     */
    onDrawFrame()
    {
        // dessiner l'objet
        this.m_Object.onDraw(this.m_Identity, this.m_Identity);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Object.destroy();
    }
}
