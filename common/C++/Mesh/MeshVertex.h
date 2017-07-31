#ifndef MESH_MESHVERTEX_H
#define MESH_MESHVERTEX_H

// Définition de la classe MeshVertex

#include <vector>
#include <map>
#include <list>
#include <string>

#include <gl-matrix.h>
#include <utils.h>


#include <Mesh.h>

// pour le module MeshModuleRedux
class Quadric;

//#define DEBUG



/**
 * Cette classe représente un sommet dans le maillage.
 */
class MeshVertex
{
private:

    /// nom du sommet
    std::string m_Name;

    /// numéro du sommet
    int m_Number;

    // attributs du sommet, voir setCoord(),  setColor(), etc.
    std::vector<vec4> m_Attributes;

    /// maillage d'appartenance de ce sommet
    Mesh* m_Mesh;
    MeshHalfEdge* m_HalfEdge;       // demi-arête sortant de ce sommet

    /* pour le module MeshModuleRedux */

    MeshVertex* m_CollapseTarget;
    float m_CollapseCost;
    Quadric* m_Quadric;


#ifdef DEBUG
    static int m_DebugGenNumber;
    int m_DebugNumber;
#endif


public:

    /**
     * Constructeur de la classe MeshVertex qui représente un sommet dans
     * le maillage. Initialise un sommet non connecté au maillage, employer
     * Maillage.addVertex pour l'insérer puis new Triangle(...) pour le mettre
     * dans un triangle, setCoord et setColor pour lui donner des coordonnées et
     * des couleurs.
     * @param mesh : maillage d'appartenance de ce sommet
     * @param nom : nom du sommet à créer
     */
    MeshVertex(Mesh* mesh, std::string name);

    /**
     * Destructeur de la classe MeshVertex
     */
    ~MeshVertex();

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
     * @param mesh : maillage du sommet
     */
    void setMesh(Mesh* mesh);

    /**
     * retourne la première demi-arête de this
     * @return MeshHalfEdge
     */
    MeshHalfEdge* getHalfEdge()
    {
        return m_HalfEdge;
    }

    /**
     * cette méthode rajoute la demi-arête dans le chaînage m_Sibling (demi-arêtes autour de this)
     * @see #unlinkSibling qui fait l'inverse
     * @param halfedge demi-arête à rajouter dans la liste m_Sibling de this
     */
    void linkSibling(MeshHalfEdge* halfedge);

    /**
     * cette méthode retire cette demi-arête de la liste m_Sibling (demi-arêtes autour de this)
     * @see #linkSibling qui fait l'inverse
     * @param halfedge demi-arête à enlever de la liste m_Sibling de this
     */
    void unlinkSibling(MeshHalfEdge* halfedge);

    /**
     * retourne la liste des triangles autour du sommet this
     * Cette liste est basée sur les liens m_Sibling entre triangles
     * NB : la liste n'est pas du tout triée par ordre autour du sommet
     * @see #getTrianglesOrderedAround pour une liste triée mais pas forcément complète
     * @return liste des triangles
     */
    std::vector<MeshTriangle*> getTrianglesAround();

    /**
     * retourne la liste des triangles autour du sommet this
     * Cette liste est basée sur les liens m_Next et m_Opposite entre triangles
     * NB : la liste est triée par ordre autour du sommet, mais si le maillage n'est
     * pas complet, alors il manquera une partie des triangles
     * @see #getTrianglesAutour pour une liste non triée mais complète
     * @return liste des triangles
     */
    std::vector<MeshTriangle*> getTrianglesOrderedAround();

    /**
     * Cette méthode retourne la collection des sommets contenus dans les triangles
     * qui contiennent le sommet this, c'est à dire les voisins de this.
     * @return liste des sommets voisins du sommet, mais sans this
     */
    std::set<MeshVertex*> getNeighborVertices();

    /**
     * retourne la MeshHalfEdge allant de this à sommet
     * @param vertex : destination de la demi-arête recherchée
     * @return demi-arête allant de this à sommet, ou nullptr si absente
     */
    MeshHalfEdge* getHalfEdgeTo(MeshVertex* vertex);

    /**
     * retourne le triangle situé à droite de l'arête allant de this à sommet
     * @param vertex : second sommet
     * @return triangle ou nullptr si aucun
     */
    MeshTriangle* getTriangleRightTo(MeshVertex* vertex);

    /**
     * retourne le triangle situé à gauche de l'arête allant de this à sommet
     * @param vertex : second sommet
     * @return triangle ou nullptr si aucun
     */
    MeshTriangle* getTriangleLeftTo(MeshVertex* vertex);

    /**
     * retourne le nom du sommet
     * @return le nom du sommet
     */
    std::string toString();

    /**
     * retourne une chaîne représentant ce sommet de manière détaillée
     * @return description du sommet
     */
    std::string toFullString();

    /**
     * retourne le nom du vertex
     * @return nom du sommet
     */
    std::string getName()
    {
        return m_Name;
    }

    /**
     * modifie le numéro du vertex
     * @param number : numero du sommet
     */
    void setNumber(int number);

    /**
     * retourne le numéro du vertex
     * @return numéro du sommet
     */
    int getNumber()
    {
        return m_Number;
    }

    /**
     * définit les coordonnées du sommet
     * @param xyz coordonnées
     * @return this pour pouvoir chaîner les affectations
     */
    MeshVertex* setCoord(vec3 xyz);

    /**
     * retourne les coordonnées du sommet
     * @return coordonnées 3D du sommet
     */
    vec3& getCoord()
    {
        return (vec3&)m_Attributes[ID_ATTR_VERTEX];
    }

    /**
     * définit la couleur du sommet
     * @param rgba couleur (r,g,b,a)
     * @return this pour pouvoir chaîner les affectations
     */
    MeshVertex* setColor(vec4 rgba);


    /**
     * définit la couleur du sommet
     * @param rgb couleur (r,g,b)
     * @return this pour pouvoir chaîner les affectations
     */
    MeshVertex* setColor(vec3 rgb);

    /**
     * retourne la couleur du sommet
     * @return couleur (r,g,b,a)
     */
    vec4& getColor()
    {
        return m_Attributes[ID_ATTR_COLOR];
    }

    /**
     * définit les coordonnées de la normale du sommet
     * @param normal normale à affecter
     * @return this pour pouvoir chaîner les affectations
     */
    MeshVertex* setNormal(vec3 normal);

    /**
     * retourne la normale du sommet
     * @return normale
     */
    vec3& getNormal()
    {
        return (vec3&)m_Attributes[ID_ATTR_NORMAL];
    }

    /**
     * retourne la tangente du sommet
     * @return tangente
     */
    vec3& getTangent()
    {
        return (vec3&)m_Attributes[ID_ATTR_TANGENT];
    }


    /**
     * définit les coordonnées de texture du sommet
     * @param uv coordonnées de texture
     * @return this pour pouvoir chaîner les affectations
     */
    MeshVertex* setTexCoord(vec2 uv);

    /**
     * retourne les coordonnées de texture du sommet
     * @return coordonnées de texture
     */
    vec2& getTexCoord()
    {
        return (vec2&)m_Attributes[ID_ATTR_TEXCOORD];
    }


    /**
     * Constantes prédéfinies pour les attributs de base d'un vertex
     * voir MeshVertex.getAttribut, c'est le code qu'il faut pour idattr
     */
    static const int ID_ATTR_VERTEX     = 0;
    static const int ID_ATTR_COLOR      = 1;
    static const int ID_ATTR_NORMAL     = 2;
    static const int ID_ATTR_TANGENT    = 3;
    static const int ID_ATTR_TEXCOORD   = 4;
    static const int ID_ATTR_VERTEX1    = 5;
    static const int ID_ATTR_NORMAL1    = 6;
    static const int ID_ATTR_IDBONES    = 7;
    static const int ID_ATTR_WEIGHTS    = 8;

    /**
     * retourne l'attribut dont on fournit l'identifiant
     * @param idattr : numéro de l'attribut, ex: MeshVertex::ID_ATTR_VERTEX
     * @return vec4 contenant l'attribut
     */
    vec4& getAttribute(int idattr)
    {
        return m_Attributes[idattr];
    }

    /**
     * affecte ou définit l'attribut dont on fournit l'identifiant
     * @param idattr : numéro de l'attribut, ex: MeshVertex::ID_ATTR_VERTEX
     * @param value valeur à affecter à l'attribut
     * @return this pour pouvoir chaîner les affectations
     */
    MeshVertex* setAttribute(int idattr, vec4 value);
    MeshVertex* setAttribute(int idattr, vec3 value);
    MeshVertex* setAttribute(int idattr, vec2 value);

    /**
     * Cette méthode calcule la normale du sommet = moyenne des normales des
     * triangles contenant ce sommet.
     */
    void computeNormal();

    /**
     * Cette méthode calcule la tangente du sommet = moyenne des tangentes des
     * triangles contenant ce sommet.
     */
    void computeTangent();

    /**
     * retourne le sommet visé pour une fusion
     * @see MeshModuleRedux
     * @return sommet
     */
    MeshVertex* getCollapseTarget()
    {
        return m_CollapseTarget;
    }

    /**
     * modifie le sommet visé pour une fusion
     * @see MeshModuleRedux
     * @param vertex : sommet à affecter
     */
    void setCollapseTarget(MeshVertex* vertex);

    /**
     * retourne le coût d'une fusion vers m_CollapseTarget
     * @see MeshModuleRedux
     * @return coût
     */
    float getCollapseCost()
    {
        return m_CollapseCost;
    }

    /**
     * modifie le coût d'une fusion vers m_CollapseTarget
     * @see MeshModuleRedux
     * @param cost : coût à affecter
     */
    void setCollapseCost(float cost);

    /**
     * retourne la quadrique permettant de calculer le coût d'une réduction
     * @see MeshModuleRedux
     * @return quadrique
     */
    Quadric* getQuadric()
    {
        return m_Quadric;
    }

    /**
     * modifie la quadrique permettant de calculer le coût d'une réduction
     * @see MeshModuleRedux
     * @param quadric quadrique à affecter
     */
    void setQuadric(Quadric* quadric);

    /**
     * retourne une copie de ce sommet (this)
     * @param suffix : chaine à rajouter au nom de this
     * @return copie du sommet this
     */
    MeshVertex* clone(std::string suffix);

    /**
     * Cette méthode modifie les attributs du vertex pour qu'ils soient interpolés
     * linéairement entre ceux de v0 et de v1, d'un facteur k
     * @param v0 : premier vertex, (k=0)
     * @param v1 : second vertex (k=1)
     * @param k : valeur entre 0 et 1 pour l'interpolation
     */
    void lerp(const MeshVertex* v0, const MeshVertex* v1, const float k);

    /**
     * Cette méthode modifie les attributs du vertex pour qu'ils soient interpolés
     * selon Hermite entre ceux de v0 et de v1, d'un facteur k
     * @param v0 : premier vertex, (k=0)
     * @param t0 : tangente à k=0
     * @param v1 : second vertex (k=1)
     * @param t1 : tangente à k=1
     * @param k : coefficient d'interpolation
     */
    void hermite(const MeshVertex* v0, const vec3& t0, const MeshVertex* v1, const vec3& t1, const float k);

};


#endif
