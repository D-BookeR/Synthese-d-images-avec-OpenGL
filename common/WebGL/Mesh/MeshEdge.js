/**
 * Cette classe représente le bord d'un triangle, le couple de deux sommets appartenant
 * au même triangle. Une seule arête est associée à chaque demi-arête isolée ou paire
 * de demi-arêtes opposées.
 */

class MeshEdge
{
    /** variables membres
    // appartenance au maillage
    private Mesh m_Mesh;

    // sommets de l'arête, NB: vertex1.getNumber() < vertex2.getNumber()
    private MeshVertex m_Vertex1;
    private MeshVertex m_Vertex2;
    **/


    /**
     * Constructeur de la classe Edge. C'est simplement un couple de deux sommets
     * @param mesh maillage d'appartenance de cette arête
     * @param vertex1 premier sommet
     * @param vertex2 second sommet
     */
    constructor(mesh, vertex1, vertex2)
    {
        // appartenance au maillage
        this.m_Mesh = mesh;
        if (mesh != null) this.m_Mesh.pushEdge(this);

        // classer selon les numéros de sommets
        if (vertex1.getNumber() < vertex2.getNumber()) {
            // vertex1 a le plus petit numéro
            this.m_Vertex1 = vertex1;
            this.m_Vertex2 = vertex2;
        } else {
            // vertex2 a le plus petit numéro
            this.m_Vertex1 = vertex2;
            this.m_Vertex2 = vertex1;
        }
    }


    /** destructeur */
    destroy()
    {
        // supprimer cette arête de la liste du maillage
        if (this.m_Mesh != null) this.m_Mesh.popEdge(this);
        this.m_Mesh = null;
    }


    /**
     * retourne une chaîne représentant cette arête
     * @return nom de l'arête
     */
    toString()
    {
        return "Edge("+this.m_Vertex1.getName()+","+this.m_Vertex2.getName()+")";
    }


    /**
     * retourne une chaîne représentant cette arête de manière détaillée
     * @return description de l'arête
     */
    toFullString()
    {
        return this.toString();
    }


    /**
     * remplace vertex1 par vertex2 dans l'arête
     * @param vertex1 : sommet présent dans l'arête
     * @param vertex2 : autre sommet à mettre à sa place
     * @return true si le remplacement est ok
     */
    replaceVertex(vertex1, vertex2)
    {
        if (this.m_Vertex1 == vertex1) {
            if (vertex2.getNumber() < this.m_Vertex2.getNumber()) {
                // vertex2 (qui remplace vertex1) a le plus petit numéro
                this.m_Vertex1 = vertex2;
            } else {
                // vertex2 (qui remplace vertex1) a le plus grand numéro
                this.m_Vertex1 = this.m_Vertex2;
                this.m_Vertex2 = vertex2;
            }
            return true;
        }
        if (this.m_Vertex2 == vertex1) {
            if (vertex2.getNumber() < this.m_Vertex1.getNumber()) {
                // vertex2 (qui remplace vertex1) a le plus petit numéro
                this.m_Vertex2 = this.m_Vertex1;
                this.m_Vertex1 = vertex2;
            } else {
                // vertex2 (qui remplace vertex1) a le plus grand numéro
                this.m_Vertex2 = vertex2;
            }
            return true;
        }
        return false;
    }


    /**
     * réaffecte le maillage associé à this
     * @param mesh : maillage
     */
    setMesh(mesh)
    {
        this.m_Mesh = mesh;
        // TODO vérifier s'il n'y a pas déjà un maillage associé
    }


    /**
     * retourne le maillage contenant this
     * @return maillage
     */
    getMesh()
    {
        return this.m_Mesh;
    }


    /**
     * retourne le premier sommet de l'arête
     * @return sommet n°1
     */
    getVertex1()
    {
        return this.m_Vertex1;
    }


    /**
     * retourne le second sommet de l'arête
     * @return sommet n°2
     */
    getVertex2()
    {
        return this.m_Vertex2;
    }
}
