// Définition de la classe Rectangle

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleDrawing.js");
Requires("DisplacementMaterial");


class Rectangle
{
    /**
     * Constructeur
     */
    constructor()
    {
        // créer le maillage
        this.m_Mesh = new Mesh("Rectangle");

        // sommets
        let P0 = this.m_Mesh.addVertex("P0");
        P0.setCoord(vec3.fromValues(-1.33, -1, 0));
        P0.setTexCoord(vec2.fromValues(0,0));
        let P1 = this.m_Mesh.addVertex("P1");
        P1.setCoord(vec3.fromValues(+1.33, -1, 0));
        P1.setTexCoord(vec2.fromValues(1.33,0));
        let P2 = this.m_Mesh.addVertex("P2");
        P2.setCoord(vec3.fromValues(+1.33, +1, 0));
        P2.setTexCoord(vec2.fromValues(1.33,1));
        let P3 = this.m_Mesh.addVertex("P3");
        P3.setCoord(vec3.fromValues(-1.33, +1, 0));
        P3.setTexCoord(vec2.fromValues(0,1));

        // quadrilatère
        this.m_Mesh.addQuad(P0, P1, P2, P3);

        // calculer les normales et tangentes
        this.m_Mesh.computeNormals();
        this.m_Mesh.computeTangents();

        // créer le matériau du rectangle
        this.m_Material = new DisplacementMaterial();

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
        this.m_VBOset.destroy();
        this.m_Mesh.destroy();
    }
}
