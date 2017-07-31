// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Misc/TurnTableScene.js");
Requires("libs/Material/OmniLight.js");
Requires("GroundMaterial");
Requires("Ground");


class Scene extends TurnTableScene
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(true);

        // créer le matériau de l'objet
        let Kd = "data/models/TerrainHM/terrain_tx.jpg";
        let Ks = vec3.fromValues(0.1, 0.1, 0.1);
        let Ns = 64;
        this.m_Material = new GroundMaterial("data/models/TerrainHM/terrain_hm.png", 0.3, 0.005, Kd, Ks, Ns);

        // créer l'objet
        this.m_Ground = new Ground(120, this.m_Material);

        // définir une lampe directionnelle
        this.m_Light0 = new OmniLight();
        this.m_Light0.setPosition(vec4.fromValues(1, 1, 1, 0));
        this.m_Light0.setColor(vec3.fromValues(1.5,1.5,1.5));
        this.addLight(this.m_Light0);

        // définir une lampe directionnelle
        this.m_Light1 = new OmniLight();
        this.m_Light1.setPosition(vec4.fromValues(-2, 1, -2, 0));
        this.m_Light1.setColor(vec3.fromValues(0.9,0.9,0.9));
        this.addLight(this.m_Light1);

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.enable(gl.CULL_FACE);

        // couleur du fond
        gl.clearColor(0.4, 0.4, 0.4, 0.0);

        // paramètres de la vue
        this.m_CameraAngleAzimut = 30.0;
        this.m_CameraAngleElevation = 30.0;
        vec3.set(this.m_CameraPivot, 0.0, -0.05, 0.0);
        this.m_CameraDistance = 4.2;
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
        this.m_Ground.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Ground.destroy();
        this.m_Light1.destroy();
        this.m_Light0.destroy();
        this.m_Material.destroy();
        super.destroy();
    }
}
