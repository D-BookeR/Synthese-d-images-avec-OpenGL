#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>
#include <sstream>
#include <math.h>

#include <utils.h>
#include <MeshModuleProcessing.h>

#include <Test.h>


/**
 * Constructeur
 */
Test::Test()
{
    vec3 color  = vec3::fromValues(1.0,0.25,0.15);

    // créer l'objet maillage
    m_Mesh = new Mesh("Test");

    /// Topologie : éléments et connexions

    // rajouter des sommets
    MeshVertex* A = m_Mesh->addVertex("A"); A->setColor(vec3::fromValues(1.0,0.25,0.15));
    MeshVertex* B = m_Mesh->addVertex("B"); B->setColor(vec3::fromValues(0.25,0.25,1.0));
    MeshVertex* C = m_Mesh->addVertex("C"); C->setColor(vec3::fromValues(0.15,1.0,0.25));
    MeshVertex* D = m_Mesh->addVertex("D"); D->setColor(vec3::fromValues(1.0,0.75,0.25));

    //mkCarre(A, B, C, D);
    mkTetra(A, B, C, D);

    // calculer les normales
    m_Mesh->computeNormals();
}


void Test::mkSquare(MeshVertex* A, MeshVertex* B, MeshVertex* C, MeshVertex* D)
{
    /// Topologie : éléments et connexions

    // rajouter des triangles pour former un carré
    m_Mesh->addTriangle(A, B, C);
    m_Mesh->addTriangle(A, C, D);

    /// Géométrie : position des sommets et autres infos

    // affectation des couleurs et des coordonnées des sommets
    A->setCoord(vec3::fromValues(-2.0, 0.0, 2.0));
    B->setCoord(vec3::fromValues( 2.0, 0.0, 2.0));
    C->setCoord(vec3::fromValues( 2.0, 0.0,-2.0));
    D->setCoord(vec3::fromValues(-2.0, 0.0,-2.0));
}


void Test::mkTetra(MeshVertex* A, MeshVertex* B, MeshVertex* C, MeshVertex* D)
{
    /// Topologie : éléments et connexions

    // rajouter des triangles pour former un tétraèdre
    MeshTriangle* abc = m_Mesh->addTriangle(A,B,C);
    m_Mesh->addTriangle(B,A,D);
    m_Mesh->addTriangle(B,D,C);
    m_Mesh->addTriangle(A,C,D);

    /// Géométrie : position des sommets et autres infos

    // affectation des couleurs et des coordonnées des sommets
    A->setCoord(vec3::fromValues( 1, 1, 1));
    B->setCoord(vec3::fromValues(-1,-1, 1));
    C->setCoord(vec3::fromValues( 1,-1,-1));
    D->setCoord(vec3::fromValues(-1, 1,-1));
}


/**
 * Cette méthode supprime les ressources allouées
 */
Test::~Test()
{
    delete m_Mesh;
}
