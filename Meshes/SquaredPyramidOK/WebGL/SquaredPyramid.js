// Définition de la classe SquaredPyramid

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleUtils.js");
Requires("libs/Mesh/MeshModulePhysics.js");


class SquaredPyramid
{
    /**
     * Constructeur
     */
    constructor()
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
        this.m_Mesh.addTriangle(s4.clone("a"),s3.clone("a"),s5);

        // créer la face droite
        this.m_Mesh.addTriangle(s3.clone("b"),s2.clone("b"),s5.clone("b"));

        // créer la face arrière
        this.m_Mesh.addTriangle(s2.clone("c"),s1.clone("c"),s5.clone("c"));

        // créer la face gauche
        this.m_Mesh.addTriangle(s1.clone("d"),s4.clone("d"),s5.clone("d"));

        // calculer les normales
        this.m_Mesh.computeNormals();

        // vérifier le maillage
        let utils = new MeshModuleUtils(this.m_Mesh);
        utils.checkMesh(true);

        // calculer le volume et le centre de gravité
        let physique = new MeshModulePhysics(this.m_Mesh);
        physique.setDensity(1.0);
        physique.compVolumeIntegrals();
    }


    /**
     * fournit le maillage de cet objet
     * @return maillage
     */
    getMesh()
    {
        return this.m_Mesh;
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Mesh.destroy();
    }
}
