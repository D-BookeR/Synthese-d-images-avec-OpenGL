// Définition de la classe Wave


#include <GL/glew.h>
#include <GL/gl.h>
#include <math.h>

#include <utils.h>
#include <MeshModuleTopology.h>

#include <Wave.h>


/**
 * Cette fonction définit la classe Wave.
 * @param border : contour à dessiner
 * @param spokes_count : nombre de secteurs
 * @param segments_count : nombre de cercles concentriques pour décrire la fonction
 */
Wave::Wave(std::vector<vec2> border, int spokes_count, int segments_count)
{
    this->border = border;

    // créer le maillage : une grille plane polaire
    m_Mesh = new Mesh("Wave");
    MeshModuleTopology topology(m_Mesh);
    int num0 = topology.addRectangularSurface(spokes_count, segments_count, "grille %d-%d", true, false);

    // parcourir les sommets pour définir leurs coordonnées
    for (int is=0; is<spokes_count; is++) {
        float a = 1.0 - (float)is / spokes_count;
        for (int ip=0; ip<segments_count; ip++) {
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
vec3 Wave::getCoord(float a, int ip)
{
    // point du contour : (r, y)
    vec2 point = border[ip];
    float r = point[0];
    float y = point[1];

    // angle de ce point
    float angle = a * 2.0 * M_PI;

    // conversion polaires -> cartésiennes
    float x = r * sin(angle);
    float z = r * cos(angle);
    return vec3::fromValues(x,y,z);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Wave::~Wave()
{
    delete m_Mesh;
}
