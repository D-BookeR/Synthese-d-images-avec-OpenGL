// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Misc/TurnTableScene.js");
Requires("libs/Material/DeferredShadingMaterial.js");
Requires("libs/Material/Light.js");
Requires("libs/Misc/MeshStructureDraw.js");
Requires("Lego4");
Requires("Lego8");


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

        // créer les objets à dessiner
        this.m_Lego4 = new Lego4();
        this.m_Lego8 = new Lego8();

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.enable(gl.CULL_FACE);

        // couleur du fond
        gl.clearColor(0.4, 0.4, 0.4, 0.0);

        // paramètres de la vue
        this.m_CameraAngleAzimut = 40.0;
        this.m_CameraAngleElevation = 40.0;
        this.m_CameraDistance = 10;

        // modes de dessin : dessiner les arêtes par défaut
        this.m_MeshStructureLego4 = new MeshStructureDraw(this.m_Lego4.getMesh(), this.m_Material, null, null, null, 0.5);
        this.m_MeshStructureLego4.nextEdgesMode();
        this.m_MeshStructureLego8 = new MeshStructureDraw(this.m_Lego8.getMesh(), this.m_Material, null, null, null, 0.5);
        this.m_MeshStructureLego8.nextEdgesMode();
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
        mat4.perspective(this.m_Mat4Projection, Utils.radians(20.0), width / height, 0.1, 20.0);
    }


    /**
     * appelée quand on appuie sur une touche du clavier
     * @param code : touche enfoncée
     */
    onKeyDown(code)
    {
        switch (code) {
        case 'T'.charCodeAt(0):
            this.m_MeshStructureLego4.nextTrianglesMode();
            this.m_MeshStructureLego8.nextTrianglesMode();
            break;
        case 'Y'.charCodeAt(0):
            this.m_MeshStructureLego4.nextEdgesMode();
            this.m_MeshStructureLego8.nextEdgesMode();
            break;
        case 'U'.charCodeAt(0):
            this.m_MeshStructureLego4.nextNormalsMode();
            this.m_MeshStructureLego8.nextNormalsMode();
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
        // matrice MV pour dessiner l'objet 1
        let mat4ModelViewObject1 = mat4.create();
        mat4.translate(mat4ModelViewObject1, mat4ModelView, vec3.fromValues(-1.2, 0.0, 0.0));
        mat4.scale(mat4ModelViewObject1, mat4ModelViewObject1, vec3.fromValues(0.6, 0.6, 0.6));

        // dessiner l'objet selon les modes demandés
        this.m_MeshStructureLego4.onDraw(mat4Projection, mat4ModelViewObject1);

        // matrice MV pour dessiner l'objet 2
        let mat4ModelViewObject2 = mat4.create();
        mat4.translate(mat4ModelViewObject2, mat4ModelView, vec3.fromValues(+1.2, 0.0, 0.0));
        mat4.scale(mat4ModelViewObject2, mat4ModelViewObject2, vec3.fromValues(0.4, 0.4, 0.4));

        // dessiner l'objet selon les modes demandés
        this.m_MeshStructureLego8.onDraw(mat4Projection, mat4ModelViewObject2);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_MeshStructureLego8.destroy();
        this.m_MeshStructureLego4.destroy();
        this.m_Lego8.destroy();
        this.m_Lego4.destroy();
        this.m_Light.destroy();
        this.m_Material.destroy();
        super.destroy();
    }
}
