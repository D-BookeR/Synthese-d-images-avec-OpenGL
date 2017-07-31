#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>
#include <sstream>
#include <math.h>

#include <utils.h>
#include <MeshModuleProcessing.h>

#include <Polygon.h>


/**
 * Constructeur
 */
Polygon::Polygon()
{
    // créer l'objet maillage
    m_Mesh = new Mesh("Polygon");

    // rajouter des sommets
    MeshVertex* A = m_Mesh->addVertex("A");
    A->setColor(vec3::fromValues(0.5, 0.8, 0.2));
    A->setCoord(vec3::fromValues(0.5, -2.0, 0.0));

    MeshVertex* B = m_Mesh->addVertex("B");
    B->setColor(vec3::fromValues(0.5, 0.8, 0.2));
    B->setCoord(vec3::fromValues(2.0, -1.0, 0.0));

    MeshVertex* C = m_Mesh->addVertex("C");
    C->setColor(vec3::fromValues(0.5, 0.8, 0.2));
    C->setCoord(vec3::fromValues(1.0, 0.0, 0.0));

    MeshVertex* D = m_Mesh->addVertex("D");
    D->setColor(vec3::fromValues(0.5, 0.8, 0.2));
    D->setCoord(vec3::fromValues(2.0, 1.5, 0.0));

    MeshVertex* E = m_Mesh->addVertex("E");
    E->setColor(vec3::fromValues(0.5, 0.8, 0.2));
    E->setCoord(vec3::fromValues(0.0, 0.5, 0.0));

    MeshVertex* F = m_Mesh->addVertex("F");
    F->setColor(vec3::fromValues(0.5, 0.8, 0.2));
    F->setCoord(vec3::fromValues(0.5, 2.0, 0.0));

    MeshVertex* G = m_Mesh->addVertex("G");
    G->setColor(vec3::fromValues(0.5, 0.8, 0.2));
    G->setCoord(vec3::fromValues(-2.0, 1.0, 0.0));

    MeshVertex* H = m_Mesh->addVertex("H");
    H->setColor(vec3::fromValues(0.5, 0.8, 0.2));
    H->setCoord(vec3::fromValues(-0.5, 0.0, 0.0));

    MeshVertex* I = m_Mesh->addVertex("I");
    I->setColor(vec3::fromValues(0.5, 0.8, 0.2));
    I->setCoord(vec3::fromValues(-2.0, -1.0, 0.0));

    // ajouter un polygone
    //m_Mesh->addPolygonConvex(std::vector<MeshVertex*> {A,B,C,D,E,F,G,H,I});
    m_Mesh->addPolygon(std::vector<MeshVertex*> {A,B,C,D,E,F,G,H,I}, vec3::fromValues(0,0,1));
}


/**
 * Cette méthode supprime les ressources allouées
 */
Polygon::~Polygon()
{
    delete m_Mesh;
}
