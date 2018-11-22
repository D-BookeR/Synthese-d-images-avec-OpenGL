// Définition de la classe Pyramid


#include <GL/glew.h>
#include <GL/gl.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <math.h>

#include <utils.h>

#include <Pyramid.h>


/**
 * Cette fonction définit la classe Pyramid.
 */
Pyramid::Pyramid()
{
    // créer le maillage
    m_Mesh = new Mesh("Pyramid");

    // créer les 5 sommets
    MeshVertex* s1 = m_Mesh->addVertex("s1");
    s1->setCoord(vec3::fromValues(-1,-1,-1));
    s1->setColor(vec3::fromValues(1.0, 0.0, 0.0));

    MeshVertex* s2 = m_Mesh->addVertex("s2");
    s2->setCoord(vec3::fromValues(+1,-1,-1));
    s2->setColor(vec3::fromValues(0.8, 1.0, 0.0));

    MeshVertex* s3 = m_Mesh->addVertex("s3");
    s3->setCoord(vec3::fromValues(+1,-1,+1));
    s3->setColor(vec3::fromValues(0.0, 1.0, 0.4));

    MeshVertex* s4 = m_Mesh->addVertex("s4");
    s4->setCoord(vec3::fromValues(-1,-1,+1));
    s4->setColor(vec3::fromValues(0.0, 0.4, 1.0));

    MeshVertex* s5 = m_Mesh->addVertex("s5");
    s5->setCoord(vec3::fromValues( 0,+1, 0));
    s5->setColor(vec3::fromValues(0.8, 0.0, 1.0));

    // créer le fond
    m_Mesh->addTriangle(s1,s2,s3);
    m_Mesh->addTriangle(s1,s3,s4);

    // créer la face devant
    m_Mesh->addTriangle(s4->clone("a"),s3->clone("a"),s5);

    // créer la face droite
    m_Mesh->addTriangle(s3->clone("b"),s2->clone("b"),s5->clone("b"));

    // créer la face arrière
    m_Mesh->addTriangle(s2->clone("c"),s1->clone("c"),s5->clone("c"));

    // créer la face gauche
    m_Mesh->addTriangle(s1->clone("d"),s4->clone("d"),s5->clone("d"));

    // calculer les normales
    m_Mesh->computeNormals();
}


/**
 * Cette méthode supprime les ressources allouées
 */
Pyramid::~Pyramid()
{
    delete m_Mesh;
}
