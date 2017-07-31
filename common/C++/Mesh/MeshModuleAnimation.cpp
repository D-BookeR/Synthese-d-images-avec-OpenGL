/**
 * Ce module rajoute des fonctions et des classes pour calculer des déformations sur un maillage
 *
 */

#include <iostream>
#include <sstream>
#include <math.h>

#include <gl-matrix.h>
#include <MeshModuleAnimation.h>


/**
 * initialise le module sur le maillage fourni
 * @param mesh : maillage à gérer
 */
MeshModuleAnimation::MeshModuleAnimation(Mesh* mesh): MeshModule(mesh)
{
}



/**
 * associe le maillage autremaillage en tant que déformation de this->m_Mesh
 * @param othermesh : morph target
 */
void MeshModuleAnimation::buildMorph(Mesh* othermesh)
{
    // les deux maillages doivent avoir le même nombre de sommets
    int count = m_Mesh->getVertexCount();
    if (count == othermesh->getVertexCount()) {
        // utiliser les coordonnées et normales de othermesh en tant que morph target
        for (int i=0; i<count; i++) {
            MeshVertex* vertex = m_Mesh->getVertex(i);
            MeshVertex* morph  = othermesh->getVertex(i);
            vertex->setAttribute(MeshVertex::ID_ATTR_VERTEX1, morph->getCoord());
            vertex->setAttribute(MeshVertex::ID_ATTR_NORMAL1, morph->getNormal());
        }
    } else {
        std::cerr << "nombre de sommets différent entre "<<m_Mesh->getName()<<" et "<<othermesh->getName()<<std::endl;
        // recopier les coordonnées et normales du maillage en tant que morph target
        for (int i=0; i<count; i++) {
            MeshVertex* vertex = m_Mesh->getVertex(i);
            vertex->setAttribute(MeshVertex::ID_ATTR_VERTEX1, vertex->getCoord());
            vertex->setAttribute(MeshVertex::ID_ATTR_NORMAL1, vertex->getNormal());
        }
    }
}
