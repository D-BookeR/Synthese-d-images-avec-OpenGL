// Définition de la classe Seashell

#include <GL/glew.h>
#include <GL/gl.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <math.h>

#include <utils.h>
#include <MeshModuleTopology.h>

#include <Seashell.h>



/**
 * constructeur
 * @param turns_count : nombre de tours
 * @param steps : nombre de divisions en longitude et en latitude
 */
Seashell::Seashell(int turns_count, int steps)
{
    // paramètres
    float r = 0.35;

    // créer le maillage : une nappe hexagonale partiellement refermée sur elle-même
    m_Mesh = new Mesh("Seashell");
    MeshModuleTopology topology(m_Mesh);
    int num0 = topology.addHexagonalSurface(steps*turns_count, steps, "seashell %d-%d", false, true);

    // parcourir les sommets pour définir les coordonnées
    for (int ilat=0; ilat<steps; ilat++) {
        for (int ilon=0; ilon<steps*turns_count; ilon++) {
            // récupérer le sommet concerné
            int num = ilon + ilat*steps*turns_count + num0;
            MeshVertex* vertex = m_Mesh->getVertex(num);
            // définir les coordonnées 3D du point
            float a = (ilon - 0.5*(ilat%2)) / steps;
            float b = (float)ilat / steps;
            vertex->setCoord(this->getCoord(r, a, b));
            // définir la couleur du point
            float c = (ilon - 0.5*(ilat%2)) / (steps*turns_count);
            vertex->setColor(Utils::hsv2rgb(vec3::fromValues(0.9-c*0.2, 0.8, 0.7)));
        }
    }

    // calculer les normales
    m_Mesh->computeNormals();
}


/**
 * Cette méthode retourne les coordonnées 3D d'un point identifié par (r,a,b)
 * @param r : rayon du tube, le grand rayon est 1.0
 * @param a : longitude variant entre 0 et 1 en tournant autour du centre
 * @param b : latitude variant entre 0 (pôle sud) et 1 (pôle nord)
 * @return vec3 contenant les coordonnées
 */
vec3 Seashell::getCoord(float r, float a, float b)
{
    // longitude  attention, on tourne dans le sens ouest->est
    float lon = -a * 2.0 * M_PI;
    float coslon = cos(lon);
    float sinlon = sin(lon);

    // latitude
    float lat = b * 2.0 * M_PI;
    float coslat = cos(lat);
    float sinlat = sin(lat);

    // évolution du petit rayon selon a
    r = r * a * 0.5;

    // conversion toriques -> cartésiennes
    float R = r * (1.0 + coslat);
    float x = R * sinlon;
    float y = r * sinlat + a*a*0.2 - 2.0;
    float z = R * coslon;
    return vec3::fromValues(x,y,z);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Seashell::~Seashell()
{
    delete m_Mesh;
}
