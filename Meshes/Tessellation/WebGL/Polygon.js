// Définition de la classe Polygon

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");


/**
 * Définit la classe Polygon
 */
class Polygon
{
    /**
     * Constructeur
     */
    constructor()
    {
        // créer l'objet maillage
        this.m_Mesh = new Mesh("Polygon");

        // rajouter des sommets
        let A = this.m_Mesh.addVertex("A");
        A.setColor(vec3.fromValues(0.5, 0.8, 0.2));
        A.setCoord(vec3.fromValues(0.5, -2.0, 0.0));

        let B = this.m_Mesh.addVertex("B");
        B.setColor(vec3.fromValues(0.5, 0.8, 0.2));
        B.setCoord(vec3.fromValues(2.0, -1.0, 0.0));

        let C = this.m_Mesh.addVertex("C");
        C.setColor(vec3.fromValues(0.5, 0.8, 0.2));
        C.setCoord(vec3.fromValues(1.0, 0.0, 0.0));

        let D = this.m_Mesh.addVertex("D");
        D.setColor(vec3.fromValues(0.5, 0.8, 0.2));
        D.setCoord(vec3.fromValues(2.0, 1.5, 0.0));

        let E = this.m_Mesh.addVertex("E");
        E.setColor(vec3.fromValues(0.5, 0.8, 0.2));
        E.setCoord(vec3.fromValues(0.0, 0.5, 0.0));

        let F = this.m_Mesh.addVertex("F");
        F.setColor(vec3.fromValues(0.5, 0.8, 0.2));
        F.setCoord(vec3.fromValues(0.5, 2.0, 0.0));

        let G = this.m_Mesh.addVertex("G");
        G.setColor(vec3.fromValues(0.5, 0.8, 0.2));
        G.setCoord(vec3.fromValues(-2.0, 1.0, 0.0));

        let H = this.m_Mesh.addVertex("H");
        H.setColor(vec3.fromValues(0.5, 0.8, 0.2));
        H.setCoord(vec3.fromValues(-0.5, 0.0, 0.0));

        let I = this.m_Mesh.addVertex("I");
        I.setColor(vec3.fromValues(0.5, 0.8, 0.2));
        I.setCoord(vec3.fromValues(-2.0, -1.0, 0.0));

        // ajouter un polygone
        //this.m_Mesh.addPolygonConvex([A,B,C,D,E,F,G,H,I]);
        this.m_Mesh.addPolygon([A,B,C,D,E,F,G,H,I], vec3.fromValues(0,0,1));
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
