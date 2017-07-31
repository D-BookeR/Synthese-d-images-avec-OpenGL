// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Misc/TurnTableScene.js");
Requires("libs/Material/DeferredShadingMaterial.js");
Requires("libs/Material/Light.js");
Requires("libs/Misc/MeshStructureDraw.js");
Requires("ColorCube");


class Scene extends TurnTableScene
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(false);

        // créer le matériau de l'objet
        let Ks = vec3.fromValues(0.1, 0.1, 0.1);
        let Ns = 64;
        this.m_Material = new DeferredShadingMaterial(null, Ks, Ns);

        // définir une lampe ambiante
        this.m_Light = new Light();
        this.m_Light.setColor(vec3.fromValues(1.0,1.0,1.0));
        this.addLight(this.m_Light);

        // créer l'objet à dessiner
        this.m_Object = new ColorCube();

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.enable(gl.CULL_FACE);

        // couleur du fond
        gl.clearColor(0.4, 0.4, 0.4, 0.0);

        // paramètres de la vue
        this.m_CameraAngleAzimut = 50.0;
        this.m_CameraAngleElevation = 10.0;
        this.m_CameraDistance = 8;

        // modes de dessin : dessiner les arêtes par défaut
        this.m_MeshStructureDraw = new MeshStructureDraw(this.m_Object.getMesh(), this.m_Material, null, null, null, 0.5);
        this.m_MeshStructureDraw.nextEdgesMode();
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
     * appelée quand on appuie sur une touche du clavier
     * @param code : touche enfoncée
     */
    onKeyDown(code)
    {
        switch (code) {
        case 'T'.charCodeAt(0):
            this.m_MeshStructureDraw.nextTrianglesMode();
            break;
        case 'Y'.charCodeAt(0):
            this.m_MeshStructureDraw.nextEdgesMode();
            break;
        case 'U'.charCodeAt(0):
            this.m_MeshStructureDraw.nextNormalsMode();
            break;
        default:
            // appeler la méthode de la superclasse
            super.onKeyDown(code);
        }
    }


    /**
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    onDraw(mat4Projection, mat4ModelView)
    {
        // dessiner l'objet selon les modes demandés
        this.m_MeshStructureDraw.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_MeshStructureDraw.destroy();
        this.m_Object.destroy();
        this.m_Light.destroy();
        this.m_Material.destroy();
        super.destroy();
    }
}
