// Définition de la classe Test

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");


/**
 * Définit la classe Test
 */
class Test
{
    /**
     * Constructeur
     */
    constructor()
    {
        // créer l'objet maillage
        this.m_Mesh = new Mesh("Test");

        /// Topologie : éléments et connexions

        // rajouter des sommets
        let A = this.m_Mesh.addVertex("A"); A.setColor(vec3.fromValues(1.0,0.25,0.15));
        let B = this.m_Mesh.addVertex("B"); B.setColor(vec3.fromValues(0.25,0.25,1.0));
        let C = this.m_Mesh.addVertex("C"); C.setColor(vec3.fromValues(0.15,1.0,0.25));
        let D = this.m_Mesh.addVertex("D"); D.setColor(vec3.fromValues(1.0,0.75,0.25));

        //this.mkCarre(A, B, C, D);
        this.mkTetra(A, B, C, D);

        // calculer les normales
        this.m_Mesh.computeNormals();
    }


    /**
     * fournit le maillage de cet objet
     * @return maillage
     */
    getMesh()
    {
        return this.m_Mesh;
    }


    mkSquare(A, B, C, D)
    {
        /// Topologie : éléments et connexions

        // rajouter des triangles pour former un carré
        this.m_Mesh.addTriangle(A, B, C);
        this.m_Mesh.addTriangle(A, C, D);

        /// Géométrie : position des sommets et autres infos

        // affectation des couleurs et des coordonnées des sommets
        A.setCoord(vec3.fromValues(-2.0, 0.0, 2.0));
        B.setCoord(vec3.fromValues( 2.0, 0.0, 2.0));
        C.setCoord(vec3.fromValues( 2.0, 0.0,-2.0));
        D.setCoord(vec3.fromValues(-2.0, 0.0,-2.0));
    }


    mkTetra(A, B, C, D)
    {
        /// Topologie : éléments et connexions

        // rajouter des triangles pour former un tétraèdre
        this.m_Mesh.addTriangle(A,B,C);
        this.m_Mesh.addTriangle(B,A,D);
        this.m_Mesh.addTriangle(B,D,C);
        this.m_Mesh.addTriangle(A,C,D);

        /// Géométrie : position des sommets et autres infos

        // affectation des couleurs et des coordonnées des sommets
        A.setCoord(vec3.fromValues( 1, 1, 1));
        B.setCoord(vec3.fromValues(-1,-1, 1));
        C.setCoord(vec3.fromValues( 1,-1,-1));
        D.setCoord(vec3.fromValues(-1, 1,-1));
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Mesh.destroy();
    }
}
