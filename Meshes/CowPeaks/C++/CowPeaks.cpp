#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <math.h>

#include <utils.h>
#include <MeshModuleLoading.h>

#include <CowPeaks.h>


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

    // copie de la liste du maillage
    std::vector<MeshTriangle*> triangles;
    for (MeshTriangle* triangle: mesh->getTriangleList()) {
        triangles.push_back(triangle);
    }

    // traiter tous les triangles
    for (MeshTriangle* triangle: triangles) {
        // triangle courant et ses sommets
        MeshVertex* s0 = triangle->getVertex0();
        MeshVertex* s1 = triangle->getVertex1();
        MeshVertex* s2 = triangle->getVertex2();
        // rajouter un sommet au milieu des trois
        MeshVertex* center = mesh->addVertex("c"+s0->getName()+s1->getName()+s2->getName());
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


CowPeaks::CowPeaks()
{
    // créer le maillage de la vache
    m_Mesh = new Mesh("CowPeaks");
    MeshModuleLoading loader(m_Mesh);
    loader.loadObjFile("data/models/Cow/Cow.obj", "", 0.15);

    // calculer les normales des triangles et des sommets
    m_Mesh->computeNormals();

    // appliquer une subdivision
    subdivideTriangles(m_Mesh, 1.0);

    // définir la couleur de tous les sommets
    for (int i=0; i<m_Mesh->getVertexCount(); i++) {
        m_Mesh->getVertex(i)->setColor(vec3::fromValues(0.9, 1.0, 0.2));
    }

    // calculer les normales (elles ne sont pas bonnes car interpolées)
    m_Mesh->computeNormals();
}


/**
 * Cette méthode supprime les ressources allouées
 */
CowPeaks::~CowPeaks()
{
    delete m_Mesh;
}
