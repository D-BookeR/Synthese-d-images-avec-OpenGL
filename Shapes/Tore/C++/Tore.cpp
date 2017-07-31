// Définition de la classe Tore

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <math.h>

#include <utils.h>
#include <MeshModuleTopology.h>

#include <Tore.h>



/**
 * Cette fonction définit la classe Tore.
 * @param nblon : nombre de divisions en longitude
 * @param nblat : nombre de divisions en latitude
 * @param r : rayon du tube, le grand rayon est 1.0
 */
Tore::Tore(int nblon, int nblat, float r)
{
    // créer le maillage : une grille rectangulaire refermée sur elle-même
    m_Mesh = new Mesh("Tore");
    MeshModuleTopology topology(m_Mesh);
    int num0 = topology.addRectangularSurface(nblon, nblat, "tore %d-%d", true, true);

    // parcourir les sommets pour définir les coordonnées
    for (int ilat=0; ilat<nblat; ilat++) {
        for (int ilon=0; ilon<nblon; ilon++) {
            // récupérer le sommet concerné
            int num = ilon + ilat*nblon + num0;
            MeshVertex* vertex = m_Mesh->getVertex(num);
            // définir les coordonnées 3D du point
            float a = (float)ilon / nblon;
            float b = (float)ilat / nblat;
            vertex->setCoord(this->getCoord(r, a, b));
            vertex->setNormal(this->getNormal(r, a, b));
        }
    }
}


/**
 * Cette méthode retourne les coordonnées 3D d'un point identifié par (a,b)
 * @param r : rayon du tube, le grand rayon est 1.0
 * @param a : longitude variant entre 0 et 1 en tournant autour du centre
 * @param b : latitude variant entre 0 (pôle sud) et 1 (pôle nord)
 * @return vec3 contenant les coordonnées
 */
vec3 Tore::getCoord(float r, float a, float b)
{
    // longitude  attention, on tourne dans le sens ouest->est
    float lon = -a * 2.0 * M_PI;
    float coslon = cos(lon);
    float sinlon = sin(lon);

    // latitude
    float lat = b * 2.0 * M_PI;
    float coslat = cos(lat);
    float sinlat = sin(lat);

    // conversion toriques -> cartésiennes
    float R = 1.0 + r * coslat;
    float x = R * sinlon;
    float y = r * sinlat;
    float z = R * coslon;
    return vec3::fromValues(x,y,z);
}


/**
 * Cette méthode retourne la normale 3D au point identifié par (a,b)
 * @param r : rayon du tube, le grand rayon est 1.0
 * @param a : longitude variant entre 0 et 1 en tournant autour du centre
 * @param b : latitude variant entre 0 (pôle sud) et 1 (pôle nord)
 * @return vec3 contenant les coordonnées du vecteur normal
 */
vec3 Tore::getNormal(float r, float a, float b)
{
    // longitude  attention, on tourne dans le sens ouest->est
    float lon = -a * 2.0 * M_PI;
    float coslon = cos(lon);
    float sinlon = sin(lon);

    // latitude
    float lat = b * 2.0 * M_PI;
    float coslat = cos(lat);
    float sinlat = sin(lat);

    // conversion en coordonnées cartésiennes
    float nx = sinlon * coslat;
    float ny = sinlat;
    float nz = coslon * coslat;
    return vec3::fromValues(nx,ny,nz);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Tore::~Tore()
{
    delete m_Mesh;
}
