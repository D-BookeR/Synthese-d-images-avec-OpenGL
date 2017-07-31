#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>
#include <math.h>

#include <utils.h>
#include <MeshModuleProcessing.h>

#include <Corner.h>


/**
 * Constructeur
 */
Corner::Corner()
{
    m_Mesh = new Mesh("Corner");

    /// Topologie : éléments et connexions

    // rajouter des sommets
    MeshVertex* s0 = m_Mesh->addVertex("s0");
    MeshVertex* s1 = m_Mesh->addVertex("s1");
    MeshVertex* s2 = m_Mesh->addVertex("s2");
    MeshVertex* s3 = m_Mesh->addVertex("s3");
    MeshVertex* s4 = m_Mesh->addVertex("s4");
    MeshVertex* s5 = m_Mesh->addVertex("s5");
    MeshVertex* s6 = m_Mesh->addVertex("s6");

    // rajouter des triangles
    m_Mesh->addTriangle(s0, s1, s2);
    m_Mesh->addTriangle(s0, s2, s3);
    m_Mesh->addTriangle(s0, s3, s4);
    m_Mesh->addTriangle(s0, s4, s5);
    m_Mesh->addTriangle(s0, s5, s6);
    m_Mesh->addTriangle(s0, s6, s1);

    /// Géométrie : position des sommets et autres info

    // affectation des couleurs et des coordonnées des sommets
    s0->setCoord(vec3::fromValues( 0, 3, 0)); s0->setColor(vec3::fromValues(1.0,0.6,0.0));   // orange
    s1->setCoord(vec3::fromValues( 3, 0, 0)); s1->setColor(vec3::fromValues(0.9,0.0,0.0));   // rouge
    s2->setCoord(vec3::fromValues( 2, 0,-2)); s2->setColor(vec3::fromValues(0.0,0.8,0.0));   // vert
    s3->setCoord(vec3::fromValues(-2,-1,-2)); s3->setColor(vec3::fromValues(0.1,0.6,0.8));   // cyan
    s4->setCoord(vec3::fromValues(-1, 0, 0)); s4->setColor(vec3::fromValues(1.0,1.0,0.2));   // jaune
    s5->setCoord(vec3::fromValues(-2,-2, 2)); s5->setColor(vec3::fromValues(1.0,0.4,0.7));   // rose
    s6->setCoord(vec3::fromValues( 1, 0, 3)); s6->setColor(vec3::fromValues(0.0,0.2,1.0));   // bleu

    // mettre à jour la géométrie : normales...
    m_Mesh->computeNormals();


    /// Transformations (décommenter celle qu'on veut appliquer)

    // module de transformation
    MeshModuleProcessing transformation(m_Mesh);

    // tronquer le coin s0
    transformation.bevelVertex(s0, 1.0, vec3::fromValues(0,1,0));

    // subdiviser
    //std::vector<Triangle*> triangles = transformation.getTrianglesInsideBorder([s1, s2, s3, s4, s5, s6]);
    //transformation.subdivideAll(triangles, 3);

    //std::vector<Triangle*> triangles = transformation.getTrianglesInsideBorder([s0, s4, s5, s6]);
    //transformation.subdivideAll(triangles, 1);

    // vérification avant préparation pour l'affichage
    transformation.checkMesh(false);

    // calculer les normales
    m_Mesh->computeNormals();
}


/**
 * Cette méthode supprime les ressources allouées
 */
Corner::~Corner()
{
    delete m_Mesh;
}
