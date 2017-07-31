// Définition de la classe Shape

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleDrawing.js");
Requires("ColorMaterial");


class Shape
{
    /**
     * Constructeur
     */
    constructor()
    {
        // créer le maillage
        this.m_Mesh = new Mesh("Shape");

        // sommet 0
        let s0 = this.m_Mesh.addVertex("s0");
        s0.setCoord(vec3.fromValues(-1, 0, +1));
        s0.setColor(vec3.fromValues(0.86,0.13,0.99));

        // sommet 1
        let s1 = this.m_Mesh.addVertex("s1");
        s1.setCoord(vec3.fromValues( 0, 0, +1));
        s1.setColor(vec3.fromValues(0.76,0.32,0.50));

        // sommet 2
        let s2 = this.m_Mesh.addVertex("s2");
        s2.setCoord(vec3.fromValues(+1, 0, +1));
        s2.setColor(vec3.fromValues(0.12,0.99,0.87));

        // sommet 3
        let s3 = this.m_Mesh.addVertex("s3");
        s3.setCoord(vec3.fromValues(-1, 0,  0));
        s3.setColor(vec3.fromValues(0.79,0.30,0.21));

        // sommet 4
        let s4 = this.m_Mesh.addVertex("s4");
        s4.setCoord(vec3.fromValues( 0, 0,  0));
        s4.setColor(vec3.fromValues(0.86,0.49,0.17));

        // sommet 5
        let s5 = this.m_Mesh.addVertex("s5");
        s5.setCoord(vec3.fromValues(+1, 0,  0));
        s5.setColor(vec3.fromValues(0.12,0.80,0.19));

        // sommet 6
        let s6 = this.m_Mesh.addVertex("s6");
        s6.setCoord(vec3.fromValues(-1, 0, -1));
        s6.setColor(vec3.fromValues(0.28,0.70,0.86));

        // sommet 7
        let s7 = this.m_Mesh.addVertex("s7");
        s7.setCoord(vec3.fromValues( 0, 0, -1));
        s7.setColor(vec3.fromValues(0.00,0.29,0.22));

        // sommet 8
        let s8 = this.m_Mesh.addVertex("s8");
        s8.setCoord(vec3.fromValues(+1, 0, -1));
        s8.setColor(vec3.fromValues(0.42,0.26,0.83));

        // ajouter les quads
        this.m_Mesh.addQuad(s0, s1, s4, s3);
        this.m_Mesh.addQuad(s1, s2, s5, s4);
        this.m_Mesh.addQuad(s3, s4, s7, s6);
        this.m_Mesh.addQuad(s4, s5, s8, s7);

        // créer le matériau du rectangle
        this.m_Material = new ColorMaterial();

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
