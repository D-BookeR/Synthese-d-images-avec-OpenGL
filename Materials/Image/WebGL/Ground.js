// Définition de la classe Ground

// superclasses et classes nécessaires
Requires("libs/Material/TextureMaterial");
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleLoading.js");
Requires("libs/Mesh/MeshModuleDrawing.js");


class Ground
{
    /**
     * Constructeur
     */
    constructor()
    {
        // créer le matériau du terrain
        this.m_Texture = new Texture2D("data/models/Terrain/TerrainAt.png");
        this.m_Material = new TextureMaterial(this.m_Texture);

        // charger le maillage
        this.m_Mesh = new Mesh("Ground");
        let loader = new MeshModuleLoading(this.m_Mesh);
        loader.loadObjFile("data/models/Terrain/Terrain.obj", null, 1.0, this, this.onGroundLoaded, this.m_Material);
        this.m_VBOset = null;
    }


    onGroundLoaded(that, material)
    {
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
        // dessiner le terrain s'il est prêt
        if (this.m_VBOset != null) {
            this.m_VBOset.onDraw(mat4Projection, mat4ModelView);
        }
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Mesh.destroy();
        this.m_Material.destroy();
        this.m_Texture.destroy();
        this.m_VBOset.destroy();
    }
}
