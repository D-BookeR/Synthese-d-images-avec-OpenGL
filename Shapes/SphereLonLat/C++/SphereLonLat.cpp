// Définition de la classe SphereLonLat

#include <GL/glew.h>
#include <GL/gl.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <math.h>

#include <utils.h>
#include <MeshModuleTopology.h>

#include <SphereLonLat.h>



/**
 * Cette fonction définit la classe SphereLonLat.
 * @param nbLon : nombre de divisions en longitude
 * @param nbLat : nombre de divisions en latitude
 */
SphereLonLat::SphereLonLat(int nbLon, int nbLat)
{
    // créer le maillage : une nappe rectangulaire
    m_Mesh = new Mesh("SphereLonLat");
    MeshModuleTopology topology(m_Mesh);
    int num0 = topology.addRectangularSurface(nbLon, nbLat, "sphere %d-%d", true, false);

    // aller du pôle sud au pôle nord, pour définir les sommets
    for (int ilat=0; ilat<nbLat; ilat++) {
        // faire le tour du globe
        for (int ilon=0; ilon<nbLon; ilon++) {
            // récupérer le sommet
            MeshVertex* vertex = m_Mesh->getVertex(ilon + ilat*nbLon + num0);
            // calculer ses coordonnées
            float a = (float)ilon / nbLon;
            float b = (float)ilat / (nbLat-1);
            vertex->setCoord(this->getCoord(a, b));
            vertex->setNormal(this->getCoord(a, b));
        }
    }
}


/**
 * Cette méthode retourne les coordonnées 3D d'un point identifié par (a,b)
 * @param a : longitude variant entre 0 et 1 en tournant autour du centre
 * @param b : latitude variant entre 0 (pôle sud) et 1 (pôle nord)
 * @return vec3 contenant les coordonnées
 */
vec3 SphereLonLat::getCoord(float a, float b)
{
    // longitude : 0.5 - a pour décaler 0° au niveau du méridien de greenwich et tourner dans le bon sens
    float lon = (0.5 - a) * 2.0 * M_PI;
    float coslon = cos(lon);
    float sinlon = sin(lon);

    // latitude
    float lat = (b - 0.5) * M_PI;
    float coslat = cos(lat);
    float sinlat = sin(lat);

    // conversion sphériques -> cartésiennes
    float x = sinlon * coslat;
    float y = sinlat;
    float z = coslon * coslat;
    return vec3::fromValues(x,y,z);
}


/**
 * Cette méthode supprime les ressources allouées
 */
SphereLonLat::~SphereLonLat()
{
    delete m_Mesh;
}
