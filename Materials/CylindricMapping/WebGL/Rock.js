// Définition de la classe Rock

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleLoading.js");
Requires("libs/Mesh/MeshModuleProcessing.js");
Requires("libs/Mesh/MeshModuleDrawing.js");
Requires("libs/Material/TextureMaterial");


class Rock
{
    /**
     * Constructeur
     * @param texture Texture2D à appliquer sur l'objet
     * @param cylindre englobant
     */
    constructor(texture, cylindre)
    {
        // créer le matériau
        this.m_Material = new TextureMaterial(texture);

        // cylindre à modifier quand le rocher sera chargé
        this.m_Cylinder = cylindre;

        // charger le maillage
        this.m_Mesh = new Mesh("Rock");
        let loader = new MeshModuleLoading(this.m_Mesh);
        loader.loadObjFile("data/models/Rock1/Rock1.obj", null, 0.5, this, this.onRockLoaded, this.m_Material);
        this.m_VBOset = null;
    }


    onRockLoaded(that, material)
    {
        // module de transformation sur le maillage
        let transformation = new MeshModuleProcessing(that.m_Mesh);

        // décaler tous les sommets
        let translation = mat4.create();
        mat4.translate(translation, translation, vec3.fromValues(0.0, -0.2, 0.0));
        transformation.transform(translation);

        // subdiviser le maillage pour avoir assez de sommets
        that.m_Mesh.computeNormals();
        transformation.subdivideAll(that.m_Mesh.getTriangleList(), 2, 0.0);

        // déterminer la hauteur minimale et maximale (idem MeshModuleUtils.creerAABB)
        let vmin = vec3.clone(that.m_Mesh.getVertex(0).getCoord());
        let vmax = vec3.clone(that.m_Mesh.getVertex(0).getCoord());
        for (let vertex of that.m_Mesh.getVertexList()) {
            vec3.min(vmin, vmin, vertex.getCoord());
            vec3.max(vmax, vmax, vertex.getCoord());
        }
        let minY = vmin[1];
        let maxY = vmax[1];

        // calculer les coordonnées de texture de chaque sommet
        for (let vertex of that.m_Mesh.getVertexList()) {
            // coordonnées 3D du sommet
            let coords = vertex.getCoord();

            // transformation en coordonnées cylindriques
            let azimut = Math.atan2(coords[0], coords[2]);  // attention x joue le rôle de y et z celui de x
            let height = coords[1];

            // ramener les coordonnées cylindriques dans l'intervalle [0,1]
            let s = ((azimut/Math.PI) + 1.0) / 2.0;
            let t = (height - minY)/(maxY - minY);

            // affectation des coordonnées de texture
            let texcoord = vec2.fromValues(s, t);
            vertex.setTexCoord(texcoord);
        }

        // créer le VBOset
        let renderer = new MeshModuleDrawing(that.m_Mesh);
        that.m_VBOset = renderer.createStripVBOset(material, true);

        // modifier le cylindre (une fois que le rocher est chargé)
        that.m_Cylinder.updateMinMaxY(minY, maxY);
    }


    /**
     * dessine l'objet sur l'écran
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    onDraw(mat4Projection, mat4ModelView)
    {
        // dessiner le VBO s'il est prêt
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
