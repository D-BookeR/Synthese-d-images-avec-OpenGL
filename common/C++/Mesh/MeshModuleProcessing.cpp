/**
 * Ce module rajoute des fonctions de transformations géométriques
 * NB: penser à appeler creerVBO() avant et après chaque transformation
 */

#include <iostream>
#include <sstream>
#include <algorithm>
#include <map>
#include <vector>

#include <MeshModuleProcessing.h>


/**
 * initialise le module sur le maillage fourni
 * @param mesh : maillage concerné
 */
MeshModuleProcessing::MeshModuleProcessing(Mesh* mesh): MeshModuleUtils(mesh)
{
}


/**
 * Cette méthode fait une homothétie du triangle autour de son centre
 * @param triangle auquel on applique la transformation
 * @param scale : rapport d'agrandissement (si > 1.0) ou de réduction (si < 1.0)
 * @return triangle : le triangle auquel on applique cette méthode
 */
MeshTriangle* MeshModuleProcessing::homothety(MeshTriangle* triangle, float scale)
{
    // traiter tous les sommets
    for (int i=0; i<3; i++) {
        MeshVertex* vertex = triangle->getVertex(i);
        vec3& coords = vertex->getCoord();
        vec3::subtract(coords, coords, triangle->getCenter());
        vec3::scale(   coords, coords, scale);
        vec3::add(     coords, coords, triangle->getCenter());
    }
    return triangle;
}


/**
 * Cette méthode casse l'angle au niveau du sommet indiqué. Elle
 * enfonce le sommet de la distance selon la direction fournie en paramètre.
 * @param vertex : sommet qu'il faut tronquer
 * @param distance : distance où vont aller les points intermédiaires
 * @param direction : vec3 indiquant l'axe d'enfoncement des points
 * @return liste des sommets du contour résultant de la découpe
 */
std::vector<MeshVertex*> MeshModuleProcessing::bevelVertex(MeshVertex* vertex, float distance, vec3 direction)
{
    // normaliser la direction
    vec3::normalize(direction, direction);

    // liste des sommets formant le contour
    std::vector<MeshVertex*> border;

    // couper en deux chaque triangle entourant le sommet
    for (MeshTriangle* triangle: vertex->getTrianglesOrderedAround()) {
        // faire tourner la liste des sommets du triangle pour mettre this en premier
        if (!triangle->cycleVertexFirst(vertex)) {
            std::cerr << "Cycle error in mesh" << std::endl;
            continue;
        }

        // découper de chaque côté
        std::vector<MeshVertex*> quad;
        quad.push_back(triangle->getVertex1());
        quad.push_back(triangle->getVertex2());
        for (int is=1; is<=2; is++) {
            MeshVertex* other = triangle->getVertex(is);
            std::string midname = getMidName(vertex, other);
            MeshVertex* mid = m_Mesh->getVertexByName(midname);
            if (mid == nullptr) {
                mid = m_Mesh->addVertex(midname);
                // calculer la distance relative
                vec3 edge = vec3::create();
                vec3::subtract(edge, vertex->getCoord(), other->getCoord());
                float k = distance / vec3::dot(direction, edge);
                // positionner le point intermédiaire à cette distance
                mid->lerp(vertex, other, k);
                // ajouter ce point au contour
                border.push_back(mid);
            }
            // mémoriser le point intermédiaire
            quad.push_back(mid);
        }

        // supprimer le triangle
        delete triangle;

        // créer un quad entre s1, s2, m1 et m2 pour remplacer le triangle
        m_Mesh->addQuad(quad[0], quad[1], quad[3], quad[2]);

        // créer un triangle entre le sommet, m1 et m2
        m_Mesh->addTriangle(vertex, quad[2], quad[3]);
    }

    /* deux choix possibles : garder l'ancien sommet et ses anciens triangles, ou refaire un polygone */

    #if 0
    /* choix 1 : on garde l'ancien sommet mais on le met dans le plan du contour */

    // déplacer le sommet dans le plan de ceux du contour
    vec3 translation = vec3::clone(direction);
    vec3::scale(translation, translation, -distance);
    vec3::add(vertex->getCoord(), vertex->getCoord(), translation);

    #else
    /* choix 2 : on supprime l'ancien sommet et on recrée un polygone dans le contour */

    // supprimer le sommet
    delete vertex;

    // créer un polygone pour remplir le trou
    m_Mesh->addPolygon(border, direction);

    // recalculer les normales
    m_Mesh->computeNormals();

    #endif
    return border;
}


/**
 * Cette méthode clone les sommets spécifiés dans le contour afin
 * de la rendre dure (séparer deux groupes de lissage de part et d'autre
 * NB: ce sont les sommets des triangles "de gauche" qui sont clonés
 * @param border : liste de sommets (l'ordre est important), il en faut au moins 2
 */
void MeshModuleProcessing::splitBorder(std::vector<MeshVertex*> border)
{
    // si la liste est trop courte, sortir sans rien faire
    int vertex_count = border.size();
    if (vertex_count < 2) return;

    // liste des triangles dont les sommets vont être clonés
    std::vector<MeshHalfEdge*> halfedges = getHalfEdgesAlongBorder(border);
    std::vector<MeshTriangle*> triangles = getTrianglesInsideBorder(halfedges);

    // liste des clones, il y en a autant que de sommets dans l'arête
    std::vector<MeshVertex*> clones(vertex_count, nullptr);

    // remplacer les sommets de l'arête dans tous les triangles
    for (MeshTriangle* triangle: triangles) {
        // remplacer tous les sommets qui sont dans l'arête par leurs clones
        for (int is=0; is<vertex_count; is++) {
            MeshVertex* vertex = border[is];
            MeshVertex* clone  = clones[is];
            // le triangle contient-il ce sommet ?
            if (triangle->containsVertex(vertex)) {
                // cloner le sommet, sauf s'il l'est déjà
                if (clone == nullptr) {
                    clone = vertex->clone("clone");
                    clones[is] = clone;
                }
                // remplacer le sommet par son clone
                triangle->replaceVertex(vertex, clone);
            }
        }
    }
}


/**
 * Cette méthode fait une extrusion des triangles contenus dans le contour
 * et de la distance indiquée
 * @param border : liste de sommets, délimitent les triangles concernés qui sont à gauche du contour
 * @param distance : longueur de l'extrusion
 * @return liste des clones des sommets de tous les triangles inclus dans le contour
 */
std::vector<MeshVertex*> MeshModuleProcessing::extrudePolygon(std::vector<MeshVertex*> border, float distance)
{
    // liste des triangles dont les sommets vont être clonés
    std::vector<MeshHalfEdge*> halfedges = getHalfEdgesAlongBorder(border);
    std::vector<MeshTriangle*> triangles = getTrianglesInsideBorder(halfedges);
    if (triangles.size() == 0) return std::vector<MeshVertex*>();
    const int vertex_count = border.size();

    // direction de l'extrusion : moyenne normalisée des normales des triangles
    vec3 direction = averageNormals(triangles);

    // définir le décalage à partir de la direction et la distance
    vec3 offset = vec3::create();
    vec3::scale(offset, direction, distance);

    // liste des clones, il y en a autant que de sommets dans le contour
    std::vector<MeshVertex*> clones;
    for (MeshVertex* vertex: border) {
        MeshVertex* clone = vertex->clone("clone");
        clones.push_back(clone);
    }

    // remplacer les sommets par leurs clones dans tous les triangles
    for (MeshTriangle* triangle: triangles) {
        // remplacer tous les sommets par leurs clones
        for (int iv=0; iv<vertex_count; iv++) {
            // sommet courant et son clone
            MeshVertex* vertex = border[iv];
            MeshVertex* clone = clones[iv];
            // remplacer le sommet par son clone (si sommet est dans ce triangle)
            triangle->replaceVertex(vertex, clone);
        }
    }

    // construire la liste des sommets contenus dans ces triangles
    std::set<MeshVertex*> vertex_list = getVerticesFromTriangles(triangles);

    // décaler les sommets
    for (MeshVertex* vertex: vertex_list) {
        // décaler le sommet
        vec3::add(vertex->getCoord(), vertex->getCoord(), offset);
    }

    // construire des quads avec chaque couple de sommets du contour
    for (int ic=0; ic<vertex_count; ic++) {
        // sommet courant et son clone
        MeshVertex* A = border[ic];
        MeshVertex* A1 = clones[ic];
        // sommet suivant et son clone
        MeshVertex* B = border[(ic+1)%vertex_count];
        MeshVertex* B1 = clones[(ic+1)%vertex_count];
        // construire un quad sur ces 4 sommets
        m_Mesh->addQuad(A, B, B1, A1);
    }

    return clones;
}


/**
 * Cette méthode fait une extrusion du triangle indiqué, le long de sa normale
 * et de la distance indiquée
 * @param triangle : celui qu'il faut extruder
 * @param distance : longueur de l'extrusion
 * @return nouveau triangle résultat de l'extrusion
 */
MeshTriangle* MeshModuleProcessing::extrudeTriangle(MeshTriangle* triangle, float distance)
{
    // sommets du triangle existant
    MeshVertex* A = triangle->getVertex0();
    MeshVertex* B = triangle->getVertex1();
    MeshVertex* C = triangle->getVertex2();

    // définir le décalage à partir de la normale et la distance
    vec3 offset = vec3::create();
    vec3::scale(offset, triangle->getNormal(), distance);

    // décaler les sommets dans la direction de la normale
    MeshVertex* A1 = A->clone("clone");
    vec3::add(A1->getCoord(), A->getCoord(), offset);
    MeshVertex* B1 = B->clone("clone");
    vec3::add(B1->getCoord(), B->getCoord(), offset);
    MeshVertex* C1 = C->clone("clone");
    vec3::add(C1->getCoord(), C->getCoord(), offset);

    // remplacer les sommets par leurs clones dans le triangle (A,B,C)
    triangle->replaceVertex(A, A1);
    triangle->replaceVertex(B, B1);
    triangle->replaceVertex(C, C1);

    // créer un quad sur chacun des trois flancs
    m_Mesh->addQuad(A, B, B1, A1);
    m_Mesh->addQuad(B, C, C1, B1);
    m_Mesh->addQuad(C, A, A1, C1);

    // retourner le triangle modifié
    return triangle;
}


/**
 * Cette méthode subdivise le triangle en sous-triangles
 * @param triangle auquel on applique la transformation
 * @param steps : fournir un entier 1..raisonnable
 * @param smooth : déplace ou non les points midpoints vers l'intérieur ou l'extérieur
 * @return liste de triangles qui doivent remplacer celui-ci
 */
std::vector<MeshTriangle*> MeshModuleProcessing::subdivide(MeshTriangle* triangle, int steps, float smooth)
{
    std::vector<MeshTriangle*> result;

    // fin de la récursivité ?
    if (steps <= 0) {
        // ajouter le triangle au résultat et le retourner
        std::vector<MeshTriangle*> triangles;
        triangles.push_back(triangle);
        return triangles;
    }
    steps -= 1;

    // noter les trois sommets du triangle
    MeshVertex* vertex_list[3] = {
            triangle->getVertex0(),
            triangle->getVertex1(),
            triangle->getVertex2()
    };

    // sommets intermédiaires (dans l'ordre des sommets)
    std::vector<MeshVertex*> midpoints;

    // traiter chaque arête
    for (int is=0; is<3; is++) {
        // extrémités de l'arête
        MeshVertex* s0 = triangle->getVertex(is);
        MeshVertex* s1 = triangle->getVertex((is+1)%3);
        // milieu entre s0 et s1
        std::string name01 = getMidName(s0, s1);
        MeshVertex* m01 = m_Mesh->getVertexByName(name01);
        if (m01 == nullptr) {
            m01 = new MeshVertex(m_Mesh, name01);
            // lissage ?
            if (smooth > 0.0) {
                // vecteur s0s1
                vec3 s0s1 = vec3::create();
                vec3::subtract(s0s1, s1->getCoord(), s0->getCoord());
                vec3::scale(s0s1, s0s1, smooth);
                // normale, binormale et tangente en s0
                vec3 n0 = s0->getNormal();
                vec3 b0 = vec3::create();
                vec3::cross(b0, s0s1, n0);
                vec3 t0 = vec3::create();
                vec3::cross(t0, n0, b0);
                // normale, binormale et tangente en s1
                vec3 n1 = s1->getNormal();
                vec3 b1 = vec3::create();
                vec3::cross(b1, s0s1, n1);
                vec3 t1 = vec3::create();
                vec3::cross(t1, n1, b1);
                // interpolation cubique spline de Hermite
                m01->hermite(s0, t0, s1, t1, 0.5);
            } else {
                // simple interpolation linéaire
                m01->lerp(s0, s1, 0.5);
            }
        }
        midpoints.push_back(m01);
    }

    // supprimer le triangle
    delete triangle;

    // créer 3 triangles à l'intérieur de this (sens trigo)
    for (int is=0; is<3; is++) {
        MeshVertex* s0 = vertex_list[is];
        MeshVertex* m01 = midpoints[is];
        MeshVertex* m20 = midpoints[(is+2)%3];
        MeshTriangle* triangle_tmp = new MeshTriangle(m_Mesh, s0, m01, m20);
        std::vector<MeshTriangle*> subdiv = subdivide(triangle_tmp, steps, smooth);
        result.insert(result.end(), subdiv.begin(), subdiv.end());
    }

    // triangle central
    MeshTriangle* triangle_tmp = new MeshTriangle(m_Mesh, midpoints[0], midpoints[1], midpoints[2]);
    std::vector<MeshTriangle*> subdiv = subdivide(triangle_tmp, steps, smooth);
    result.insert(result.end(), subdiv.begin(), subdiv.end());

    return result;
}


/**
 * Cette méthode subdivise les triangles indiqués
 * @param triangles : liste des triangles concernés par la subdivision
 * @param steps : fournir un entier 1..raisonnable
 * @param smooth : déplace ou non les points milieux vers l'intérieur ou l'extérieur
 * @return liste de triangles qui doivent remplacer celui-ci
 */
std::vector<MeshTriangle*> MeshModuleProcessing::subdivideAll(std::vector<MeshTriangle*> triangles, int steps, float smooth)
{
    // mémoriser les triangles qui sont subdivisés
    std::vector<MeshTriangle*> result;

    // liste des triangles dont les sommets vont être clonés
    const int nbtriangles = triangles.size();
    if (nbtriangles == 0) return result;

    // faut-il faire quelque chose ?
    if (steps <= 0) return triangles;

    // subdiviser tous les triangles
    for (MeshTriangle* triangle: triangles) {
        std::vector<MeshTriangle*> subdiv = subdivide(triangle, steps, smooth);
        result.insert(result.end(), subdiv.begin(), subdiv.end());
    }
    return result;
}


/**
 * Cette méthode applique une matrice à tous les sommets du maillage
 * @param matrix : la transformation qu'il faut appliquer sur les sommets
 */
void MeshModuleProcessing::transform(mat4 matrix)
{
    for (MeshVertex* vertex: m_Mesh->getVertexList()) {
        vec3::transformMat4(vertex->getCoord(), vertex->getCoord(), matrix);
    }
}
