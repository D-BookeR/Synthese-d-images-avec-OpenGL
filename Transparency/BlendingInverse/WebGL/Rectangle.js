// Définition de la classe Rectangle

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleDrawing.js");
Requires("TextureMaterial");


class Rectangle
{
    /**
     * Constructeur
     * @param nomimage : nom complet du fichier à charger
     * @param width : largeur du rectangle
     * @param height : hauteur du rectangle
     */
    constructor(nomimage, width, height)
    {
        // créer le maillage
        this.m_Mesh = new Mesh(nomimage);

        // sommets
        let P0 = this.m_Mesh.addVertex("P0");
        P0.setCoord(vec3.fromValues(-width, -height, 0));
        P0.setTexCoord(vec2.fromValues(0,0));
        let P1 = this.m_Mesh.addVertex("P1");
        P1.setCoord(vec3.fromValues(+width, -height, 0));
        P1.setTexCoord(vec2.fromValues(1,0));
        let P2 = this.m_Mesh.addVertex("P2");
        P2.setCoord(vec3.fromValues(+width, +height, 0));
        P2.setTexCoord(vec2.fromValues(1,1));
        let P3 = this.m_Mesh.addVertex("P3");
        P3.setCoord(vec3.fromValues(-width, +height, 0));
        P3.setTexCoord(vec2.fromValues(0,1));

        // quadrilatère
        this.m_Mesh.addQuad(P0, P1, P2, P3);

        // créer le matériau du rectangle
        this.m_Texture = new Texture2D(nomimage);
        this.m_Material = new TextureMaterial(this.m_Texture);

        // créer les VBO pour afficher ce maillage
        let renderer = new MeshModuleDrawing(this.m_Mesh);
        this.m_VBOset = renderer.createStripVBOset(this.m_Material, true);
    }


    /**
     * dessine l'objet sur l'écran
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    onDraw(mat4Projection, mat4ModelView)
    {
        this.m_VBOset.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Material.destroy();
        this.m_Texture.destroy();
        this.m_VBOset.destroy();
        this.m_Mesh.destroy();
    }
}
