// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Misc/TurnTableScene.js");
Requires("libs/Material/DeferredShadingMaterial.js");
Requires("libs/Material/OmniLight.js");
Requires("libs/Misc/MeshStructureDraw.js");
Requires("CubeOK");


class Scene extends TurnTableScene
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(true);

        // créer le matériau de l'objet
        let Kd = vec4.fromValues(0.2, 0.5, 0.7, 1.0);
        let Ks = vec3.fromValues(0.1, 0.1, 0.1);
        let Ns = 64;
        this.m_Material = new DeferredShadingMaterial(Kd, Ks, Ns);

        // définir une lampe directionnelle
        this.m_Light0 = new OmniLight();
        this.m_Light0.setPosition(vec4.fromValues(1, 1, 1, 0));
        this.m_Light0.setColor(vec3.fromValues(1.0,1.0,1.0));
        this.addLight(this.m_Light0);

        // définir une lampe positionnelle
        this.m_Light1 = new OmniLight();
        this.m_Light1.setPosition(vec4.fromValues(-10, 10, 5, 1));
        this.m_Light1.setColor(vec3.fromValues(150,150,150));
        this.addLight(this.m_Light1);

        // créer l'objet à dessiner
        this.m_Object = new CubeOK();

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.enable(gl.CULL_FACE);

        // couleur du fond
        gl.clearColor(0.4, 0.4, 0.4, 0.0);

        // paramètres de la vue
        this.m_CameraAngleAzimut = 30.0;
        this.m_CameraAngleElevation = 20.0;
        this.m_CameraDistance = 7;

        // modes de dessin
        this.m_MeshStructureDraw = new MeshStructureDraw(this.m_Object.getMesh(), this.m_Material, null, null, null, 0.5);
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
        this.m_Light1.destroy();
        this.m_Light0.destroy();
        this.m_Material.destroy();
        super.destroy();
    }
}
