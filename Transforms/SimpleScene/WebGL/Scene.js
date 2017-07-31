// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Misc/SceneBase.js");
Requires("libs/Misc/GridXZ.js");
Requires("Pyramid");

class Scene extends SceneBase
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(false);

        // créer les objets à dessiner
        this.m_Pyramid = new Pyramid();
        this.m_Grid = new GridXZ();

        // définir la transformation M1
        this.m_matModel1 = mat4.create();
        mat4.translate(this.m_matModel1, this.m_matModel1, vec3.fromValues(-1.5, 1.0, 2.0));

        // définir la transformation M2
        this.m_matModel2 = mat4.create();
        mat4.translate(this.m_matModel2, this.m_matModel2, vec3.fromValues(2.0, 1.0, 1.0));
        mat4.rotateY(this.m_matModel2, this.m_matModel2, Utils.radians(120));

        // définir la transformation V
        this.m_matView = mat4.create();
        mat4.translate(this.m_matView, this.m_matView, vec3.fromValues(0,0,-10));
        mat4.rotateX(this.m_matView, this.m_matView, Utils.radians(32));

        // la transformation P est définie dans onSurfaceChanged
        this.m_matProjection = mat4.create();

        // couleur du fond
        gl.clearColor(0.9, 0.9, 0.9, 0.0);
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
        mat4.perspective(this.m_matProjection, Utils.radians(40.0), width / height, 4.0, 16.0);
    }


    /**
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection (égale à this.m_matP)
     * @param mat4View : matrice de vue (ignorée)
     */
    onDraw(mat4Projection, mat4View)
    {
        // dessiner la grille
        this.m_Grid.onDraw(this.m_matProjection, this.m_matView);

        // dessiner la pyramide n°1 (devant et à gauche)
        this.m_Pyramid.onDraw(this.m_matProjection, this.m_matView, this.m_matModel1);

        // dessiner la pyramide n°2 (à droite)
        this.m_Pyramid.onDraw(this.m_matProjection, this.m_matView, this.m_matModel2);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Grid.destroy();
        this.m_Pyramid.destroy();
        super.destroy();
    }
}
