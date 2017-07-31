/**
 * Cette classe représente le bord d'un triangle, le couple de deux sommets appartenant
 * au même triangle. Une seule arête est associée à chaque demi-arête isolée ou paire
 * de demi-arêtes opposées.
 */

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <stdexcept>

#include <utils.h>
#include <MeshEdge.h>


#ifdef DEBUG
int MeshHalfEdge::m_DebugGenNumber = 0;
#endif


/**
 * Constructeur de la classe Edge. C'est simplement un couple de deux sommets
 * @param mesh maillage d'appartenance de cette arête
 * @param vertex1 premier sommet
 * @param vertex2 second sommet
 */
MeshEdge::MeshEdge(Mesh* mesh, MeshVertex* vertex1, MeshVertex* vertex2)
{
    // appartenance au maillage
    m_Mesh = mesh;
    if (mesh != nullptr) m_Mesh->pushEdge(this);

    // classer selon les numéros de sommets
    if (vertex1->getNumber() < vertex2->getNumber()) {
        // vertex1 a le plus petit numéro
        m_Vertex1 = vertex1;
        m_Vertex2 = vertex2;
    } else {
        // vertex2 a le plus petit numéro
        m_Vertex1 = vertex2;
        m_Vertex2 = vertex1;
    }

    #ifdef DEBUG
    m_DebugGenNumber++;
    m_DebugNumber = m_DebugGenNumber;
    std::cerr << "new " << toString() << std::endl;
    #endif
}


/** destructeur */
MeshEdge::~MeshEdge()
{
    #ifdef DEBUG
    std::cerr << "del " << toString() << std::endl;
    #endif

    // supprimer cette arête de la liste du maillage
    if (m_Mesh != nullptr) m_Mesh->popEdge(this);
}


/**
 * retourne une chaîne représentant cette arête
 * @return nom de l'arête
 */
std::string MeshEdge::toString()
{
    std::ostringstream str;
    #ifdef DEBUG
    str << "MeshEdge "<<m_DebugNumber<<" ("<<m_Vertex1->getName()<<","<<m_Vertex2->getName()<<")";
    #else
    str << "MeshEdge("<<m_Vertex1->getName()<<","<<m_Vertex2->getName()<<")";
    #endif
    return str.str();
}


/**
 * retourne une chaîne représentant cette arête de manière détaillée
 * @return description de l'arête
 */
std::string MeshEdge::toFullString()
{
    return toString();
}


/**
 * remplace vertex1 par vertex2 dans l'arête
 * @param vertex1 : sommet présent dans l'arête
 * @param vertex2 : autre sommet à mettre à sa place
 * @return true si le remplacement est ok
 */
bool MeshEdge::replaceVertex(MeshVertex* vertex1, MeshVertex* vertex2)
{
    if (m_Vertex1 == vertex1) {
        if (vertex2->getNumber() < m_Vertex2->getNumber()) {
            // vertex2 (qui remplace vertex1) a le plus petit numéro
            m_Vertex1 = vertex2;
        } else {
            // vertex2 (qui remplace vertex1) a le plus grand numéro
            m_Vertex1 = m_Vertex2;
            m_Vertex2 = vertex2;
        }
        return true;
    }
    if (m_Vertex2 == vertex1) {
        if (vertex2->getNumber() < m_Vertex1->getNumber()) {
            // vertex2 (qui remplace vertex1) a le plus petit numéro
            m_Vertex2 = m_Vertex1;
            m_Vertex1 = vertex2;
        } else {
            // vertex2 (qui remplace vertex1) a le plus grand numéro
            m_Vertex2 = vertex2;
        }
        return true;
    }
    return false;
}


/**
 * réaffecte le maillage associé à this
 * @param mesh : maillage
 */
void MeshEdge::setMesh(Mesh* mesh)
{
    m_Mesh = mesh;
    // TODO vérifier s'il n'y a pas déjà un maillage associé
}

