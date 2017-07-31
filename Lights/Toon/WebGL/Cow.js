// Définition de la classe Cow

// superclasses et classes nécessaires
Requires("ToonMaterial");
Requires("libs/Material/ColorMaterial.js");
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleLoading.js");
Requires("libs/Mesh/MeshModuleDrawing.js");


class Cow
{
    /**
     * Constructeur
     * @param material : matériau à appliquer
     */
    constructor(material)
    {
        // créer le matériau pour les arêtes
        let black = vec4.fromValues(0.0, 0.0, 0.0, 1.0);
        this.m_EdgesMaterial = new ColorMaterial(black);

        // charger le maillage
        this.m_Mesh = new Mesh("Cow");
        let loader = new MeshModuleLoading(this.m_Mesh);
        loader.loadObjFile("data/models/Cow/Cow.obj", null, 0.15, this, this.onCowLoaded, material);
        this.m_VBOset = null;
        this.m_VBOsetEdges = null;
    }


    onCowLoaded(that, material)
    {
        // calculer les normales
        that.m_Mesh.computeNormals();

        // créer le VBOset
        let renderer = new MeshModuleDrawing(that.m_Mesh);
        that.m_VBOset = renderer.createStripVBOset(material, true);

        // créer le VBOset des aretes
        that.m_VBOsetEdges = renderer.createEdgesVBOset(that.m_EdgesMaterial);
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

        // dessiner l'objet
        this.m_VBOset.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Fonction de dessin des arêtes de l'objet
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    onDrawEdges(mat4Projection, mat4ModelView)
    {
        // dessiner l'objet s'il est prêt
        if (this.m_VBOsetEdges == null) return;

        // dessiner l'objet
        this.m_VBOsetEdges.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Mesh.destroy();
        this.m_VBOset.destroy();
        this.m_VBOsetEdges.destroy();
        this.m_EdgesMaterial.destroy();
    }
}
