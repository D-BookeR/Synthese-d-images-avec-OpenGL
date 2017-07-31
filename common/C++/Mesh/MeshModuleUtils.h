#ifndef MESH_MESHMODULEUTILS_H
#define MESH_MESHMODULEUTILS_H

#include <set>

#include <MeshModule.h>
#include <MeshVertex.h>
#include <MeshTriangle.h>


// /**
//  * Cette fonction retourne la liste des sommets qui sont contenus dans les
//  * triangles fournis.
//  * @param resultat : tableau des sommets, peut être déjà pré-rempli
//  * @param triangles : tableau de triangles
//  */
// void getVertexList(std::vector<MeshVertex*> & resultat, std::vector<Triangle*> & triangles);

/**
 * Cette méthode calcule le vec3 interpolé par une spline cubique de Hermite entre (p0,t0), (p1,t1)
 * @note hermite3 pour des vec3, voir hermite2 pour des vec2...
 * @param out : résultat
 * @param p0 : coordonnées à k=0
 * @param t0 : tangente à k=0
 * @param p1 : coordonnées à k=1
 * @param t1 : tangente à k=1
 * @param k : coefficient d'interpolation
 */
void hermite3(vec3& out, const vec3& p0, const vec3& t0, const vec3& p1, const vec3& t1, const float k);

/**
 * Cette méthode calcule le vec2 interpolé par une spline cubique de Hermite entre (p0,t0), (p1,t1)
 * @note hermite2 pour des vec2, voir hermite3 pour des vec3...
 * @param out : résultat
 * @param p0 : coordonnées à k=0
 * @param t0 : tangente à k=0
 * @param p1 : coordonnées à k=1
 * @param t1 : tangente à k=1
 * @param k : coefficient d'interpolation
 */
void hermite2(vec2& out, const vec2& p0, const vec2& t0, const vec2& p1, const vec2& t1, const float k);




class MeshModuleUtils: public MeshModule
{

public:

    /**
     * initialise le module sur le maillage fourni
     * @param mesh : maillage concerné
     */
    MeshModuleUtils(Mesh* mesh=nullptr);

    /**
     * Cette méthode produit le nom du milieu entre p1 et p2, c'est la concaténation
     * du nom de p1 et de p2 mais dans l'ordre lexicographique afin que
     * nomMilieu(p1, p2) == nomMilieu(p2, p1) quels que soient p1 et p2
     * @param p1 : vertex
     * @param p2 : vertex
     * @return "m" + nom du plus petit sommet + nom du plus grand
     */
    std::string getMidName(MeshVertex* p1, MeshVertex* p2);

    /**
     * calcule la moyenne des normales des triangles fournis
     * @param triangles : liste des triangles à considérer
     * @return moyenne des normales
     */
    vec3 averageNormals(std::vector<MeshTriangle*>& triangles);

    /**
     * calcule la moyenne des tangentes des triangles fournis
     * @param triangles : liste des triangles à considérer
     * @return moyenne des tangentes
     */
    vec3 averageTangents(std::vector<MeshTriangle*>& triangles);

    /**
     * Cette fonction retourne la liste des sommets qui sont contenus dans les triangles fournis.
     * @param triangles : liste de triangles
     * @return tableau des sommets
     */
    std::set<MeshVertex*> getVerticesFromTriangles(std::vector<MeshTriangle*>& triangles);

    /**
     * Détermine les (deux) triangles qui sont à gauche de la demi-arête mais sans sortir
     * de la zone délimitée par le contour. Rajoute aussi tous les triangles qui sont en contact
     * avec les triangles déterminés au premier coup (appels récursifs).
     * En fait, c'est un algo de remplissage de tache appliqué à un maillage de triangles.
     * @param triangles : liste à laquelle sont rajoutés les triangles
     * @param halfedge : demi-arête de départ
     * @param border : liste de demi-arêtes qui délimitent les triangles intéressants
     */
    void appendNeighborTriangles(std::vector<MeshTriangle*>& triangles, MeshHalfEdge* halfedge, std::vector<MeshHalfEdge*>& border);

    /**
     * retourne la liste des triangles qui sont délimités par une succession de demi-arêtes
     * Le contour doit être refermé sur lui-même ou constituer une frontière d'un bord
     * à l'autre du maillage, sinon tous les triangles seront sélectionnés.
     * Attention à l'ordre de spécification des sommets du contour : ce sont les triangles
     * à gauche qui sont spécifiés (tourner dans le sens trigo pour entourer une zone).
     * @param border : liste des demi-arêtes. Les triangles sont à gauche d'elles
     * @return liste des triangles compris dans le contour
     */
    std::vector<MeshTriangle*> getTrianglesInsideBorder(std::vector<MeshHalfEdge*>& border);

    /**
     * retourne la liste des demi-arêtes correspondant à une liste de vertex formant un contour fermé
     * @param border : liste de sommets classés dans l'ordre, le dernier doit être lié au premier
     * @return liste des demi-arêtes
     * @throws Exception
     */
    std::vector<MeshHalfEdge*> getHalfEdgesAlongBorder(std::vector<MeshVertex*>& border);

    /**
     * Cette méthode calcule la valeur minimum des coordonnées
     * @return vec3 contenant min(x),min(y),min(z) des coordonnées des sommets
     */
    vec3 computeMinAABB();

    /**
     * Cette méthode calcule la valeur maximale des coordonnées
     * @return vec3 contenant max(x),max(y),max(z) des coordonnées des sommets
     */
    vec3 computeMaxAABB();

    /**
     * Cette méthode construit la boîte englobante du maillage this
     * @return maillage en forme de parallépipède rectangle
     */
    Mesh* createAABB();

    /**
     * Cette méthode vérifie la cohérence des informations du maillage.
     * La référence est la liste des sommets et la liste des triangles
     * NB : cette fonction ne teste pas toutes les erreurs possibles, mais
     * seulement quelques unes des plus graves.
     * @param repair : true s'il faut tenter de réparer les erreurs, ex: vertex non utilisé
     * @return true si le maillage semble correct, false s'il y a une grave erreur
     */
    bool checkMesh(bool repair=false);

    /**
     * Cette méthode affiche la totalité des structures de données du maillage
     */
    void dump();

};

#endif
