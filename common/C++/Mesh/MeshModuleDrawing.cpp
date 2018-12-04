#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>
#include <algorithm>

#include <utils.h>
#include <Mesh.h>
#include <Material.h>
#include <MeshEdge.h>

#include <MeshModuleDrawing.h>



/**
 * initialise le module sur le maillage fourni
 * @param mesh : maillage concerné
 */
MeshModuleDrawing::MeshModuleDrawing(Mesh* mesh): MeshModule(mesh)
{
}


/**
 * Cette méthode crée tous les VBO pour dessiner le maillage à l'aide de triangles
 * @param material : celui qu'il faut employer pour dessiner les triangles
 * @param interleaved : true s'il faut entrelacer les données
 */
VBOset* MeshModuleDrawing::createVBOset(Material* material, bool interleaved)
{
    // créer les VBO demandés par le matériau
    VBOset* vboset = material->createVBOset();
    vboset->createAttributesVBO(m_Mesh, interleaved);

    // rassembler les indices des triangles
    std::vector<int> indexlist;
    for (MeshTriangle* triangle: m_Mesh->getTriangleList()) {
        for (int i=0; i<3; i++) {
            indexlist.push_back(triangle->getVertex(i)->getNumber());
        }
    }

    // créer le VBO des indices
    int size = vboset->createIndexedPrimitiveVBO(GL_TRIANGLES, indexlist);
    std::cout << m_Mesh->getName() << ": "<<size<<" indices with triangles" << std::endl;

    return vboset;
}


/**
 * Cette méthode crée tous les VBO pour dessiner le maillage à l'aide de triangle_strips
 * @param material : celui qu'il faut employer pour dessiner les triangles du ruban
 * @param interleaved : true s'il faut entrelacer les données
 */
VBOset* MeshModuleDrawing::createStripVBOset(Material* material, bool interleaved)
{
    // créer les VBO demandés par le matériau
    VBOset* vboset = material->createVBOset();
    vboset->createAttributesVBO(m_Mesh, interleaved);

    // créer une liste de rubans
    std::vector<TriangleStrip*> striplist;

    // parcourir tous les triangles du maillage
    for (MeshTriangle* triangle: m_Mesh->getTriangleList()) {
        // tenter d'insérer le triangle dans l'un des rubans
        bool ok = false;
        const int stripnumber = striplist.size();
        for (int ir=0; ir<stripnumber; ir++) {
            TriangleStrip* strip = striplist[ir];
            // le triangle peut-il être mis dans ce ruban ?
            if (strip->appendTriangle(triangle)) {
                // voir si l'un des rubans suivants peut être concaténé au courant
                for (int irs=ir+1; irs<stripnumber; irs++) {
                    TriangleStrip* nextstrip = striplist[irs];
                    if (strip->concat(nextstrip)) {
                        // supprimer le ruban suivant
                        list_remove(striplist, nextstrip);
                        delete nextstrip;
                        // ne pas continuer avec les autres rubans
                        break;
                    }
                }
                // c'est ok, on arrête le parcours des rubans
                ok = true;
                break;
            }
        }
        // si le triangle n'a pas pu être ajouté à un ruban, alors créer un autre ruban et le mettre en tête
        if (! ok) {
            striplist.insert(striplist.begin(), new TriangleStrip(triangle));
        }
    }

    // rassembler les indices des sommets des rubans, avec des codes de redémarrage entre rubans
    std::vector<int> indexlist;
    TriangleStrip* precstrip = nullptr;
    for (TriangleStrip* strip: striplist) {
        // rajouter un "primitive restart"
        if (precstrip != nullptr) {
            int prec = precstrip->getVertex(-1)->getNumber();
            if ((indexlist.size() % 2) == 1) indexlist.push_back(prec);
            indexlist.push_back(prec);
            int svt = strip->getVertex(0)->getNumber();
            indexlist.push_back(svt);
        }
        // rajouter les sommets du ruban
        for (MeshVertex* vertex: strip->getVertexList()) {
            indexlist.push_back(vertex->getNumber());
        }
        precstrip = strip;
    }

    // créer le VBO des indices
    int size = vboset->createIndexedPrimitiveVBO(GL_TRIANGLE_STRIP, indexlist);

    // message d'information
    int triangle_count = m_Mesh->getTriangleCount();
    std::cout << m_Mesh->getName() << ": "<<size<<" indices with "<<striplist.size()<<" strips, instead of "<<(triangle_count*3)<<std::endl;

    // libérer les rubans
    for (TriangleStrip* ruban: striplist) {
        delete ruban;
    }

    return vboset;
}


/**
 * Cette méthode crée tous les VBO pour dessiner les arêtes du maillage
 * @param material : celui qu'il faut employer pour dessiner les arêtes
 */
VBOset* MeshModuleDrawing::createEdgesVBOset(Material* material)
{
    // créer les VBO demandés par le matériau
    VBOset* vboset = material->createVBOset();
    vboset->createAttributesVBO(m_Mesh, true);

    // test sur le nombre d'indices
    //if (m_Mesh->getEdgeList().size() > 32767)
    //    throw std::invalid_argument("Too many edges to draw");

    // rassembler les indices des arêtes
    std::vector<int> indexlist;
    for (MeshEdge* edge: m_Mesh->getEdgeList()) {
        // rajouter les sommets de l'arête
        indexlist.push_back(edge->getVertex1()->getNumber());
        indexlist.push_back(edge->getVertex2()->getNumber());
    }

    // créer le VBO des indices
    vboset->createIndexedPrimitiveVBO(GL_LINES, indexlist);

    return vboset;
}


/**
 * Cette méthode crée tous les VBO pour dessiner les normales des facettes du maillage
 * @param material : celui qu'il faut employer pour dessiner les normales
 * @param length : nombre donnant la longueur des normales à afficher
 */
VBOset* MeshModuleDrawing::createFacesNormalsVBOset(Material* material, float length)
{
    // créer les VBO demandés par le matériau
    VBOset* vboset = material->createVBOset();

    // remplir les VBO avec les données demandées
    std::vector<GLfloat> data;

    // sommet et coordonnées temporaires
    MeshVertex* vertextmp = new MeshVertex(nullptr, "tmp");
    vec3 coords_center = vec3::create();
    vec3 coords_endpoint = vec3::create();

    // traiter chaque triangle
    for (MeshTriangle* triangle: m_Mesh->getTriangleList()) {

        // sommets du triangle courant
        MeshVertex* v0 = triangle->getVertex0();
        MeshVertex* v1 = triangle->getVertex1();
        MeshVertex* v2 = triangle->getVertex2();

        // normale du sommet temporaire
        vertextmp->setNormal(triangle->getNormal());

        // centre du triangle : ajouter ses données dans le VBO
        vec3::zero(coords_center);
        vec3::add(coords_center, v0->getCoord(), v1->getCoord());
        vec3::add(coords_center, coords_center,  v2->getCoord());
        vec3::scale(coords_center, coords_center, 1.0/3.0);
        vertextmp->setCoord(coords_center);

        // ajouter les valeurs des variables attributs (VBOvar)
        vboset->appendVertexComponents(vertextmp, data);

        // extremité = centre + normale * longueur
        vec3::zero(coords_endpoint);
        vec3::scale(coords_endpoint, triangle->getNormal(), length);
        vec3::add(coords_endpoint, coords_center, coords_endpoint);
        vertextmp->setCoord(coords_endpoint);

        // ajouter les valeurs des variables attributs (VBOvar)
        vboset->appendVertexComponents(vertextmp, data);
    }
    delete vertextmp;

    // créer le VBO entrelacé
    vboset->createInterleavedDataAttributesVBO(data);

    // pas de VBO d'indices car seulement des lignes à dessiner
    const int triangle_count = m_Mesh->getTriangleCount();
    vboset->createDirectPrimitiveVBO(GL_LINES, triangle_count * 2);

    return vboset;
}


/**
 * Cette méthode crée tous les VBO pour dessiner les normales des sommets du maillage
 * @param material : celui qu'il faut employer pour dessiner les normales
 * @param length : nombre donnant la longueur des normales à afficher
 */
VBOset* MeshModuleDrawing::createVertexNormalsVBOset(Material* material, float length)
{
    // créer les VBO demandés par le matériau
    VBOset* vboset = material->createVBOset();

    // remplir les VBO avec les données demandées
    std::vector<GLfloat> data;

    // sommet et coordonnées temporaires
    MeshVertex* vertextmp = new MeshVertex(nullptr, "tmp");
    vec3 coords_endpoint = vec3::create();

    // traiter chaque vertex
    for (MeshVertex* vertex: m_Mesh->getVertexList()) {

        // ajouter les valeurs des variables attributs (VBOvar)
        vboset->appendVertexComponents(vertex, data);

        // extremité = sommet + normale * longueur
        vec3::zero(coords_endpoint);
        vec3::scale(coords_endpoint, vertex->getNormal(), length);
        vec3::add(coords_endpoint, vertex->getCoord(), coords_endpoint);
        vertextmp->setCoord(coords_endpoint);

        // ajouter les valeurs des variables attributs (VBOvar)
        vboset->appendVertexComponents(vertextmp, data);
    }
    delete vertextmp;

    // créer le VBO entrelacé
    vboset->createInterleavedDataAttributesVBO(data);

    // pas de VBO d'indices car seulement des lignes à dessiner
    const int vertex_count = m_Mesh->getVertexCount();
    vboset->createDirectPrimitiveVBO(GL_LINES, vertex_count * 2);

    return vboset;
}



TriangleStrip::TriangleStrip(MeshTriangle* triangle)
{
    // copie de la liste des sommets du triangle
    m_VertexList.clear();
    m_VertexList.push_back(triangle->getVertex0());
    m_VertexList.push_back(triangle->getVertex1());
    m_VertexList.push_back(triangle->getVertex2());
}


/**
 * retourne l'un des sommets du ruban, désigné par son numéro
 * @param i = le numéro du sommet, si <0 alors par rapport à la fin du tableau
 * @return le ie sommet du ruban
 */
MeshVertex* TriangleStrip::getVertex(int i)
{
    if (i < 0) {
        return m_VertexList[m_VertexList.size()+i];
    } else {
        return m_VertexList[i];
    }
}


/**
 * Cette méthode tente d'ajouter le triangle au ruban
 * @param triangle à rajouter
 * @return true si le triangle a pu être mis dans le ruban, false sinon
 */
bool TriangleStrip::appendTriangle(MeshTriangle* triangle)
{
    // nombre de sommets du ruban
    int vertex_number = m_VertexList.size();

    // cas particulier : le ruban ne compte qu'un seul triangle
    if (vertex_number == 3) {
        // sommets du ruban
        MeshVertex* v0 = m_VertexList[0];
        MeshVertex* v1 = m_VertexList[1];
        MeshVertex* v2 = m_VertexList[2];
        // NB: les tests sont dans l'ordre le plus favorable si les triangles ont été créés par addNappeRectangulaire
        // le triangle contient-il l'arête C-B ?
        if (triangle->containsEdge(v2, v1)) {
            // le triangle se colle à l'arête B-C du ruban
            m_VertexList.clear();
            m_VertexList.push_back(v0);
            m_VertexList.push_back(v1);
            m_VertexList.push_back(v2);
            m_VertexList.push_back(triangle->getVertex(2));
            return true;
        }
        // le triangle contient-il l'arête B-A ?
        if (triangle->containsEdge(v1, v0)) {
            // le triangle se colle à l'arête A-B du ruban
            m_VertexList.clear();
            m_VertexList.push_back(v2);
            m_VertexList.push_back(v0);
            m_VertexList.push_back(v1);
            m_VertexList.push_back(triangle->getVertex(2));
            return true;
        }
        // le triangle contient-il l'arête A-C ?
        if (triangle->containsEdge(v0, v2)) {
            // le triangle se colle à l'arête C-A du ruban
            m_VertexList.clear();
            m_VertexList.push_back(v1);
            m_VertexList.push_back(v2);
            m_VertexList.push_back(v0);
            m_VertexList.push_back(triangle->getVertex(2));
            return true;
        }
        // aucun des trois côtés ne convient
        return false;
    }

    // si le nombre de sommets du ruban est pair, on peut le retourner si besoin
    if ((vertex_number % 2) == 0) {
        // test d'insertion en fin de ruban
        if (triangle->containsEdge(m_VertexList[vertex_number-2], m_VertexList[vertex_number-1])) {
            m_VertexList.push_back(triangle->getVertex(2));
            return true;
        }
        // test d'insertion en debut de ruban
        if (triangle->containsEdge(m_VertexList[1], m_VertexList[0])) {
            // retourner le ruban
            std::reverse(m_VertexList.begin(), m_VertexList.end());
            m_VertexList.push_back(triangle->getVertex(2));
            return true;
        }
    } else {
        // le nombre de sommets du ruban est impair, on ne peut insérer qu'en fin
        if (triangle->containsEdge(m_VertexList[vertex_number-1], m_VertexList[vertex_number-2])) {
            m_VertexList.push_back(triangle->getVertex(2));
            return true;
        }
    }

    // aucune solution n'est satisfaisante, le triangle ne peut pas être ajouté
    return false;
}


/**
 * Cette méthode tente d'ajouter un ruban de 3 sommets au ruban
 * @param other : autre ruban à rajouter
 * @return true si l'autre ruban a pu être mis dans le ruban, false sinon
 */
bool TriangleStrip::appendStrip3(TriangleStrip* other)
{
    // nombre de sommets du ruban
    int vertex_number = m_VertexList.size();

    // cas particulier : le ruban ne compte qu'un seul triangle
    if (vertex_number == 3) {
        // sommets du ruban
        MeshVertex* v0 = m_VertexList[0];
        MeshVertex* v1 = m_VertexList[1];
        MeshVertex* v2 = m_VertexList[2];
        // NB: les tests sont dans l'ordre le plus favorable si les triangles ont été créés par addNappeRectangulaire
        // l'autre ruban contient-il l'arête C-B ?
        if (other->containsEdge(v2, v1)) {
            // l'autre ruban se colle à l'arête B-C du ruban
            m_VertexList.clear();
            m_VertexList.push_back(v0);
            m_VertexList.push_back(v1);
            m_VertexList.push_back(v2);
            m_VertexList.push_back(other->m_VertexList[2]);
            return true;
        }
        // l'autre ruban contient-il l'arête B-A ?
        if (other->containsEdge(v1, v0)) {
            // l'autre ruban se colle à l'arête A-B du ruban
            m_VertexList.clear();
            m_VertexList.push_back(v2);
            m_VertexList.push_back(v0);
            m_VertexList.push_back(v1);
            m_VertexList.push_back(other->m_VertexList[2]);
            return true;
        }
        // l'autre ruban contient-il l'arête A-C ?
        if (other->containsEdge(v0, v2)) {
            // l'autre ruban se colle à l'arête C-A du ruban
            m_VertexList.clear();
            m_VertexList.push_back(v1);
            m_VertexList.push_back(v2);
            m_VertexList.push_back(v0);
            m_VertexList.push_back(other->m_VertexList[2]);
            return true;
        }
        // aucun des trois côtés ne convient
        return false;
    }

    // si le nombre de sommets du ruban est pair, on peut le retourner si besoin
    if ((vertex_number % 2) == 0) {
        // test d'insertion en fin de ruban
        if (other->containsEdge(m_VertexList[vertex_number-2], m_VertexList[vertex_number-1])) {
            m_VertexList.push_back(other->m_VertexList[2]);
            return true;
        }
        // test d'insertion en debut de ruban
        if (other->containsEdge(m_VertexList[1], m_VertexList[0])) {
            // retourner le ruban
            std::reverse(m_VertexList.begin(), m_VertexList.end());
            m_VertexList.push_back(other->m_VertexList[2]);
            return true;
        }
    } else {
        // le nombre de sommets du ruban est impair, on ne peut insérer qu'en fin
        if (other->containsEdge(m_VertexList[vertex_number-1], m_VertexList[vertex_number-2])) {
            m_VertexList.push_back(other->m_VertexList[2]);
            return true;
        }
    }

    // aucune solution n'est satisfaisante, le triangle ne peut pas être ajouté
    return false;
}


/**
 * retourne true si le ruban contient 3 sommets, dont v1 suivi de v2
 * Si true, alors au retour, v1 et v2 sont dans les deux premières places du ruban
 * @param vertex0 : premier sommet de l'arête (elle est orientée de 1 à 2)
 * @param vertex1 : second sommet
 * @return true si oui, false si l'arête n'est pas dans le triangle
 */
bool TriangleStrip::containsEdge(MeshVertex* vertex0, MeshVertex* vertex1)
{
    if (m_VertexList.size() != 3) return false;

    if (m_VertexList[0] == vertex0 && m_VertexList[1] == vertex1) return true;

    if (m_VertexList[1] == vertex0 && m_VertexList[2] == vertex1) {
        // mettre m_VertexList[1] en premier
        MeshVertex* tmp = m_VertexList[1];
        m_VertexList[1] = m_VertexList[2];
        m_VertexList[2] = m_VertexList[0];
        m_VertexList[0] = tmp;
        return true;
    }

    if (m_VertexList[2] == vertex0 && m_VertexList[0] == vertex1) {
        // mettre m_VertexList[2] en premier
        MeshVertex* tmp = m_VertexList[1];
        m_VertexList[1] = m_VertexList[0];
        m_VertexList[0] = m_VertexList[2];
        m_VertexList[2] = tmp;
        return true;
    }

    return false;
}


/**
 * Cette méthode tente d'ajouter le ruban fourni au ruban this
 * @param strip : ruban à rajouter
 * @return true si le ruban a pu être concaténé à this, false sinon
 */
bool TriangleStrip::concat(TriangleStrip* strip)
{
    // nombre de sommets des rubans
    const int thisvertexnumber  = m_VertexList.size();
    const int stripvertexnumber = strip->m_VertexList.size();

    // cas particulier d'un triangle, appeler l'autre méthode
    if (stripvertexnumber == 3) return appendStrip3(strip);

    // si this a un nombre de sommets pair, alors tenter
    if ((thisvertexnumber % 2) == 0) {
        // il faut que la dernière arête de this = la première de ruban
        if (m_VertexList[thisvertexnumber-2] == strip->m_VertexList[0] &&
            m_VertexList[thisvertexnumber-1] == strip->m_VertexList[1]) {
            m_VertexList.insert(m_VertexList.end(), strip->m_VertexList.begin()+2, strip->m_VertexList.end());
            return true;
        }
        // si l'autre ruban a un nombre pair de sommets, on peut le retourner
        if ((stripvertexnumber % 2) == 0) {
            // tenter de mettre l'autre ruban à l'envers en fin de this
            if (m_VertexList[thisvertexnumber-2] == strip->m_VertexList[stripvertexnumber-1] &&
                m_VertexList[thisvertexnumber-1] == strip->m_VertexList[stripvertexnumber-2]) {
                std::reverse(strip->m_VertexList.begin(), strip->m_VertexList.end());
                m_VertexList.insert(m_VertexList.end(), strip->m_VertexList.begin()+2, strip->m_VertexList.end());
                return true;
            }
            // tenter de metre l'autre ruban au début de this
            if (m_VertexList[0] == strip->m_VertexList[stripvertexnumber-2] &&
                m_VertexList[1] == strip->m_VertexList[stripvertexnumber-1]) {
                m_VertexList.erase(m_VertexList.begin(), m_VertexList.begin()+2);
                m_VertexList.insert(m_VertexList.begin(), strip->m_VertexList.begin(), strip->m_VertexList.end());
                return true;
            }
        }
    }

    // si ruban a un nombre de sommets pair
    if ((stripvertexnumber % 2) == 0) {
        // tenter de mettre this à la fin de ruban
        if (m_VertexList[0] == strip->m_VertexList[stripvertexnumber-2] &&
            m_VertexList[1] == strip->m_VertexList[stripvertexnumber-1]) {
            m_VertexList.erase(m_VertexList.begin(), m_VertexList.begin()+2);
            m_VertexList.insert(m_VertexList.begin(), strip->m_VertexList.begin(), strip->m_VertexList.end());
            return true;
        }
        // tenter de mettre this en début de ruban (en inversant ruban)
        if (m_VertexList[0] == strip->m_VertexList[1] &&
            m_VertexList[1] == strip->m_VertexList[0]) {
            std::reverse(strip->m_VertexList.begin(), strip->m_VertexList.end());
            m_VertexList.erase(m_VertexList.begin(), m_VertexList.begin()+2);
            m_VertexList.insert(m_VertexList.begin(), strip->m_VertexList.begin(), strip->m_VertexList.end());
            return true;
        }
    }

    // aucune solution n'est satisfaisante, le ruban ne peut pas être ajouté
    return false;
}
