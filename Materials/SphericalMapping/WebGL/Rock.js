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
     */
    constructor(texture)
    {
        // créer le matériau
        this.m_Material = new TextureMaterial(texture);

        // charger le maillage
        this.m_Mesh = new Mesh("Rock");
        let loader = new MeshModuleLoading(this.m_Mesh);
        loader.loadObjFile("data/models/Rock1/Rock1.obj", null, 0.5, this, this.onRockLoaded, this.m_Material);
        this.m_VBOset = null;
    }


    onRockLoaded(that, material)
    {
        // subdiviser le maillage pour avoir assez de sommets
        let transformation = new MeshModuleProcessing(that.m_Mesh);
        that.m_Mesh.computeNormals();
        transformation.subdivideAll(that.m_Mesh.getTriangleList(), 2, 0.0);

        // recalculer les coordonnées de texture de chaque sommet
        for (let vertex of that.m_Mesh.getVertexList()) {
            // coordonnées 3D du sommet
            let coords = vertex.getCoord();

            // translation du maillage
            vec3.add(coords, coords, vec3.fromValues(0.0, -0.2, 0.0));

            // transformation en coordonnées sphériques
            let norm = vec3.length(coords);
            let azimut = Math.atan2(coords[0], coords[2]);  // attention x joue le rôle de y et z celui de x
            let height = Math.asin(coords[1] / norm);

            // ramener les coordonnées sphériques dans l'intervalle [0,1]
            let s = ((azimut/Math.PI) + 1.0) / 2.0;
            let t = (height/Math.PI) + 0.5;

            // affectation des coordonnées de texture
            let texcoord = vec2.fromValues(s, t);
            vertex.setTexCoord(texcoord);
        }

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
