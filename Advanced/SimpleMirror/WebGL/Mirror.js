// Définition de la classe Mirror

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleDrawing.js");
Requires("libs/Material/ColorMaterial.js");


/**
 * Classe Mirror = surface réfléchissante
 */
class Mirror
{
    /**
     * Constructeur
     */
    constructor()
    {
        // créer le maillage du miroir
        this.m_Mesh = new Mesh("miroir");

        // sommets
        let P0m = this.m_Mesh.addVertex("P0");
        P0m.setCoord(vec3.fromValues(-2.0, 0, 0));
        let P1m = this.m_Mesh.addVertex("P1");
        P1m.setCoord(vec3.fromValues(+2.0, 0, 0));
        let P2m = this.m_Mesh.addVertex("P2");
        P2m.setCoord(vec3.fromValues(+2.0, +2.5, 0));
        let P3m = this.m_Mesh.addVertex("P3");
        P3m.setCoord(vec3.fromValues(-2.0, +2.5, 0));

        // quadrilatère
        this.m_Mesh.addQuad(P0m, P1m, P2m, P3m);

        // créer le matériau du rectangle
        this.m_Material = new ColorMaterial(vec4.fromValues(0.1, 0.1, 0.2, 1.0));

        // créer les VBO pour afficher ce maillage
        let renderer = new MeshModuleDrawing(this.m_Mesh);
        this.m_VBOset = renderer.createStripVBOset(this.m_Material, true);
    }


    /**
     * dessine la surface du miroir
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
        this.m_Mesh.destroy();
    }
}
