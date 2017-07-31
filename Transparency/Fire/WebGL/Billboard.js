// Définition de la classe Billboard

Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleDrawing.js");
Requires("libs/Material/Material.js");


class Billboard
{
    /**
     * crée un billboard : un rectangle portant une texture transparente
     * @param material : matériau à appliquer
     * @param width : largeur du panneau
     * @param height : hauteur du panneau
     */
    constructor(material, width=1.0, height=1.0)
    {
        // matériau
        this.m_Material = material;

        // matrices pour stocker les ModelView
        this.m_ModelViewFixed = mat4.create();

        // créer le maillage
        this.m_Mesh = new Mesh("Billboard");

        // sommets
        let P0 = this.m_Mesh.addVertex("P0");
        P0.setCoord(vec3.fromValues(-width*0.5, 0.0, 0.0));
        P0.setTexCoord(vec2.fromValues(0.01, 0.01));

        let P1 = this.m_Mesh.addVertex("P1");
        P1.setCoord(vec3.fromValues(+width*0.5, 0.0, 0.0));
        P1.setTexCoord(vec2.fromValues(0.99, 0.01));

        let P2 = this.m_Mesh.addVertex("P2");
        P2.setCoord(vec3.fromValues(+width*0.5, height, 0.0));
        P2.setTexCoord(vec2.fromValues(0.99, 0.99));

        let P3 = this.m_Mesh.addVertex("P3");
        P3.setCoord(vec3.fromValues(-width*0.5, height, 0.0));
        P3.setTexCoord(vec2.fromValues(0.01, 0.99));

        // quadrilatère
        this.m_Mesh.addQuad(P0, P1, P2, P3);

        // créer les VBO pour afficher ce maillage
        let renderer = new MeshModuleDrawing(this.m_Mesh);
        this.m_VBOset = renderer.createStripVBOset(this.m_Material, true);
    }


    /**
     * dessin du billboard sur l'écran, variante simple
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    onDraw(mat4Projection, mat4ModelView)
    {
        // modifier la matrice ModelView
        mat4.copy(this.m_ModelViewFixed, mat4ModelView);
        this.m_ModelViewFixed[ 0] = 1.0;
        this.m_ModelViewFixed[ 1] = 0.0;
        this.m_ModelViewFixed[ 2] = 0.0;
        this.m_ModelViewFixed[ 4] = 0.0;
        this.m_ModelViewFixed[ 5] = 1.0;
        this.m_ModelViewFixed[ 6] = 0.0;
        this.m_ModelViewFixed[ 8] = 0.0;
        this.m_ModelViewFixed[ 9] = 0.0;
        this.m_ModelViewFixed[10] = 1.0;

        // mettre en place le blending
        gl.enable(gl.BLEND);

        // dessiner le rectangle face à la caméra
        this.m_VBOset.onDraw(mat4Projection, this.m_ModelViewFixed);

        // arrêter le blending
        gl.disable(gl.BLEND);
     }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_VBOset.destroy();
        this.m_Mesh.destroy();
        super.destroy();
    }
}
