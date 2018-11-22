// Définition de la classe SphereTess

#include <GL/glew.h>
#include <GL/gl.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <math.h>

#include <utils.h>
#include <MeshModuleProcessing.h>

#include <SphereTess.h>

// prototype
static Mesh* createPlatonicSolid(int platonic_type);


/**
 * Cette fonction définit la classe SphereTess.
 * NB: on ne peut pas définir de bonnes coordonnées de texture, il y a un repliement à la longitude 180°
 * @param platonic_type : forme de base 0=tétraèdre, 1=octaèdre, 2=icosaèdre
 * @param iterations : niveau de subdivision, 0=forme de base
 */
SphereTess::SphereTess(int platonic_type, int iterations)
{
    // construire le maillage initial
    m_Mesh = createPlatonicSolid(platonic_type);

    // module de transformation
    MeshModuleProcessing transformation(m_Mesh);

    // subdiviser le maillage au niveau demandé
    transformation.subdivideAll(m_Mesh->getTriangleList(), iterations, 0.0);

    // repasser sur tous les points pour les amener à une distance de 1 du centre
    for (MeshVertex* vertex: m_Mesh->getVertexList()) {
        vec3& coords = vertex->getCoord();      // coords est directement une référence sur le vec3 du sommet
        vec3::normalize(coords, coords);        // coords = les coordonnées du sommet et non une copie
        // normale = coordonnées car centrée et rayon 1
        vertex->setNormal(coords);
    }
}


/**
 * Cette méthode supprime les ressources allouées
 */
SphereTess::~SphereTess()
{
    delete m_Mesh;
}


/**
 * Cette fonction retourne un maillage contenant l'une des formes de Platon
 * @param platonic_type : 0=tétraèdre, 1=octaèdre, 2=icosaèdre
 * @return Mesh
 */
static Mesh* createPlatonicSolid(int platonic_type)
{
    // maillage retourné en résultat
    Mesh* mesh = new Mesh("SphereTess");

    // constantes
    const float a = 0.525731112119133606;
    const float b = 0.850650808352039932;
    const float c = 0.577350269;

    // coordonnées des sommets
    switch (platonic_type) {
        case 0: {
            // tétraèdre
            MeshVertex* A = mesh->addVertex("A"); A->setCoord(vec3::fromValues( c, c, c));
            MeshVertex* B = mesh->addVertex("B"); B->setCoord(vec3::fromValues(-c,-c, c));
            MeshVertex* C = mesh->addVertex("C"); C->setCoord(vec3::fromValues( c,-c,-c));
            MeshVertex* D = mesh->addVertex("D"); D->setCoord(vec3::fromValues(-c, c,-c));

            // ajout des triangles de cette forme
            mesh->addTriangle(A,B,C);
            mesh->addTriangle(B,A,D);
            mesh->addTriangle(B,D,C);
            mesh->addTriangle(A,C,D);

            } break;

        case 1: {
            // octaèdre
            MeshVertex* A = mesh->addVertex("A"); A->setCoord(vec3::fromValues( 0, 0,-1));
            MeshVertex* B = mesh->addVertex("B"); B->setCoord(vec3::fromValues( 1, 0, 0));
            MeshVertex* C = mesh->addVertex("C"); C->setCoord(vec3::fromValues( 0,-1, 0));
            MeshVertex* D = mesh->addVertex("D"); D->setCoord(vec3::fromValues(-1, 0, 0));
            MeshVertex* E = mesh->addVertex("E"); E->setCoord(vec3::fromValues( 0, 1, 0));
            MeshVertex* F = mesh->addVertex("F"); F->setCoord(vec3::fromValues( 0, 0, 1));

            // ajout des triangles de cette forme
            mesh->addTriangle(A,B,C);
            mesh->addTriangle(A,C,D);
            mesh->addTriangle(A,D,E);
            mesh->addTriangle(A,E,B);
            mesh->addTriangle(F,C,B);
            mesh->addTriangle(F,D,C);
            mesh->addTriangle(F,E,D);
            mesh->addTriangle(F,B,E);

            } break;

        case 2: {
            // icosaèdre
            MeshVertex* A = mesh->addVertex("A"); A->setCoord(vec3::fromValues( a, 0,-b));
            MeshVertex* B = mesh->addVertex("B"); B->setCoord(vec3::fromValues(-a, 0,-b));
            MeshVertex* C = mesh->addVertex("C"); C->setCoord(vec3::fromValues( a, 0, b));
            MeshVertex* D = mesh->addVertex("D"); D->setCoord(vec3::fromValues(-a, 0, b));
            MeshVertex* E = mesh->addVertex("E"); E->setCoord(vec3::fromValues( 0,-b,-a));
            MeshVertex* F = mesh->addVertex("F"); F->setCoord(vec3::fromValues( 0,-b, a));
            MeshVertex* G = mesh->addVertex("G"); G->setCoord(vec3::fromValues( 0, b,-a));
            MeshVertex* H = mesh->addVertex("H"); H->setCoord(vec3::fromValues( 0, b, a));
            MeshVertex* I = mesh->addVertex("I"); I->setCoord(vec3::fromValues(-b,-a, 0));
            MeshVertex* J = mesh->addVertex("J"); J->setCoord(vec3::fromValues( b,-a, 0));
            MeshVertex* K = mesh->addVertex("K"); K->setCoord(vec3::fromValues(-b, a, 0));
            MeshVertex* L = mesh->addVertex("L"); L->setCoord(vec3::fromValues( b, a, 0));

            // ajout des triangles de cette forme
            mesh->addTriangle(A,E,B);
            mesh->addTriangle(A,J,E);
            mesh->addTriangle(J,F,E);
            mesh->addTriangle(E,F,I);
            mesh->addTriangle(E,I,B);
            mesh->addTriangle(I,K,B);
            mesh->addTriangle(I,D,K);
            mesh->addTriangle(F,D,I);
            mesh->addTriangle(F,C,D);
            mesh->addTriangle(C,H,D);
            mesh->addTriangle(H,K,D);
            mesh->addTriangle(H,G,K);
            mesh->addTriangle(H,L,G);
            mesh->addTriangle(L,A,G);
            mesh->addTriangle(A,B,G);
            mesh->addTriangle(G,B,K);
            mesh->addTriangle(J,A,L);
            mesh->addTriangle(J,L,C);
            mesh->addTriangle(J,C,F);
            mesh->addTriangle(H,C,L);

            } break;
    }
    return mesh;
}
