#ifndef MESH_MESHTRIANGLE_H
#define MESH_MESHTRIANGLE_H

// Définition de la classe MeshTriangle

#include <iterator>
#include <vector>
#include <map>
#include <list>
#include <string>

#include <gl-matrix.h>
#include <utils.h>

#include <Mesh.h>


/**
 * Cette classe permet d'itérer sur les sommets d'un triangle
 */
class VertexList
{
public:

    class iterator
    {
    private:

        int m_I;                      // nombre de sommets restant à retourner
        MeshHalfEdge* m_HalfEdge;     // demi-arête courante

    public:

        iterator(MeshTriangle* triangle, int i);

        MeshVertex* operator*() const;
        const iterator& operator++();
        bool operator!=(const iterator& other) const;
    };

private:

    MeshTriangle* m_Triangle;       // triangle en cours de parcours

public:

    VertexList(MeshTriangle* triangle) { m_Triangle = triangle; };

    iterator begin() const { return iterator(m_Triangle, 3); };
    iterator end()   const { return iterator(m_Triangle, 0); };
};



/**
 * Cette classe représente un triangle dans le maillage.
 */
class MeshTriangle
{
private:

    /// maillage d'appartenance du triangle
    Mesh* m_Mesh;
    MeshHalfEdge* m_HalfEdge;

    // attributs de triangle
    vec3 m_Normal;
    vec3 m_Tangent;
    vec3 m_Center;
    float m_Surface;

    /// coefficient W du triangle (définit le plan du triangle)
    float m_W;

#ifdef DEBUG
    static int m_DebugGenNumero;
    int m_DebugNumero;
#endif


public:

     /**
     * Constructeur de la classe Triangle à partir de trois sommets
     * NB: l'ordre de rotation des sommets est crucial pour le calcul des normales.
     * Il faut tourner dans le sens trigonométrique, comme dans OpenGL.
     * @param maillage : maillage dans lequel on rajoute ce triangle
     * @param v1 : l'un des coins du triangle, nullptr si création manuelle
     * @param v2 : l'un des coins du triangle, nullptr si création manuelle
     * @param v3 : l'un des coins du triangle, nullptr si création manuelle
     */
    MeshTriangle(Mesh* mesh, MeshVertex* v1=nullptr, MeshVertex* v2=nullptr, MeshVertex* v3=nullptr);
    MeshTriangle(Mesh* mesh);

    /**
     * Cette méthode supprime ce triangle du maillage en mettant à jour toutes
     * les listes. Cela peut supprimer des arêtes et rendre des sommets isolés.
     */
    ~MeshTriangle();

    /**
     * retourne une chaîne qui décrit le triangle
     * @return nom du triangle
     */
    std::string toString();

    /**
     * retourne une chaîne qui décrit le triangle de manière détaillée
     * @return description du triangle
     */
    std::string toFullString();

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
     * @param mesh maillage
     */
    void setMesh(Mesh* mesh);

    /**
     * retourne les sommets du triangle sous forme d'un itérable
     * @return itérable sur les trois sommets du triangle
     */
    VertexList getVertexList();

    /**
     * retourne le sommet n°n (0..2) du triangle, ou nullptr si n n'est pas correct
     * @param n : numéro 0..2 du sommet
     * @return le MeshVertex() demandé ou nullptr si n n'est pas dans les bornes
     */
    MeshVertex* getVertex(int n);

    /**
     * retourne la demi-arête du triangle
     * @return demi-arête
     */
    MeshHalfEdge* getHalfEdge()
    {
        return m_HalfEdge;
    }

    /**
     * retourne le premier sommet du triangle
     * @return premier sommet
     */
    MeshVertex* getVertex0();

    /**
     * retourne le deuxième sommet du triangle
     * @return deuxième sommet
     */
    MeshVertex* getVertex1();

    /**
     * retourne le troisième sommet du triangle
     * @return troisième sommet
     */
    MeshVertex* getVertex2();

    /**
     * retourne la valeur de la normale
     * @see #calcNormal pour la calculer auparavant
     * @return normale du triangle
     */
    vec3 getNormal()
    {
        return m_Normal;
    }

    /**
     * retourne la valeur de la surface
     * @see #calcNormal pour la calculer auparavant
     * @return surface du triangle
     */
    float getSurface()
    {
        return m_Surface;
    }

    /**
     * retourne le centre du triangle
     * @see #calcNormal pour le calculer auparavant
     * @return centre du triangle
     */
    vec3 getCenter()
    {
        return m_Center;
    }

    /**
     * retourne la valeur w du triangle
     * @return w
     */
    float getW()
    {
        return m_W;
    }

    /**
     * affecte la variable w du triangle
     * @param w
     */
    void setW(float w);

    /**
     * recalcule les informations géométriques du triangle : centre, normale, surface...
     */
    void computeNormal();

    /**
     * retourne la valeur de la tangente
     * @see #calcTangente pour la calculer auparavant
     * @return tangente du triangle
     */
    vec3 getTangent()
    {
        return m_Tangent;
    }

    /**
     * recalcule la tangente du triangle à l'aide de la normale et des coordonnées de texture
     */
    void computeTangent();

    /**
     * Cette méthode fait tourner les sommets du triangle afin que
     * celui qui est passé en paramètre se trouve en premier
     * @param vertex : sommet à mettre en tête de liste (sauf s'il en est absent)
     * @return true si ok, false si le sommet est absent du triangle
     */
    bool cycleVertexFirst(MeshVertex* vertex);

    /**
     * Cette méthode indique si le triangle this contient le sommet indiqué
     * @param vertex : sommet dont il faut vérifier l'appartenance à this
     * @return true si ok, false si le sommet est absent du triangle
     */
    bool containsVertex(MeshVertex* vertex);

    /**
     * Cette méthode indique si le triangle this contient l'arête indiquée. Et
     * si c'est le cas, alors sommet1 est mis en premier dans le triangle. Le
     * triangle est transformé en (sommet1, sommet2, 3e sommet).
     * @param vertex1 : premier sommet de l'arête (elle est orientée de 1 à 2)
     * @param vertex2 : second sommet
     * @return true si oui, false si l'arête n'est pas dans le triangle
     */
    bool containsEdge(MeshVertex* vertex1, MeshVertex* vertex2);

    /**
     * remplace un sommet par un autre dans ce triangle
     * @param vertex1 sommet à remplacer
     * @param vertex2 sommet à mettre à la place
     */
    void replaceVertex(MeshVertex* vertex1, MeshVertex* vertex2);

};

#endif
