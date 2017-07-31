// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Misc/TurnTableScene.js");
Requires("libs/Material/OmniLight.js");
Requires("libs/Misc/GridXZ.js");
Requires("libs/Misc/AxesXYZ.js");


class Scene extends TurnTableScene
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(false);

        // créer les objets à dessiner
        this.m_Plane = new MeshObjectFromObj("data/models/Avion", "avion.obj", "avion.mtl", 1.0);
        this.m_Grid = new GridXZ(5, 1.0, 1.0, 4.0);
        this.m_Axes = new AxesXYZ(2.0, 1.0, 9.0);

        // définir une lampe directionnelle
        this.m_Light0 = new OmniLight();
        this.m_Light0.setPosition(vec4.fromValues(-1, 1, 0, 0));
        this.m_Light0.setColor(vec3.fromValues(1.0,1.0,1.0));
        this.addLight(this.m_Light0);

        // définir une lampe positionnelle
        this.m_Light1 = new OmniLight();
        this.m_Light1.setPosition(vec4.fromValues(5, 5, 5, 1));
        this.m_Light1.setColor(vec3.fromValues(20,20,20));
        this.addLight(this.m_Light1);

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.enable(gl.CULL_FACE);

        // couleur du fond
        gl.clearColor(0.60, 0.65, 0.70, 0.0);

        // paramètres de la vue
        this.m_CameraAngleAzimut = 20.0;
        this.m_CameraAngleElevation = 30.0;
        this.m_CameraDistance = 22;
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
        mat4.perspective(this.m_Mat4Projection, Utils.radians(20.0), width / height, 1.0, 100.0);
    }


    /**
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4View : matrice de vue
     */
    onDraw(mat4Projection, mat4View)
    {
        // matrice temporaire
        let mat4ModelView = mat4.create();

        // dessiner la grille et le trièdre
        this.m_Grid.onDraw(mat4Projection, mat4View);
        this.m_Axes.onDraw(mat4Projection, mat4View);

        /// dessin de l'avion de gauche

        // positionnement de l'avion sur la grille
        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(-3,1,0));

        // rotation de l'avion sur l'axe X (tangage)
        mat4.rotateX(mat4ModelView, mat4ModelView, Utils.radians(20.0 * Math.sin(Utils.Time)));

        // dessiner un trièdre non soumis à l'homothétie
        this.m_Axes.onDraw(mat4Projection, mat4ModelView);

        // agrandissement de l'avion
        mat4.scale(mat4ModelView, mat4ModelView, vec3.fromValues(5,5,5));

        // dessiner l'avion
        this.m_Plane.onDraw(mat4Projection, mat4ModelView);


        /// dessin de l'avion de droite

        // positionnement de l'avion sur la grille
        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(+3,1,0));

        // rotation de l'avion sur l'axe Y (lacet)
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(45.0));

        // rotation de l'avion sur l'axe X (tangage)
        mat4.rotateX(mat4ModelView, mat4ModelView, Utils.radians(20.0));

        // rotation de l'avion sur l'axe Z (roulis)
        mat4.rotateZ(mat4ModelView, mat4ModelView, Utils.radians(20.0 * Math.sin(Utils.Time)));

        // dessiner un trièdre non soumis à l'homothétie
        this.m_Axes.onDraw(mat4Projection, mat4ModelView);

        // agrandissement de l'avion
        mat4.scale(mat4ModelView, mat4ModelView, vec3.fromValues(5,5,5));

        // dessiner l'avion
        this.m_Plane.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Light1.destroy();
        this.m_Light0.destroy();
        this.m_Axes.destroy();
        this.m_Grid.destroy();
        this.m_Plane.destroy();
        super.destroy();
    }
}
