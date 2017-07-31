// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Material/Texture2D.js");
Requires("InvertColors");


class Scene extends SceneBase
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(false);

        // charger la texture
        this.m_Texture = new Texture2D("data/textures/1024px-LandscapeArchPano.jpg");

        // traitement d'image
        this.m_InvertColors = new InvertColors();

        // désactiver les tests de profondeur
        gl.disable(gl.DEPTH_TEST);
    }


    /**
     * Dessine l'image courante
     */
    onDrawFrame()
    {
        // on applique le traitement d'image sur la texture et ça remplit l'écran
        this.m_InvertColors.process(this.m_Texture);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_InvertColors.destroy();
        this.m_Texture.destroy();
        super.destroy();
    }
}
