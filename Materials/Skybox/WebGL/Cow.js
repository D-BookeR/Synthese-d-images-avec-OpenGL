// Définition de la classe Cow

// superclasses et classes nécessaires
Requires("libs/Material/DeferredShadingMaterial.js");
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleLoading.js");
Requires("libs/Mesh/MeshModuleDrawing.js");


class Cow
{
    /**
     * Constructeur
     */
    constructor()
    {
        // créer un matériau
        let Kd = vec4.fromValues(0.2, 0.8, 0.2, 1.0);
        let Ks = vec3.fromValues(0.9, 0.9, 0.9);
        let Ns = 128;
        this.m_Material = new DeferredShadingMaterial(Kd, Ks, Ns)

        // créer un maillage
        this.m_Mesh = new Mesh("Cow");
        let loader = new MeshModuleLoading(this.m_Mesh);
        loader.loadObjFile("data/models/Cow/Cow.obj", null, 0.05, this, this.onCowLoaded);
        this.m_VBOset = null;
    }


    onCowLoaded(that)
    {
        // calculer les normales
        that.m_Mesh.computeNormals();

        // créer le VBOset
        let renderer = new MeshModuleDrawing(that.m_Mesh);
        that.m_VBOset = renderer.createStripVBOset(that.m_Material, true);
    }


    /**
     * dessine l'objet sur l'écran
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    onDraw(mat4Projection, mat4ModelView)
    {
        // dessiner le maillage s'il est prêt
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
        this.m_VBOset.destroy();
    }
}
