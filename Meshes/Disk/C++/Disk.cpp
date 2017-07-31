#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>
#include <sstream>
#include <math.h>

#include <utils.h>

#include <Disk.h>
#include <DeferredShadingMaterial.h>



static std::string name(std::string racine, int number)
{
    std::stringstream ss;
    ss << racine << number;
    return ss.str();
}



/**
 * remplace chaque triangle du maillage par trois triangles appuyés sur
 * le barycentre de l'ancien triangle
 * @param mesh : maillage concerné par l'opération
 * @param height : amplitude relative du déplacement du barycentre
 */
void subdivideTriangles(Mesh* mesh, float height)
{
    // calculer les normales des triangles et des sommets
    mesh->computeNormals();

    std::vector<MeshTriangle*> triangles;
    for (MeshTriangle* triangle: mesh->getTriangleList()) {
        triangles.push_back(triangle);
    }

    // traiter tous les triangles
    for (MeshTriangle* triangle: triangles) {
        // sommets du triangle courant
        MeshVertex* s0 = triangle->getVertex0();
        MeshVertex* s1 = triangle->getVertex1();
        MeshVertex* s2 = triangle->getVertex2();
        // rajouter un sommet au milieu des trois
        std::stringstream ssnom;
        ssnom << "c" << s0->getName() << s1->getName() << s2->getName();
        MeshVertex* center = mesh->addVertex(ssnom.str());
        // ses coordonnées
        vec3 coords = vec3::clone(triangle->getCenter());
        // amplitude du déplacement
        float dist = sqrt(triangle->getSurface()) * height;
        // déplacer le centre en suivant la normale
        vec3 displacement = vec3::create();
        vec3::scale(displacement, triangle->getNormal(), dist);
        vec3::add(coords, coords, displacement);
        center->setCoord(coords);
        // supprimer l'ancien triangle
        delete triangle;
        // construire trois triangles
        mesh->addTriangle(center, s0, s1);
        mesh->addTriangle(center, s1, s2);
        mesh->addTriangle(center, s2, s0);
    }
}


Disk::Disk()
{
    // créer une sorte de disque convexe

    m_Mesh = new Mesh("Disk");

    /// Topologie : éléments et connexions

    const int N = 6;

    // rajouter des sommets
    for (int i=0; i<=N; i++) {
        m_Mesh->addVertex(name("s",i));
    }

    // rajouter des triangles autour du premier sommet
    for (int i=0; i<N; i++) {
        m_Mesh->addTriangle(
            m_Mesh->getVertex(0),
            m_Mesh->getVertex(i+1),
            m_Mesh->getVertex((i+1)%N + 1));
    }

    /// Géométrie : position des sommets et autres infos

    // affectation des coordonnées des sommets
    const float R = 1.6;
    m_Mesh->getVertex(0)->setCoord(vec3::fromValues( 0.0,+0.5 * R, 0.0));
    m_Mesh->getVertex(0)->setColor(vec3::fromValues(0.2, 0.5, 0.3));
    for (int i=0; i<N; i++) {
        float angle = i * 2.0f * M_PI / N;
        m_Mesh->getVertex(i+1)->setCoord(vec3::fromValues(R * sin(angle), 0.0, R * cos(angle)));
        m_Mesh->getVertex(i+1)->setColor(vec3::fromValues(0.2, 0.5, 0.3));
    }

    // appliquer une subdivision
    subdivideTriangles(m_Mesh, 0.05);

    // calculer les normales
    m_Mesh->computeNormals();
}


/**
 * Cette méthode supprime les ressources allouées
 */
Disk::~Disk()
{
    delete m_Mesh;
}
