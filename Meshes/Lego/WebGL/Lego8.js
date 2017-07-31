// Définition de la classe Lego8

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleProcessing.js");


/**
 * Définit la classe Lego8
 */
class Lego8
{
    /**
     * Constructeur
     */
    constructor()
    {
        let color1 = vec3.fromValues(0.0,0.3,0.8);
        let color2 = vec3.fromValues(0.1,0.5,1.0);

        // créer l'objet maillage
        this.m_Mesh = new Mesh("Lego8");

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
        let I = this.m_Mesh.addVertex("I");
        let J = this.m_Mesh.addVertex("J");
        let K = this.m_Mesh.addVertex("K");
        let L = this.m_Mesh.addVertex("L");
        let M = this.m_Mesh.addVertex("M");
        let N = this.m_Mesh.addVertex("N");
        let O = this.m_Mesh.addVertex("O");
        let P = this.m_Mesh.addVertex("P");

        // rajouter des triangles et des quads
        this.m_Mesh.addTriangle(A, G, E);
        this.m_Mesh.addQuad(A, E, F, B);
        this.m_Mesh.addTriangle(B, F, H);
        this.m_Mesh.addQuad(B, H, J, D);
        this.m_Mesh.addTriangle(D, J, L);
        this.m_Mesh.addQuad(D, L, K, C);
        this.m_Mesh.addTriangle(C, K, I);
        this.m_Mesh.addQuad(C, I, G, A);

        //this.m_Mesh.addQuad(G, I, F, E);
        //this.m_Mesh.addQuad(I, K, H, F);
        //this.m_Mesh.addQuad(K, L, J, H);

        this.m_Mesh.addTriangle(G, M, E);
        this.m_Mesh.addQuad(E, M, N, F);
        this.m_Mesh.addTriangle(F, N, H);
        this.m_Mesh.addQuad(H, N, P, J);
        this.m_Mesh.addTriangle(J, P, L);
        this.m_Mesh.addQuad(L, P, O, K);
        this.m_Mesh.addTriangle(K, O, I);
        this.m_Mesh.addQuad(I, O, M, G);

        this.m_Mesh.addQuad(M, O, P, N);

        /// Géométrie : position des sommets et autres info

        // affectation des couleurs et des coordonnées des sommets
        A.setCoord(vec3.fromValues(-3.0, 0.0,-3.0)); A.setColor(color1);
        B.setCoord(vec3.fromValues( 3.0, 0.0,-3.0)); B.setColor(color1);
        C.setCoord(vec3.fromValues(-3.0, 0.0, 3.0)); C.setColor(color1);
        D.setCoord(vec3.fromValues( 3.0, 0.0, 3.0)); D.setColor(color1);

        E.setCoord(vec3.fromValues(-1.0, 0.0,-2.0)); E.setColor(color1);
        F.setCoord(vec3.fromValues( 1.0, 0.0,-2.0)); F.setColor(color1);

        G.setCoord(vec3.fromValues(-2.0, 0.0,-1.0)); G.setColor(color1);
        H.setCoord(vec3.fromValues( 2.0, 0.0,-1.0)); H.setColor(color1);
        I.setCoord(vec3.fromValues(-2.0, 0.0, 1.0)); I.setColor(color1);
        J.setCoord(vec3.fromValues( 2.0, 0.0, 1.0)); J.setColor(color1);

        K.setCoord(vec3.fromValues(-1.0, 0.0, 2.0)); K.setColor(color1);
        L.setCoord(vec3.fromValues( 1.0, 0.0, 2.0)); L.setColor(color1);

        M.setCoord(vec3.fromValues(-1.0, 0.0,-1.0)); M.setColor(color2);
        N.setCoord(vec3.fromValues( 1.0, 0.0,-1.0)); N.setColor(color2);
        O.setCoord(vec3.fromValues(-1.0, 0.0, 1.0)); O.setColor(color2);
        P.setCoord(vec3.fromValues( 1.0, 0.0, 1.0)); P.setColor(color2);


        // calculer les normales
        this.m_Mesh.computeNormals();

        // module de transformation pour ce maillage
        let transformation = new MeshModuleProcessing(this.m_Mesh);

        // faire une extrusion de la bosse
        let edge1 = [G, I, K, L, J, H, F, E];
        let clones = transformation.extrudePolygon(edge1, 2.5);

        transformation.splitBorder(edge1);
        transformation.splitBorder(clones);

        // sous-extrusion du centre de la bosse
        let edge2 = [M, O, P, N];
        transformation.extrudePolygon(edge2, -1.0);

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
