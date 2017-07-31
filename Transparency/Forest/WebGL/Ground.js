// Définition de la classe Ground

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleDrawing.js");
Requires("libs/Material/TextureMaterial");


class Ground
{
    /**
     * Constructeur
     * @param taille du rectangle
     */
    constructor(taille=6.0)
    {
        // créer le maillage
        this.m_Mesh = new Mesh("Ground");

        // sommets
        let P0 = this.m_Mesh.addVertex("P0");
        P0.setCoord(vec3.fromValues(-taille, 0, -taille));
        P0.setTexCoord(vec2.fromValues(0,taille));
        let P1 = this.m_Mesh.addVertex("P1");
        P1.setCoord(vec3.fromValues(-taille, 0, +taille));
        P1.setTexCoord(vec2.fromValues(0,0));
        let P2 = this.m_Mesh.addVertex("P2");
        P2.setCoord(vec3.fromValues(+taille, 0, +taille));
        P2.setTexCoord(vec2.fromValues(taille,0));
        let P3 = this.m_Mesh.addVertex("P3");
        P3.setCoord(vec3.fromValues(+taille, 0, -taille));
        P3.setTexCoord(vec2.fromValues(taille,taille));

        // quadrilatère
        this.m_Mesh.addQuad(P0, P1, P2, P3);

        // créer le matériau du rectangle
        this.m_Texture = new Texture2D("data/textures/sols/ground_texture898.jpg", gl.LINEAR_MIPMAP_LINEAR, gl.REPEAT);
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
