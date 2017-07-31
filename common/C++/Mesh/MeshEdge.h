#ifndef MESH_MESHEDGE_H
#define MESH_MESHEDGE_H

// Définition de la classe MeshEdge

#include <vector>
#include <map>
#include <list>
#include <string>

#include <gl-matrix.h>
#include <utils.h>

#include <MeshVertex.h>

//#define DEBUG

/**
 * Définit la classe Edge. C'est simplement un couple
 * de deux sommets
 */
class MeshEdge
{
private:

    Mesh* m_Mesh;
    MeshVertex* m_Vertex1;
    MeshVertex* m_Vertex2;

#ifdef DEBUG
    static int m_DebugGenNumber;
    int m_DebugNumber;
#endif


public:

    /**
     * Constructeur de la classe Edge. C'est simplement un couple de deux sommets
     * @param mesh maillage d'appartenance de cette arête
     * @param vertex1 premier sommet
     * @param vertex2 second sommet
     */
    MeshEdge(Mesh* mesh, MeshVertex* vertex1, MeshVertex* vertex2);

    /** destructeur */
    ~MeshEdge();

    /**
     * retourne une chaîne représentant cette arête
     * @return nom de l'arête
     */
    std::string toString();

    /**
     * retourne une chaîne représentant cette arête de manière détaillée
     * @return description de l'arête
     */
    std::string toFullString();

    /**
     * remplace vertex1 par vertex2 dans l'arête
     * @param vertex1 : sommet présent dans l'arête
     * @param vertex2 : autre sommet à mettre à sa place
     * @return true si le remplacement est ok
     */
    bool replaceVertex(MeshVertex* vertex1, MeshVertex* vertex2);

    /**
     * retourne le maillage contenant this
     * @return maillage
     */
    Mesh* getMesh()
    {
        return m_Mesh;
    }

    /**
     * modifie le maillage associé à this
     * @param mesh
     */
    void setMesh(Mesh* mesh);

    /**
     * retourne le premier sommet de l'arête
     * @return sommet n°1
     */
    MeshVertex* getVertex1()
    {
        return m_Vertex1;
    }

    /**
     * retourne le second sommet de l'arête
     * @return sommet n°2
     */
    MeshVertex* getVertex2()
    {
        return m_Vertex2;
    }
};

#endif
