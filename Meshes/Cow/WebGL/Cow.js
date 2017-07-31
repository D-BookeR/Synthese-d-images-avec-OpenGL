// Définition de la classe Cow

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleLoading.js");
Requires("libs/Mesh/MeshModuleRedux.js");
Requires("libs/Mesh/MeshModulePhysics.js");


class Cow
{
    /**
     * Constructeur
     * @param nombre de sommets à supprimer par simplification de maillage
     */
    constructor(nombre)
    {
        this.m_Mesh = new Mesh("Cow");
        let loader = new MeshModuleLoading(this.m_Mesh);
        loader.loadObjFile("data/models/Cow/Cow.obj", null, 0.15, this, this.onCowLoaded, nombre);
        this.m_VBOset = null;
    }


    onCowLoaded(that, nombre)
    {
        // définir la couleur de tous les sommets
        let vertex_count = that.m_Mesh.getVertexCount();
        for (let i=0; i<vertex_count; i++) {
            that.m_Mesh.getVertex(i).setColor(vec4.fromValues(1.0, 0.7, 0.6, 1.0));
        }
        console.log("La vache contient "+that.m_Mesh.getTriangleCount()+" triangles");

        // appliquer une réduction : supprimer un certain nombre de sommets
        let reduction = new MeshModuleRedux(that.m_Mesh);
        that.m_Mesh.computeNormals();
        reduction.reduxCount(nombre);
        console.log("Après réduction, la vache contient seulement "+that.m_Mesh.getTriangleList().length+" triangles");

        // calculer les informations physiques
        let physique = new MeshModulePhysics(that.m_Mesh);
        physique.setDensity(1.0);
        physique.compVolumeIntegrals();
    }


    /**
     * fournit le maillage de cet objet
     * @return maillage
     */
    getMesh()
    {
        return this.m_Mesh;
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Mesh.destroy();
    }
}
