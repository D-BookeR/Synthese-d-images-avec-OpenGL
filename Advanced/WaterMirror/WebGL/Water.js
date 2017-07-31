// Définition de la classe Water

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleDrawing.js");


/**
 * Classe Water
 */
class Water
{
    /**
     * Constructeur
     * @param material : matériau pour l'eau
     */
    constructor(material)
    {
        // créer le maillage du miroir
        this.m_Mesh = new Mesh("eau");

        // sommets
        let P0m = this.m_Mesh.addVertex("P0");
        P0m.setCoord(vec3.fromValues(-999, 0, +999));
        P0m.setTexCoord(vec2.fromValues(-999, +999));
        P0m.setNormal(vec3.fromValues(0.0, 1.0, 0.0));
        let P1m = this.m_Mesh.addVertex("P1");
        P1m.setCoord(vec3.fromValues(+999, 0, +999));
        P1m.setTexCoord(vec2.fromValues(+999, +999));
        P1m.setNormal(vec3.fromValues(0.0, 1.0, 0.0));
        let P2m = this.m_Mesh.addVertex("P2");
        P2m.setCoord(vec3.fromValues(+999, 0, -999));
        P2m.setTexCoord(vec2.fromValues(+999, -999));
        P2m.setNormal(vec3.fromValues(0.0, 1.0, 0.0));
        let P3m = this.m_Mesh.addVertex("P3");
        P3m.setCoord(vec3.fromValues(-999, 0, -999));
        P3m.setTexCoord(vec2.fromValues(-999, -999));
        P3m.setNormal(vec3.fromValues(0.0, 1.0, 0.0));

        // quadrilatère
        this.m_Mesh.addQuad(P0m, P1m, P2m, P3m);

        // créer les VBO pour afficher ce maillage
        let renderer = new MeshModuleDrawing(this.m_Mesh);
        this.m_VBOset = renderer.createStripVBOset(material, true);
    }


    /**
     * dessine la surface de l'eau
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
        this.m_Mesh.destroy();
    }
}
