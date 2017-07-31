/**
 * Cette classe représente le bord d'un triangle, le couple de deux sommets appartenant
 * au même triangle. Une seule arête est associée à chaque demi-arête isolée ou paire
 * de demi-arêtes opposées.
 */

package com.dbooker.livreopengl.mesh;


/**
 * Cette classe représente le bord d'un triangle, le couple de deux sommets appartenant
 * au même triangle
 */
@SuppressWarnings("unused")
public class MeshEdge
{
    // appartenance au maillage
    private Mesh m_Mesh;

    // sommets de l'arête, NB: vertex1.getNumber() < vertex2.getNumber()
    private MeshVertex m_Vertex1;
    private MeshVertex m_Vertex2;


    /**
     * Constructeur de la classe Edge. C'est simplement un couple de deux sommets
     * @param mesh maillage d'appartenance de cette arête
     * @param vertex1 premier sommet
     * @param vertex2 second sommet
     */
    MeshEdge(Mesh mesh, MeshVertex vertex1, MeshVertex vertex2)
    {
        // appartenance au maillage
        m_Mesh = mesh;
        if (mesh != null) m_Mesh.pushEdge(this);

        // classer selon les numéros de sommets
        if (vertex1.getNumber() < vertex2.getNumber()) {
            // vertex1 a le plus petit numéro
            m_Vertex1 = vertex1;
            m_Vertex2 = vertex2;
        } else {
            // vertex2 a le plus petit numéro
            m_Vertex1 = vertex2;
            m_Vertex2 = vertex1;
        }
    }


    /** destructeur */
    public void destroy()
    {
        destroy(true);
    }
    public void destroy(boolean remove_from_mesh)
    {
        // supprimer cette arête de la liste du maillage
        if (m_Mesh != null && remove_from_mesh) m_Mesh.popEdge(this);
        m_Mesh = null;
    }


    /**
     * retourne une chaîne représentant cette arête
     * @return nom de l'arête
     */
    public String toString()
    {
        return "Edge("+m_Vertex1.getName()+","+m_Vertex2.getName()+")";
    }


    /**
     * retourne une chaîne représentant cette arête de manière détaillée
     * @return description de l'arête
     */
    public String toFullString()
    {
        return toString();
    }


    /**
     * remplace vertex1 par vertex2 dans l'arête
     * @param vertex1 : sommet présent dans l'arête
     * @param vertex2 : autre sommet à mettre à sa place
     * @return true si le remplacement est ok
     */
    boolean replaceVertex(MeshVertex vertex1, MeshVertex vertex2)
    {
        if (m_Vertex1 == vertex1) {
            if (vertex2.getNumber() < m_Vertex2.getNumber()) {
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
            if (vertex2.getNumber() < m_Vertex1.getNumber()) {
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
    public void setMesh(Mesh mesh)
    {
        m_Mesh = mesh;
        // TODO vérifier s'il n'y a pas déjà un maillage associé
    }


    /**
     * retourne le maillage contenant this
     * @return maillage
     */
    public Mesh getMesh()
    {
        return m_Mesh;
    }


    /**
     * retourne le premier sommet de l'arête
     * @return sommet n°1
     */
    public MeshVertex getVertex1()
    {
        return m_Vertex1;
    }


    /**
     * retourne le second sommet de l'arête
     * @return sommet n°2
     */
    public MeshVertex getVertex2()
    {
        return m_Vertex2;
    }
}
