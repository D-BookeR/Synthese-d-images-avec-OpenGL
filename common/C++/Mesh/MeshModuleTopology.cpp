/**
 * Ce module rajoute des fonctions et des classes pour créer des maillages particuliers
 *
 */

#include <iostream>
#include <sstream>
#include <math.h>

#include <gl-matrix.h>
#include <MeshModuleTopology.h>


/**
 * initialise un générateur de nappe sur le maillage fourni
 * @param mesh : maillage à gérer
 */
MeshModuleTopology::MeshModuleTopology(Mesh* mesh): MeshModule(mesh)
{
}


/**
 * Cette fonction remplace la première occurrence du joker dans la chaîne par la valeur
 * @param chaîne contenant éventuellement des jokers, le premier nous intéresse
 * @param joker par exemple "%d"
 * @param valeur à mettre à la place du premier %d
 * @return résultat
 */
static std::string replace(std::string line, std::string joker, int value)
{
    // chercher le premier %d et sortir si absent
    std::size_t position = line.find(joker);
    if (position == std::string::npos) return line;

    // écrire la valeur sous forme d'une chaîne
    // TODO formater la valeur selon le joker (nombre de chiffres, zéros...)
    std::stringstream ssvalue;
    ssvalue << value;

    // effectuer le remplacement
    return line.replace(position, joker.size(), ssvalue.str());
}


/**
 * ajoute une nappe en forme de rectangle ayant nbX sommets sur l'axe X et nbZ sur l'axe Z.
 * @param nbX : nombre de sommets sur l'axe X
 * @param nbZ : nombre de sommets sur l'axe Z
 * @param names : noms des sommets, doit contenir deux %d, ex "grille%d-%d" qui sont remplacés par ix et iz
 * @param foldX : true si la grille se referme sur elle-même en X
 * @param foldZ : true si la grille se referme sur elle-même en Z
 * @return num0 : numéro du premier sommet ajouté au maillage
 * NB : le numéro d'un point (ix, iz) est ix + iz*nbX + num0
 * NB : les triangles sont créés dans un ordre qui permet de créer un VBOset de type Ruban rapidement
 */
int MeshModuleTopology::addRectangularSurface(int nbX, int nbZ, std::string names, bool foldX, bool foldZ)
{
    // nombre de sommets existants déjà dans le maillage
    int num0 = m_Mesh->getVertexCount();

    // créer un rectangle de sommets
    for (int iz=0; iz<nbZ; iz++) {
        for (int ix=0; ix<nbX; ix++) {
            // créer le nom du sommet en replaçant les %d par ix et iz
            std::string name = replace(replace(names, "%d", ix), "%d", iz);
            // créer un sommet dans le maillage
            MeshVertex* vertex = m_Mesh->addVertex(name);
            // coordonnées par défaut du sommet
            vertex->setCoord(vec3::fromValues(ix, 0, iz));
        }
    }

    // créer les quads de la grille
    for (int iz=0; iz<nbZ-1; iz++) {
        for (int ix=0; ix<nbX-1; ix++) {
            // récupérer les 4 sommets concernés
            MeshVertex* v00 = m_Mesh->getVertex((ix  ) + (iz  )*nbX + num0);
            MeshVertex* v01 = m_Mesh->getVertex((ix  ) + (iz+1)*nbX + num0);
            MeshVertex* v10 = m_Mesh->getVertex((ix+1) + (iz  )*nbX + num0);
            MeshVertex* v11 = m_Mesh->getVertex((ix+1) + (iz+1)*nbX + num0);
            // ajouter un quad
            m_Mesh->addQuad(v00, v01, v11, v10);
        }
    }

    // si repli en X, rejouter des quads sur le bord Z
    if (foldX) {
        for (int iz=0; iz<nbZ-1; iz++) {
            // récupérer les 4 sommets concernés
            MeshVertex* v00 = m_Mesh->getVertex((    0) + (iz  )*nbX + num0);
            MeshVertex* v01 = m_Mesh->getVertex((    0) + (iz+1)*nbX + num0);
            MeshVertex* v10 = m_Mesh->getVertex((nbX-1) + (iz  )*nbX + num0);
            MeshVertex* v11 = m_Mesh->getVertex((nbX-1) + (iz+1)*nbX + num0);
            // ajouter un quad
            m_Mesh->addQuad(v10, v11, v01, v00);
        }
    }

    // si repli en Z, rejouter des quads sur le bord X
    if (foldZ) {
        for (int ix=0; ix<nbX-1; ix++) {
            // récupérer les 4 sommets concernés
            MeshVertex* v00 = m_Mesh->getVertex((ix  ) + (    0)*nbX + num0);
            MeshVertex* v01 = m_Mesh->getVertex((ix  ) + (nbZ-1)*nbX + num0);
            MeshVertex* v10 = m_Mesh->getVertex((ix+1) + (    0)*nbX + num0);
            MeshVertex* v11 = m_Mesh->getVertex((ix+1) + (nbZ-1)*nbX + num0);
            // ajouter un quad
            m_Mesh->addQuad(v00, v10, v11, v01);
        }
    }

    // si repli en X et Z, rajouter un quad dans le coin
    if (foldX && foldZ) {
        // récupérer les 4 sommets concernés
        MeshVertex* v00 = m_Mesh->getVertex((    0) + (    0)*nbX + num0);
        MeshVertex* v01 = m_Mesh->getVertex((    0) + (nbZ-1)*nbX + num0);
        MeshVertex* v10 = m_Mesh->getVertex((nbX-1) + (    0)*nbX + num0);
        MeshVertex* v11 = m_Mesh->getVertex((nbX-1) + (nbZ-1)*nbX + num0);
        // ajouter un quad
        m_Mesh->addQuad(v00, v01, v11, v10);
    }

    // retourner le numéro du premier point
    return num0;
}


/**
 * ajoute une nappe de triangles équilatéraux ayant nbX sommets sur l'axe X et nbZ sur l'axe Z.
 * @param nbX : nombre de sommets sur l'axe X
 * @param nbZ : nombre de sommets sur l'axe Z
 * @param names : noms des sommets, doit contenir deux %d, ex "grille%d-%d" qui sont remplacés par ix et iz
 * @param foldX : true si la grille se referme sur elle-même en X
 * @param foldZ : true si la grille se referme sur elle-même en Z
 * @return num0 : numéro du premier sommet ajouté au maillage
 * NB : le numéro d'un point (ix, iz) est ix + iz*nbX + num0
 * NB : les triangles sont créés dans un ordre qui permet de créer un VBOset de type Ruban rapidement
 */
int MeshModuleTopology::addHexagonalSurface(int nbX, int nbZ, std::string names, bool foldX, bool foldZ)
{
    // nombre de sommets existants déjà dans le maillage
    int num0 = m_Mesh->getVertexCount();

    // créer un rectangle de sommets
    for (int iz=0; iz<nbZ; iz++) {
        for (int ix=0; ix<nbX; ix++) {
            // créer le nom du sommet en replaçant les %d par ix et iz
            std::string name = replace(replace(names, "%d", ix), "%d", iz);
            // créer un sommet dans le maillage
            MeshVertex* vertex = m_Mesh->addVertex(name);
            // coordonnées par défaut du sommet
            vertex->setCoord(vec3::fromValues(ix-0.5*(iz%2), 0, iz * sqrt(3.0)/2.0));
        }
    }

    // créer les triangles de la grille
    for (int iz=0; iz<nbZ-1; iz++) {
        for (int ix=0; ix<nbX-1; ix++) {
            // récupérer les 4 sommets concernés
            MeshVertex* v00 = m_Mesh->getVertex((ix  ) + (iz  )*nbX + num0);
            MeshVertex* v01 = m_Mesh->getVertex((ix  ) + (iz+1)*nbX + num0);
            MeshVertex* v10 = m_Mesh->getVertex((ix+1) + (iz  )*nbX + num0);
            MeshVertex* v11 = m_Mesh->getVertex((ix+1) + (iz+1)*nbX + num0);
            // selon la parité de iz, créer deux triangles dans un sens ou dans l'autre
            if (iz%2 == 0) {
                m_Mesh->addTriangle(v00, v01, v11);
                m_Mesh->addTriangle(v00, v11, v10);
            } else {
                m_Mesh->addTriangle(v00, v01, v10);
                m_Mesh->addTriangle(v10, v01, v11);
            }
        }
    }

    // si repli en X, rejouter des triangles sur le bord Z
    if (foldX) {
        /// FIXME : selon la parité du nombre de sommets en Z
        for (int iz=0; iz<nbZ-1; iz++) {
            // récupérer les 4 sommets concernés
            MeshVertex* v00 = m_Mesh->getVertex((    0) + (iz  )*nbX + num0);
            MeshVertex* v01 = m_Mesh->getVertex((    0) + (iz+1)*nbX + num0);
            MeshVertex* v10 = m_Mesh->getVertex((nbX-1) + (iz  )*nbX + num0);
            MeshVertex* v11 = m_Mesh->getVertex((nbX-1) + (iz+1)*nbX + num0);
            // selon la parité de iz, créer deux triangles dans un sens ou dans l'autre
            if (iz%2 == 0) {
                m_Mesh->addTriangle(v10, v11, v01);
                m_Mesh->addTriangle(v10, v01, v00);
            } else {
                m_Mesh->addTriangle(v10, v11, v00);
                m_Mesh->addTriangle(v00, v11, v01);
            }
        }
    }

    // si repli en Z, rejouter des quads sur le bord X
    if (foldZ) {
        // afficher un avertissement si nbZ n'est pas pair
        if (nbZ%2 != 0) std::cerr << "ATTENTION: addHexagonalSurface, le nombre nbZ n'est pas pair : " << nbZ << std::endl;
        for (int ix=0; ix<nbX-1; ix++) {
            // récupérer les 4 sommets concernés
            MeshVertex* v00 = m_Mesh->getVertex((ix  ) + (    0)*nbX + num0);
            MeshVertex* v01 = m_Mesh->getVertex((ix  ) + (nbZ-1)*nbX + num0);
            MeshVertex* v10 = m_Mesh->getVertex((ix+1) + (    0)*nbX + num0);
            MeshVertex* v11 = m_Mesh->getVertex((ix+1) + (nbZ-1)*nbX + num0);
            m_Mesh->addTriangle(v01, v00, v11);
            m_Mesh->addTriangle(v11, v00, v10);
        }
    }

    // si repli en X et Z, rajouter un quad dans le coin
    if (foldX && foldZ) {
        // récupérer les 4 sommets concernés
        MeshVertex* v00 = m_Mesh->getVertex((    0) + (    0)*nbX + num0);
        MeshVertex* v01 = m_Mesh->getVertex((    0) + (nbZ-1)*nbX + num0);
        MeshVertex* v10 = m_Mesh->getVertex((nbX-1) + (    0)*nbX + num0);
        MeshVertex* v11 = m_Mesh->getVertex((nbX-1) + (nbZ-1)*nbX + num0);
        m_Mesh->addTriangle(v00, v01, v10);
        m_Mesh->addTriangle(v10, v01, v11);
    }

    // retourner le numéro du premier point
    return num0;
}


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
int MeshModuleTopology::addRevolutionSurface(int spokes_count, int segments_count, std::string names)
{
    // nombre de sommets existants déjà dans le maillage
    int num0 = m_Mesh->getVertexCount();

    // créer le vertex du centre
    std::string name = names.replace(names.find("%d"), 2, "C");
    MeshVertex* center = m_Mesh->addVertex(name);
    center->setCoord(vec3::fromValues(0,0,0));

    // définir les vertices des cercles autour du centre
    for (int ir=0; ir<spokes_count; ir++) {
        float angle = (float)ir/spokes_count*M_PI*2.0;
        for (int is=0; is<segments_count; is++) {
            // créer le nom du sommet en replaçant le %d par is
            std::string name = replace(replace(names, "%d", ir), "%d", is);
            // créer un sommet dans le maillage
            MeshVertex* vertex = m_Mesh->addVertex(name);
            // coordonnées par défaut du sommet
            vertex->setCoord(vec3::fromValues((is+1.0)*cos(angle), 0, (is+1.0)*sin(angle)));
        }
    }

    // rajouter des triangles afin de pouvoir créer des rubans rapidement
    for (int ir=0; ir<spokes_count; ir++) {
        // rajouter les triangles juste autour du centre
        if (segments_count > 0) {
            MeshVertex* vertex0 = m_Mesh->getVertex(((ir  )%spokes_count)*segments_count + num0+1);
            MeshVertex* vertex1 = m_Mesh->getVertex(((ir+1)%spokes_count)*segments_count + num0+1);
            m_Mesh->addTriangle(center, vertex0, vertex1);
        }

        // rajouter les quads autour des triangles du centre
        for (int is=0; is<segments_count-1; is++) {
            // récupérer les 4 sommets concernés
            MeshVertex* v00 = m_Mesh->getVertex(((ir  )%spokes_count)*segments_count + (is  ) + num0+1);
            MeshVertex* v10 = m_Mesh->getVertex(((ir+1)%spokes_count)*segments_count + (is  ) + num0+1);
            MeshVertex* v01 = m_Mesh->getVertex(((ir  )%spokes_count)*segments_count + (is+1) + num0+1);
            MeshVertex* v11 = m_Mesh->getVertex(((ir+1)%spokes_count)*segments_count + (is+1) + num0+1);
            // ajouter un quad (ordre adéquat pour créer des rubans rapidement)
            m_Mesh->addQuad(v00, v01, v11, v10);
        }
    }

    // retourner le numéro du premier point
    return num0;
}
