// Définition de la classe Rectangle

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleDrawing.js");
Requires("libs/Material/ColorMaterial.js");
Requires("libs/Material/TextureMaterial");


class Rectangle
{
    /**
     * Constructeur
     */
    constructor()
    {
        // créer le maillage
        this.m_Mesh = new Mesh("Rectangle");

        // tableau des coordonnées de texture
        // NB: la première ligne donne les coordonnées des sommets du bas du rectangle
        let texcoords = [
            0.0, 0.0,       0.5, 0.0,       1.0, 0.0,
            0.0, 0.5,       0.5, 0.5,       1.0, 0.5,
            0.0, 1.0,       0.5, 1.0,       1.0, 1.0,
        ];

        // créer les sommets
        let itc = 0;
        for (let z=+1; z>=-1; z--) {
            for (let x=-1; x<=+1; x++) {
                let vertex = this.m_Mesh.addVertex(null);
                vertex.setCoord(vec3.fromValues(x,0,z));
                // affecter les coordonnées de texture de ce sommet
                let texcoord = vec2.fromValues(texcoords[itc+0],texcoords[itc+1]);
                itc += 2;
                vertex.setTexCoord(texcoord);
            }
        }

        // créer les quads (automatiquement transformés en triangles)
        for (let z=0; z<2; z++) {
            for (let x=0; x<2; x++) {
                // récupérer les 4 sommets
                let s00 = this.m_Mesh.getVertex((x+0)+(z+0)*3);
                let s10 = this.m_Mesh.getVertex((x+1)+(z+0)*3);
                let s01 = this.m_Mesh.getVertex((x+0)+(z+1)*3);
                let s11 = this.m_Mesh.getVertex((x+1)+(z+1)*3);
                this.m_Mesh.addQuad(s00, s10, s11, s01);
            }
        }

        // créer la texture du rectangle
        this.m_Texture = new Texture2D("data/textures/earth/earth_map.jpg");

        // créer le matériau du rectangle
        this.m_Material = new TextureMaterial(this.m_Texture);

        // créer les VBO pour afficher ce maillage
        let renderer = new MeshModuleDrawing(this.m_Mesh);
        this.m_VBOset = renderer.createStripVBOset(this.m_Material, true);

        // créer le matériau jaune pour dessiner les arêtes
        this.m_EdgesMaterial = new ColorMaterial(vec4.fromValues(1,1,0,1));

        // créer le VBOset
        this.m_VBOsetEdges = renderer.createEdgesVBOset(this.m_EdgesMaterial);
    }


    /**
     * dessine l'objet sur l'écran
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    onDraw(mat4Projection, mat4ModelView)
    {
        // mettre en place un décalage de polygone
        gl.enable(gl.POLYGON_OFFSET_FILL);
        gl.polygonOffset(1.0, 1.0);

        // dessiner les triangles
        this.m_VBOset.onDraw(mat4Projection, mat4ModelView);

        // supprimer le décalage
        gl.polygonOffset(0, 0);
        gl.disable(gl.POLYGON_OFFSET_FILL);

        // dessiner les arêtes
        gl.lineWidth(1.5);
        this.m_VBOsetEdges.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_EdgesMaterial.destroy();
        this.m_Material.destroy();
        this.m_Texture.destroy();
        this.m_VBOset.destroy();
        this.m_Mesh.destroy();
    }
}
