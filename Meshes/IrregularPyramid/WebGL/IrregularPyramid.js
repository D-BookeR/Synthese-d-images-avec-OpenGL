// Définition de la classe IrregularPyramid

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");


/**
 * constructeur
 */
class IrregularPyramid
{
    /**
     * Constructeur
     */
    constructor()
    {
        // créer le maillage
        this.m_Mesh = new Mesh("IrregularPyramid");

        /// Topologie : éléments et connexions

        // rajouter des sommets
        let s1 = this.m_Mesh.addVertex("s1");
        let s2 = this.m_Mesh.addVertex("s2");
        let s3 = this.m_Mesh.addVertex("s3");
        let s4 = this.m_Mesh.addVertex("s4");
        let s5 = this.m_Mesh.addVertex("s5");
        let s6 = this.m_Mesh.addVertex("s6");

        // rajouter des triangles autour de s1
        this.m_Mesh.addTriangle(s1, s2, s3);
        this.m_Mesh.addTriangle(s1, s3, s4);
        this.m_Mesh.addTriangle(s1, s4, s5);
        //this.m_Mesh.addTriangle(s1, s5, s4);       // => normale inversée pour tester
        this.m_Mesh.addTriangle(s1, s5, s6);
        this.m_Mesh.addTriangle(s1, s6, s2);

        /// Géométrie : position des sommets et autres info

        // affectation des couleurs et des coordonnées des sommets
        s1.setCoord(vec3.fromValues( 0.0,+1.0, 0.0));
        s1.setColor(vec3.fromValues(0.9,0.0,0.0));   // rouge

        s2.setCoord(vec3.fromValues(-1.5, 0.0, 0.4));
        s2.setColor(vec3.fromValues(0.0,0.8,0.0));   // vert

        s3.setCoord(vec3.fromValues(+1.0, 0.0,+1.0));
        s3.setColor(vec3.fromValues(0.8,1.0,0.0));   // jaune-vert

        s4.setCoord(vec3.fromValues(+1.0, 0.0, 0.8));
        s4.setColor(vec3.fromValues(0.0,0.8,1.0));   // cyan

        s5.setCoord(vec3.fromValues(+0.1, 0.0,-1.4));
        s5.setColor(vec3.fromValues(1.0,0.2,0.5));   // rose

        s6.setCoord(vec3.fromValues(-0.1, 0.0,-1.5));
        s6.setColor(vec3.fromValues(1.0,0.7,0.1));   // orange


        // mettre à jour la géométrie : normales...
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


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Mesh.destroy();
    }
}
