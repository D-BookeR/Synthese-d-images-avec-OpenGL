#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>
#include <math.h>

#include <utils.h>
#include <MeshModuleLoading.h>
#include <MeshModuleRedux.h>
#include <MeshModulePhysics.h>

#include <Cow.h>


/**
 * Constructeur
 * @param nombre de sommets à supprimer par simplification de maillage
 */
Cow::Cow(int nombre)
{
    m_Mesh = new Mesh("Cow");
    MeshModuleLoading loader(m_Mesh);
    loader.loadObjFile("data/models/Cow/Cow.obj", "", 0.15);

    // définir la couleur de tous les sommets
    const int vertex_count = m_Mesh->getVertexCount();
    for (int i=0; i<vertex_count; i++) {
        m_Mesh->getVertex(i)->setColor(vec4::fromValues(1.0, 0.7, 0.6, 1.0));
    }
    std::cout << "La vache contient " << m_Mesh->getTriangleCount() << " triangles" << std::endl;

    // appliquer une réduction : supprimer un certain nombre de sommets
    MeshModuleRedux reduction(m_Mesh);
    m_Mesh->computeNormals();
    reduction.reduxCount(nombre);
    std::cout << "Après réduction, la vache contient seulement " << m_Mesh->getTriangleCount() << " triangles" << std::endl;

    // calculer les informations physiques
    MeshModulePhysics physique(m_Mesh);
    physique.setDensity(1.0);
    physique.compVolumeIntegrals();
}


/**
 * Cette méthode supprime les ressources allouées
 */
Cow::~Cow()
{
    delete m_Mesh;
}
