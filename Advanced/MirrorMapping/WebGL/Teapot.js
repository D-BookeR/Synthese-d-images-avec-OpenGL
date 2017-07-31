// Définition de la classe Teapot

// superclasses et classes nécessaires
Requires("MirrorMaterial");
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleLoading.js");
Requires("libs/Mesh/MeshModuleDrawing.js");


class Teapot
{
    /**
     * Cette fonction définit la classe Teapot = maillage recouvert par une texture.
     * @param texture : instance de TextureCube
     */
    constructor(texture)
    {
        // créer le matériau de la théière
        this.m_Material = new MirrorMaterial(texture);

        // charger le maillage
        this.m_Mesh = new Mesh("Teapot");
        let loader = new MeshModuleLoading(this.m_Mesh);
        loader.loadObjFile("data/models/Teapot/teapot.obj", null, 0.005, this, this.onTeapotLoaded, this.m_Material);
        //loader.loadObjFile("data/models/Sphere/sphere.obj", null, 0.3, this, this.onTeapotLoaded, this.m_Material);
        //loader.loadObjFile("data/models/Cow/Cow.obj", null, 0.05, this, this.onTeapotLoaded, this.m_Material);
        //loader.loadObjFile("data/models/Apple/apple.obj", null, 0.008, this, this.onTeapotLoaded, this.m_Material);
        this.m_VBOset = null;
    }


    onTeapotLoaded(that, material)
    {
        // calculer les normales
        that.m_Mesh.computeNormals();

        // créer le VBOset
        let renderer = new MeshModuleDrawing(that.m_Mesh);
        that.m_VBOset = renderer.createStripVBOset(material, true);
    }


    /**
     * dessine l'objet sur l'écran
     * @param mat4Projection : matrice de projection
     * @param mat4ModelViewScene : matrice de vue
     */
    onDraw(mat4Projection, mat4ModelViewScene)
    {
        // dessiner l'objet s'il est prêt
        if (this.m_VBOset == null) return;

        // fournir la matrice mat4ModelViewScene au matériau
        this.m_Material.setModelViewScene(mat4ModelViewScene);

        // matrice ModelView pour afficher la théière
        let mat4ModelViewTeapot = mat4.create();

        // appliquer une rotation en fonction du temps
        mat4.rotateX(mat4ModelViewTeapot, mat4ModelViewScene, Utils.radians(Utils.Time * 15.0));
        mat4.rotateY(mat4ModelViewTeapot, mat4ModelViewTeapot, Utils.radians(Utils.Time * 32.0));

        // décaler la théière vers le bas
        mat4.translate(mat4ModelViewTeapot, mat4ModelViewTeapot, vec3.fromValues(0.0,-0.2,0.0));

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
        this.m_VBOset.destroy();
    }
}
