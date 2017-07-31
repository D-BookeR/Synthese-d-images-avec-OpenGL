// Définition de la classe ConeFrustum

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <math.h>

#include <utils.h>
#include <MeshModuleTopology.h>

#include <ConeFrustum.h>



/**
 * Cette fonction définit la classe ConeFrustum.
 * @param spokes_count : nombre de secteurs
 * @param radius_bottom : rayon en bas (pour y=-1)
 * @param radius_top : rayon en haut (pour y=+1)
 */
ConeFrustum::ConeFrustum(int spokes_count, float radius_bottom, float radius_top)
{
    // créer le maillage : une grille et deux disques
    m_Mesh = new Mesh("ConeFrustum");
    MeshModuleTopology topology(m_Mesh);
    int diskS0 = topology.addRevolutionSurface(spokes_count, 1, "disqueS %d");
    int grid0  = topology.addRectangularSurface(spokes_count, 2, "tronc %d-%d", true, false);
    int diskN0 = topology.addRevolutionSurface(spokes_count, 1, "disqueN %d");

    // définir les vertices du tube
    for (int is=0; is<spokes_count; is++) {
        float a = (float)is / spokes_count;
        // vertex sud
        MeshVertex* vS = m_Mesh->getVertex(is + 1*spokes_count + grid0);
        vS->setCoord(this->getCoord(radius_bottom, radius_top, a, -1.0));
        // vertex nord
        MeshVertex* vN = m_Mesh->getVertex(is + 0*spokes_count + grid0);
        vN->setCoord(this->getCoord(radius_bottom, radius_top, a, +1.0));
    }

    // définir les vertices des disques
    for (int is=0; is<spokes_count; is++) {
        float a = (float)is / spokes_count;
        // vertex sud (attention, inverser la direction)
        MeshVertex* vS = m_Mesh->getVertex(is + diskS0+1);
        vS->setCoord(this->getCoord(radius_bottom, radius_top, -a, -1.0));
        // vertex nord
        MeshVertex* vN = m_Mesh->getVertex(is + diskN0+1);
        vN->setCoord(this->getCoord(radius_bottom, radius_top, +a, +1.0));
    }

    // définir le vertex du pole sud
    MeshVertex* vS = m_Mesh->getVertex(diskS0);
    vS->setCoord(vec3::fromValues(0.0,-1.0,0.0));

    // définir le vertex du pole nord
    MeshVertex* vN = m_Mesh->getVertex(diskN0);
    vN->setCoord(vec3::fromValues(0.0,+1.0,0.0));

    // calculer les normales
    m_Mesh->computeNormals();
}


/**
 * Cette méthode retourne les coordonnées 3D d'un point identifié par (a,b)
 * @param radius_bottom : rayon en bas, quand b vaut -1
 * @param radius_top : rayon en haut, quand b vaut +1
 * @param a : paramètre variant entre 0 et 1 en tournant autour du centre
 * @param b : hauteur considérée, entre -1 et +1
 * @return vec3 contenant les coordonnées
 */
vec3 ConeFrustum::getCoord(float radius_bottom, float radius_top, float a, float b)
{
    // angle de ce point
    float angle = a * 2.0*M_PI;

    // rayon de ce point
    float k = (b + 1.0) / 2.0;
    float r = k*radius_top + (1.0-k)*radius_bottom;

    // conversion cylindriques -> cartésiennes
    float x = r * sin(angle);
    float y = b;
    float z = r * cos(angle);
    return vec3::fromValues(x,y,z);
}


/**
 * Cette méthode supprime les ressources allouées
 */
ConeFrustum::~ConeFrustum()
{
    delete m_Mesh;
}
