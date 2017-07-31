// Définition de la classe SquaredPyramid

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleDrawing.js");


class SquaredPyramid
{
    /**
     * Constructeur
     * @param material : matériau à appliquer
     */
    constructor(material)
    {
        // créer le maillage
        this.m_Mesh = new Mesh("SquaredPyramid");

        // créer les 5 sommets
        let s1 = this.m_Mesh.addVertex("s1");
        s1.setCoord(vec3.fromValues(-1,-1,-1));
        s1.setColor( vec3.fromValues(1.0, 0.0, 0.0));

        let s2 = this.m_Mesh.addVertex("s2");
        s2.setCoord(vec3.fromValues(+1,-1,-1));
        s2.setColor( vec3.fromValues(0.8, 1.0, 0.0));

        let s3 = this.m_Mesh.addVertex("s3");
        s3.setCoord(vec3.fromValues(+1,-1,+1));
        s3.setColor( vec3.fromValues(0.0, 1.0, 0.4));

        let s4 = this.m_Mesh.addVertex("s4");
        s4.setCoord(vec3.fromValues(-1,-1,+1));
        s4.setColor( vec3.fromValues(0.0, 0.4, 1.0));

        let s5 = this.m_Mesh.addVertex("s5");
        s5.setCoord(vec3.fromValues( 0,+1, 0));
        s5.setColor( vec3.fromValues(0.8, 0.0, 1.0));

        // créer le fond
        this.m_Mesh.addTriangle(s1,s2,s3);
        this.m_Mesh.addTriangle(s1,s3,s4);

        // créer la face devant
        this.m_Mesh.addTriangle(s4,s3,s5);

        // créer la face droite
        this.m_Mesh.addTriangle(s3,s2,s5);

        // créer la face arrière
        this.m_Mesh.addTriangle(s2,s1,s5);

        // créer la face gauche
        this.m_Mesh.addTriangle(s1,s4,s5);

        // créer le VBO
        let renderer = new MeshModuleDrawing(this.m_Mesh);
        this.m_VBOset = renderer.createVBOset(material);
    }


    /**
     * dessine la pyramide
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
