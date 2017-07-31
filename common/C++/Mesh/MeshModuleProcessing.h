#ifndef MESH_MESHMODULEPROCESSING_H
#define MESH_MESHMODULEPROCESSING_H

#include <vector>

#include <MeshModuleUtils.h>
#include <MeshTriangle.h>
#include <MeshVertex.h>


class MeshModuleProcessing: public MeshModuleUtils
{

public:

    /**
     * initialise le module sans maillage
     */
    MeshModuleProcessing();

    /**
     * initialise le module sur le maillage fourni
     * @param mesh maillage concerné
     */
    MeshModuleProcessing(Mesh* mesh);

    /**
     * Cette méthode fait une homothétie du triangle autour de son centre
     * @param triangle auquel on applique la transformation
     * @param scale : rapport d'agrandissement (si > 1.0) ou de réduction (si < 1.0)
     * @return triangle : le triangle auquel on applique cette méthode
     */
    static MeshTriangle* homothety(MeshTriangle* triangle, float scale);


    /**
     * Cette méthode casse l'angle au niveau du sommet indiqué. Elle
     * enfonce le sommet de la distance selon la direction fournie en paramètre.
     * @param vertex : celui qu'il faut tronquer
     * @param distance : distance où vont aller les points intermédiaires
     * @param direction : vec3 indiquant l'axe d'enfoncement des points
     * @return liste des sommets du contour résultant de la découpe
     */
    std::vector<MeshVertex*> bevelVertex(MeshVertex* vertex, float distance, vec3 direction);

    /**
     * Cette méthode clone les sommets spécifiés dans le contour afin
     * de la rendre dure (séparer deux groupes de lissage de part et d'autre
     * NB: ce sont les sommets des triangles "de gauche" qui sont clonés
     * @param border : liste de sommets (l'ordre est important), il en faut au moins 2
     */
    void splitBorder(std::vector<MeshVertex*> border);

    /**
     * Cette méthode fait une extrusion des triangles contenus dans le contour
     * et de la distance indiquée
     * @param border : liste de sommets, délimitent les triangles concernés qui sont à gauche du contour
     * @param distance : longueur de l'extrusion
     * @return liste des clones des sommets de tous les triangles inclus dans le contour
     */
    std::vector<MeshVertex*> extrudePolygon(std::vector<MeshVertex*> border, float distance);

    /**
     * Cette méthode fait une extrusion du triangle indiqué, le long de sa normale
     * et de la distance indiquée
     * @param triangle : celui qu'il faut extruder
     * @param distance : longueur de l'extrusion
     * @return nouveau triangle résultat de l'extrusion
     */
    MeshTriangle* extrudeTriangle(MeshTriangle* triangle, float distance);

    /**
     * Cette méthode subdivise le triangle en sous-triangles
     * @param triangle auquel on applique la transformation
     * @param steps : fournir un entier 1..raisonnable
     * @param smooth : déplace ou non les points milieux vers l'intérieur ou l'extérieur
     * @return liste de triangles qui doivent remplacer celui-ci
     */
    std::vector<MeshTriangle*> subdivide(MeshTriangle* triangle, int steps, float smooth);

    /**
     * Cette méthode subdivise les triangles indiqués
     * @param triangles : liste des triangles concernés par la subdivision
     * @param steps : fournir un entier 1..raisonnable
     * @param smooth : déplace ou non les points milieux vers l'intérieur ou l'extérieur
     * @return liste de triangles qui doivent remplacer celui-ci
     */
    std::vector<MeshTriangle*> subdivideAll(std::vector<MeshTriangle*> triangles, int steps, float smooth);

    /**
     * Cette méthode applique une matrice à tous les sommets du maillage
     * @param matrix : la transformation qu'il faut appliquer sur les sommets
     */
    void transform(mat4 matrix);

};

#endif
