// Définition de la classe Teapot

// superclasses et classes nécessaires
Requires("NormalMaterial");
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleLoading.js");
Requires("libs/Mesh/MeshModuleDrawing.js");


/**
 * Cette fonction définit la classe Teapot = maillage recouvert par une texture.
 * @param texture : instance de TextureCube
 */
class Teapot
{
    /**
     * Constructeur
     */
    constructor()
    {
        // créer le matériau de la théière
        this.m_Material = new NormalMaterial();

        // charger le maillage
        this.m_Mesh = new Mesh("Teapot");
        let loader = new MeshModuleLoading(this.m_Mesh);
        loader.loadObjFile("data/models/Teapot/teapot.obj", null, 0.02, this, this.onTeapotLoaded, this.m_Material);
    //    loader.loadObjFile("data/models/Sphere/sphere.obj", null, 0.3, this, this.onTeapotLoaded, this.m_Material);
    //    loader.loadObjFile("data/models/Cow/Cow.obj", null, 0.05, this, this.onTeapotLoaded, this.m_Material);
        this.m_VBOset = null;
    }


    onTeapotLoaded(that, material)
    {
        // calculer les normales
        that.m_Mesh.computeNormals();

        // calculer les tangentes
        that.m_Mesh.computeTangents();

        // créer le VBOset
        let renderer = new MeshModuleDrawing(that.m_Mesh);
        that.m_VBOset = renderer.createStripVBOset(material, true);
    }


    /**
     * dessine l'objet sur l'écran
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    onDraw(mat4Projection, mat4ModelView)
    {
        // dessiner l'objet s'il est prêt
        if (this.m_VBOset == null) return;

        // construire la matrice MV finale
        let mat4ModelViewTeapot = mat4.clone(mat4ModelView);

        // décaler la théière vers le bas
        mat4.translate(mat4ModelViewTeapot, mat4ModelViewTeapot, vec3.fromValues(0.0,-0.7,0.0));

        // dessiner l'objet
        this.m_VBOset.onDraw(mat4Projection, mat4ModelViewTeapot);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Mesh.destroy();
        this.m_Material.destroy();
    }
}
