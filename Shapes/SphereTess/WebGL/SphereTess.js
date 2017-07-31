// Définition de la classe SphereTess
// inspiration : https://github.com/vicentereig/tessellated-earth/blob/master/src/main/java/com/google/code/earth3d/TessellatedSphere.java
// inspiration : http://musingsofninjarat.wordpress.com/spheres-through-triangle-tessellation/

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleProcessing.js");


class SphereTess
{
    /**
     * Cette fonction définit la classe SphereTess.
     * NB: on ne peut pas définir de bonnes coordonnées de texture, il y a un repliement à la longitude 180°
     * @param platonic_type : forme de base 0=tétraèdre, 1=octaèdre, 2=icosaèdre
     * @param iterations : niveau de subdivision, 0=forme de base
     */
    constructor(platonic_type, iterations)
    {
        // construire le maillage initial
        this.m_Mesh = createPlatonicSolid(platonic_type);

        // module de transformation
        let transformation = new MeshModuleProcessing(this.m_Mesh);

        // subdiviser le maillage au niveau demandé
        transformation.subdivideAll(this.m_Mesh.getTriangleList(), iterations, 0.0);

        // repasser sur tous les points pour les amener à une distance de 1 du centre
        for (let vertex of this.m_Mesh.getVertexList()) {
            let coords = vertex.getCoord();   // coords est directement une référence sur le vec3 du sommet
            vec3.normalize(coords, coords);    // coords = les coordonnées du sommet et non une copie
            // normale = coordonnées car centrée et rayon 1
            vertex.setNormal(coords);
        }
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


/**
 * Cette fonction retourne un maillage contenant l'une des formes de Platon
 * @param platonic_type : 0=tétraèdre, 1=octaèdre, 2=icosaèdre
 * @return Mesh
 */
function createPlatonicSolid(platonic_type)
{
    // maillage retourné en résultat
    let mesh = new Mesh("SphereTess");

    // constantes
    const a = 0.525731112119133606;
    const b = 0.850650808352039932;
    const c = 0.577350269;

    // sommets
    let A,B,C,D,E,F,G,H,I,J,K,L;

    // coordonnées des sommets
    switch (platonic_type) {
        case 0:
            // tétraèdre
            A = mesh.addVertex("A"); A.setCoord(vec3.fromValues( c, c, c));
            B = mesh.addVertex("B"); B.setCoord(vec3.fromValues(-c,-c, c));
            C = mesh.addVertex("C"); C.setCoord(vec3.fromValues( c,-c,-c));
            D = mesh.addVertex("D"); D.setCoord(vec3.fromValues(-c, c,-c));

            // ajout des triangles de cette forme
            mesh.addTriangle(A,B,C);
            mesh.addTriangle(B,A,D);
            mesh.addTriangle(B,D,C);
            mesh.addTriangle(A,C,D);

            break;

        case 1:
            // octaèdre
            A = mesh.addVertex("A"); A.setCoord(vec3.fromValues( 0, 0,-1));
            B = mesh.addVertex("B"); B.setCoord(vec3.fromValues( 1, 0, 0));
            C = mesh.addVertex("C"); C.setCoord(vec3.fromValues( 0,-1, 0));
            D = mesh.addVertex("D"); D.setCoord(vec3.fromValues(-1, 0, 0));
            E = mesh.addVertex("E"); E.setCoord(vec3.fromValues( 0, 1, 0));
            F = mesh.addVertex("F"); F.setCoord(vec3.fromValues( 0, 0, 1));

            // ajout des triangles de cette forme
            mesh.addTriangle(A,B,C);
            mesh.addTriangle(A,C,D);
            mesh.addTriangle(A,D,E);
            mesh.addTriangle(A,E,B);
            mesh.addTriangle(F,C,B);
            mesh.addTriangle(F,D,C);
            mesh.addTriangle(F,E,D);
            mesh.addTriangle(F,B,E);

            break;

        case 2:
            // icosaèdre
            A = mesh.addVertex("A"); A.setCoord(vec3.fromValues( a, 0,-b));
            B = mesh.addVertex("B"); B.setCoord(vec3.fromValues(-a, 0,-b));
            C = mesh.addVertex("C"); C.setCoord(vec3.fromValues( a, 0, b));
            D = mesh.addVertex("D"); D.setCoord(vec3.fromValues(-a, 0, b));
            E = mesh.addVertex("E"); E.setCoord(vec3.fromValues( 0,-b,-a));
            F = mesh.addVertex("F"); F.setCoord(vec3.fromValues( 0,-b, a));
            G = mesh.addVertex("G"); G.setCoord(vec3.fromValues( 0, b,-a));
            H = mesh.addVertex("H"); H.setCoord(vec3.fromValues( 0, b, a));
            I = mesh.addVertex("I"); I.setCoord(vec3.fromValues(-b,-a, 0));
            J = mesh.addVertex("J"); J.setCoord(vec3.fromValues( b,-a, 0));
            K = mesh.addVertex("K"); K.setCoord(vec3.fromValues(-b, a, 0));
            L = mesh.addVertex("L"); L.setCoord(vec3.fromValues( b, a, 0));

            // ajout des triangles de cette forme
            mesh.addTriangle(A,E,B);
            mesh.addTriangle(A,J,E);
            mesh.addTriangle(J,F,E);
            mesh.addTriangle(E,F,I);
            mesh.addTriangle(E,I,B);
            mesh.addTriangle(I,K,B);
            mesh.addTriangle(I,D,K);
            mesh.addTriangle(F,D,I);
            mesh.addTriangle(F,C,D);
            mesh.addTriangle(C,H,D);
            mesh.addTriangle(H,K,D);
            mesh.addTriangle(H,G,K);
            mesh.addTriangle(H,L,G);
            mesh.addTriangle(L,A,G);
            mesh.addTriangle(A,B,G);
            mesh.addTriangle(G,B,K);
            mesh.addTriangle(J,A,L);
            mesh.addTriangle(J,L,C);
            mesh.addTriangle(J,C,F);
            mesh.addTriangle(H,C,L);

            break;
    }
    return mesh;
}
