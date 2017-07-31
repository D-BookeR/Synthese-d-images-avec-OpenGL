// Définition de la classe Ground

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleDrawing.js");
Requires("libs/Material/TextureMaterial.js");


/**
 * Classe Ground
 */
class Ground
{
    /**
     * Constructeur
     * @param depth : profondeur à affecter à la coordonnée y des sommets
     */
    constructor(depth)
    {
        // créer le maillage du miroir
        this.m_Mesh = new Mesh("fond");

        // sommets
        let P0m = this.m_Mesh.addVertex("P0");
        P0m.setCoord(vec3.fromValues(-999, depth, +999));
        P0m.setTexCoord(vec2.fromValues(-999, +999));
        P0m.setNormal(vec3.fromValues(0.0, 1.0, 0.0));
        let P1m = this.m_Mesh.addVertex("P1");
        P1m.setCoord(vec3.fromValues(+999, depth, +999));
        P1m.setTexCoord(vec2.fromValues(+999, +999));
        P1m.setNormal(vec3.fromValues(0.0, 1.0, 0.0));
        let P2m = this.m_Mesh.addVertex("P2");
        P2m.setCoord(vec3.fromValues(+999, depth, -999));
        P2m.setTexCoord(vec2.fromValues(+999, -999));
        P2m.setNormal(vec3.fromValues(0.0, 1.0, 0.0));
        let P3m = this.m_Mesh.addVertex("P3");
        P3m.setCoord(vec3.fromValues(-999, depth, -999));
        P3m.setTexCoord(vec2.fromValues(-999, -999));
        P3m.setNormal(vec3.fromValues(0.0, 1.0, 0.0));

        // quadrilatère
        this.m_Mesh.addQuad(P0m, P1m, P2m, P3m);

        // créer le matériau du rectangle
        this.m_Texture = new Texture2D("data/textures/sols/79798.jpg", gl.LINEAR_MIPMAP_LINEAR, gl.REPEAT);
        this.m_Material = new TextureMaterial(this.m_Texture);

        // créer les VBO pour afficher ce maillage
        let renderer = new MeshModuleDrawing(this.m_Mesh);
        this.m_VBOset = renderer.createStripVBOset(this.m_Material, true);
    }


    /**
     * dessine le fond
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
        this.m_VBOset.destroy();
        this.m_Material.destroy();
        this.m_Texture.destroy();
        this.m_Mesh.destroy();
    }
}
