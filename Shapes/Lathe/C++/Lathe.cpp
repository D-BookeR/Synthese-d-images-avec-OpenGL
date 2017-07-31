// Définition de la classe Lathe


#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>

#include <utils.h>
#include <MeshModuleTopology.h>

#include <Lathe.h>


/**
 * Cette fonction définit la classe Lathe.
 * @param border : tableau de vec2 représentant la ligne qu'il faut faire tourner
 * @param spokes_count : nombre de secteurs
 */
Lathe::Lathe(std::vector<vec2> border, int spokes_count)
{
    // initialiser les variables membres
    this->border = border;
    int points_count = border.size();

    // créer le maillage : une grille rectangulaire
    m_Mesh = new Mesh("Lathe");
    MeshModuleTopology topology(m_Mesh);
    int num0 = topology.addHexagonalSurface(spokes_count, points_count, "tour %d-%d", true, false);

    // parcourir les sommets pour définir leurs coordonnées
    for (int is=0; is<spokes_count; is++) {
        for (int ip=0; ip<points_count; ip++) {
            // angle 0..1 autour de l'axe
            float a = (is - 0.5*(ip%2)) / spokes_count;
            // numéro du vertex
            int num = is + ip*spokes_count + num0;
            // récupérer le sommet concerné
            MeshVertex* vertex = m_Mesh->getVertex(num);
            // définir les coordonnées 3D du point
            vertex->setCoord(this->getCoord(a, ip));
        }
    }

    // calculer les normales
    m_Mesh->computeNormals();
}


/**
 * Cette méthode retourne les coordonnées 3D d'un point identifié par (a,ip)
 * @param a : paramètre variant entre 0 et 1 en tournant autour de l'axe Y
 * @param ip : numéro du point sur le contour
 * @return vec3 contenant les coordonnées
 */
vec3 Lathe::getCoord(float a, int ip)
{
    // point du contour : (r, y)
    vec2 point = border[ip];
    float r = point[0];
    float y = point[1];

    // angle de ce point, attention, ouest->est
    float angle = -a * 2.0 * M_PI;

    // conversion polaires -> cartésiennes
    float x = r * sin(angle);
    float z = r * cos(angle);
    return vec3::fromValues(x,y,z);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Lathe::~Lathe()
{
    delete m_Mesh;
}
