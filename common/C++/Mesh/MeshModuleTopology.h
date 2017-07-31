#ifndef MESH_MESHMODULETOPOLOGY_H
#define MESH_MESHMODULETOPOLOGY_H

/**
 * Ce module rajoute des fonctions et des classes pour créer des maillages particuliers
 *
 */

#include <MeshModule.h>

class MeshModuleTopology: public MeshModule
{

public:

    /**
     * initialise un générateur de nappe sur le maillage fourni
     * @param maillage à gérer
     */
    MeshModuleTopology(Mesh* mesh=nullptr);

    /**
     * ajoute une nappe en forme de rectangle ayant nbX sommets sur l'axe X et nbZ sur l'axe Z.
     * @param nbX nombre de sommets sur l'axe X
     * @param nbZ nombre de sommets sur l'axe Z
     * @param noms des sommets, doit contenir deux %d, ex "grille%d-%d" qui sont remplacés par ix et iz
     * @param foldX true si la grille se referme sur elle-même en X
     * @param foldZ true si la grille se referme sur elle-même en Z
     * @return num0 numéro du premier sommet ajouté au maillage
     * NB : le numéro d'un point (ix, iz) est ix + iz*nbX + num0
     * NB : les triangles sont créés dans un ordre qui permet de créer un VBOset de type Ruban rapidement
     */
    int addRectangularSurface(int nbX, int nbZ, std::string noms, bool foldX, bool foldZ);

    /**
     * ajoute une nappe de triangles équilatéraux ayant nbX sommets sur l'axe X et nbZ sur l'axe Z.
     * @param nbX nombre de sommets sur l'axe X
     * @param nbZ nombre de sommets sur l'axe Z
     * @param noms des sommets, doit contenir deux %d, ex "grille%d-%d" qui sont remplacés par ix et iz
     * @param foldX true si la grille se referme sur elle-même en X
     * @param foldZ true si la grille se referme sur elle-même en Z
     * @return num0 numéro du premier sommet ajouté au maillage
     * NB : le numéro d'un point (ix, iz) est ix + iz*nbX + num0
     * NB : les triangles sont créés dans un ordre qui permet de créer un VBOset de type Ruban rapidement
     */
    int addHexagonalSurface(int nbX, int nbZ, std::string noms, bool foldX, bool foldZ);

    /**
     * ajoute une nappe en forme de toile d'araignée avec un point central. Cette toile contient un sommet central
     * de numéro num0 (retourné par cette méthode). Ensuite les sommets sont agencés en cercles concentriques.
     * Un sommet est noté (ir, is) avec ir = numéro de rayon (angle autour du centre) et is = distance du centre.
     * @param spokes_count : nombre de rayons => angle = 360°/spokesNumber
     * @param segments_count : nombre de segments sur les rayons, c'est le nombre de cercles de sommets autour du centre,
     * il n'y a que le centre si ce nombre est nul
     * @param names : noms des sommets, doit contenir deux %d, ex: "disque%d-%d" qui sont remplacés par ir et is
     * @return num0 : numéro du premier sommet ajouté au maillage, c'est le centre du disque
     * NB : le numéro d'un point (ir, is) est ir*nbSegments + is + num0+1, le centre est num0
     * NB : les triangles sont créés dans un ordre qui permet de créer un VBOset de type Ruban rapidement
     */
    int addRevolutionSurface(int spokes_count, int segments_count, std::string names);

};

#endif
