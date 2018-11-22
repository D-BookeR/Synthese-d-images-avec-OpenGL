// Définition de la classe Disk


#include <GL/glew.h>
#include <GL/gl.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <math.h>

#include <utils.h>
#include <MeshModuleTopology.h>

#include <Disk.h>


/**
 * constructeur
 * @param triangle_count : nombre de secteurs
 */
Disk::Disk(int triangle_count)
{
    // créer le maillage
    m_Mesh = new Mesh("Disk");

    // ajouter une nappe circulaire
    MeshModuleTopology topology(m_Mesh);
    int disk0 = topology.addRevolutionSurface(triangle_count, 1, "disque %d-%d");

    // définir le vertex central
    MeshVertex* center = m_Mesh->getVertex(disk0);
    center->setCoord(this->getCoord(0.0, 0.0));
    center->setNormal(vec3::fromValues(0.0,+1.0,0.0));

    // définir les vertices du bord
    for (int is=0; is<triangle_count; is++) {
        int num = is + disk0+1;
        MeshVertex* s = m_Mesh->getVertex(num);
        // calculer les coordonnées du point
        s->setCoord(this->getCoord((float)is / triangle_count, +1.0));
        s->setNormal(vec3::fromValues(0.0,+1.0,0.0));
    }
}


/**
 * Cette méthode retourne les coordonnées 3D d'un point identifié par (a,b)
 * @param a : paramètre variant entre 0 et 1 en tournant autour du centre
 * @param b : rayon du point, distance au centre
 * @return vec3 contenant les coordonnées
 */
vec3 Disk::getCoord(float a, float b)
{
    // angle de ce point
    float angle = a * 2.0*M_PI;

    // conversion en coordonnées cartésiennes
    float x = b * sin(angle);
    float y = 0.0;
    float z = b * cos(angle);
    return vec3::fromValues(x,y,z);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Disk::~Disk()
{
    delete m_Mesh;
}
