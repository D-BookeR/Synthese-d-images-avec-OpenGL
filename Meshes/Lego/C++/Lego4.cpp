#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>
#include <math.h>

#include <utils.h>
#include <MeshModuleProcessing.h>

#include <Lego4.h>


/**
 * Constructeur
 */
Lego4::Lego4()
{
    vec3 color  = vec3::fromValues(1.0,0.25,0.15);

    // créer l'objet maillage
    m_Mesh = new Mesh("Lego4");

    /// Topologie : éléments et connexions

    // rajouter des sommets
    MeshVertex* A = m_Mesh->addVertex("A");
    MeshVertex* B = m_Mesh->addVertex("B");
    MeshVertex* C = m_Mesh->addVertex("C");
    MeshVertex* D = m_Mesh->addVertex("D");
    MeshVertex* E = m_Mesh->addVertex("E");
    MeshVertex* F = m_Mesh->addVertex("F");
    MeshVertex* G = m_Mesh->addVertex("G");
    MeshVertex* H = m_Mesh->addVertex("H");

    // rajouter des triangles et des quads
    m_Mesh->addTriangle(A, E, B);
    m_Mesh->addTriangle(B, E, G);
    m_Mesh->addTriangle(B, G, D);
    m_Mesh->addTriangle(D, G, H);
    m_Mesh->addTriangle(D, H, C);
    m_Mesh->addTriangle(C, H, F);
    m_Mesh->addTriangle(C, F, A);
    m_Mesh->addTriangle(A, F, E);
    m_Mesh->addQuad(E, F, H, G);


    /// Géométrie : position des sommets et autres info

    // affectation des couleurs et des coordonnées des sommets
    A->setCoord(vec3::fromValues(-2.0, 0.0,-2.0)); A->setColor(color);
    B->setCoord(vec3::fromValues( 2.0, 0.0,-2.0)); B->setColor(color);
    C->setCoord(vec3::fromValues(-2.0, 0.0, 2.0)); C->setColor(color);
    D->setCoord(vec3::fromValues( 2.0, 0.0, 2.0)); D->setColor(color);

    E->setCoord(vec3::fromValues( 0.0, 0.0,-1.0)); E->setColor(color);
    F->setCoord(vec3::fromValues(-1.0, 0.0, 0.0)); F->setColor(color);
    G->setCoord(vec3::fromValues( 1.0, 0.0, 0.0)); G->setColor(color);
    H->setCoord(vec3::fromValues( 0.0, 0.0, 1.0)); H->setColor(color);

    // calculer les normales
    m_Mesh->computeNormals();

    // module de transformation pour ce maillage
    MeshModuleProcessing transformation(m_Mesh);

    // faire une extrusion de la bosse
    std::vector<MeshVertex*> edge { E, F, H, G };
    transformation.extrudePolygon(edge, 3.0);

    // calculer les normales
    m_Mesh->computeNormals();
}


/**
 * Cette méthode supprime les ressources allouées
 */
Lego4::~Lego4()
{
    delete m_Mesh;
}
