// Définition de la classe Lego4

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleProcessing.js");


/**
 * Définit la classe Lego4
 */
class Lego4
{
    /**
     * Constructeur
     */
    constructor()
    {
        let color  = vec3.fromValues(1.0,0.25,0.15);

        // créer l'objet maillage
        this.m_Mesh = new Mesh("Lego4");

        /// Topologie : éléments et connexions

        // rajouter des sommets
        let A = this.m_Mesh.addVertex("A");
        let B = this.m_Mesh.addVertex("B");
        let C = this.m_Mesh.addVertex("C");
        let D = this.m_Mesh.addVertex("D");
        let E = this.m_Mesh.addVertex("E");
        let F = this.m_Mesh.addVertex("F");
        let G = this.m_Mesh.addVertex("G");
        let H = this.m_Mesh.addVertex("H");

        // rajouter des triangles et des quads
        this.m_Mesh.addTriangle(A, E, B);
        this.m_Mesh.addTriangle(B, E, G);
        this.m_Mesh.addTriangle(B, G, D);
        this.m_Mesh.addTriangle(D, G, H);
        this.m_Mesh.addTriangle(D, H, C);
        this.m_Mesh.addTriangle(C, H, F);
        this.m_Mesh.addTriangle(C, F, A);
        this.m_Mesh.addTriangle(A, F, E);
        this.m_Mesh.addQuad(E, F, H, G);


        /// Géométrie : position des sommets et autres info

        // affectation des couleurs et des coordonnées des sommets
        A.setCoord(vec3.fromValues(-2.0, 0.0,-2.0)); A.setColor(color);
        B.setCoord(vec3.fromValues( 2.0, 0.0,-2.0)); B.setColor(color);
        C.setCoord(vec3.fromValues(-2.0, 0.0, 2.0)); C.setColor(color);
        D.setCoord(vec3.fromValues( 2.0, 0.0, 2.0)); D.setColor(color);

        E.setCoord(vec3.fromValues( 0.0, 0.0,-1.0)); E.setColor(color);
        F.setCoord(vec3.fromValues(-1.0, 0.0, 0.0)); F.setColor(color);
        G.setCoord(vec3.fromValues( 1.0, 0.0, 0.0)); G.setColor(color);
        H.setCoord(vec3.fromValues( 0.0, 0.0, 1.0)); H.setColor(color);

        // calculer les normales
        this.m_Mesh.computeNormals();

        // module de transformation pour ce maillage
        let transformation = new MeshModuleProcessing(this.m_Mesh);

        // faire une extrusion de la bosse
        let edge = [E, F, H, G];
        transformation.extrudePolygon(edge, 3.0);

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


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Mesh.destroy();
    }
}
