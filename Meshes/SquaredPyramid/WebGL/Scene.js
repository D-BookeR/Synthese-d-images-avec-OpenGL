// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Misc/TurnTableScene.js");
Requires("libs/Material/DeferredShadingMaterial.js");
Requires("libs/Material/OmniLight.js");
Requires("libs/Misc/MeshStructureDraw.js");
Requires("SquaredPyramid");


class Scene extends TurnTableScene
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(true);

        // créer le matériau de l'objet
        let Ks = vec3.fromValues(0.4, 0.4, 0.4);
        let Ns = 200;
        this.m_Material = new DeferredShadingMaterial(null, Ks, Ns);

        // définir une lampe ambiante
        this.m_Light = new Light();
        this.m_Light.setColor(vec3.fromValues(1.0,1.0,1.0));
        this.addLight(this.m_Light);

        // créer l'objet à dessiner
        this.m_Object = new SquaredPyramid(this.m_Material);

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.enable(gl.CULL_FACE);

        // couleur du fond
        gl.clearColor(0.4, 0.4, 0.4, 0.0);

        // paramètres de la vue
        this.m_CameraAngleAzimut = 210.0;
        this.m_CameraAngleElevation = 10.0;
        vec3.set(this.m_CameraPivot, 0.0, +0.2, 0.0);
        this.m_CameraDistance = 7;
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
        mat4.perspective(this.m_Mat4Projection, Utils.radians(25.0), width / height, 0.1, 20.0);
    }


    /**
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    onDraw(mat4Projection, mat4ModelView)
    {
        // dessiner l'objet selon les modes demandés
        this.m_Object.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Object.destroy();
        this.m_Light.destroy();
        this.m_Material.destroy();
        super.destroy();
    }
}
