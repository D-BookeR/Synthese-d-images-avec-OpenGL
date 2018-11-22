// Définition de la classe Capsule


#include <GL/glew.h>
#include <GL/gl.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <math.h>

#include <utils.h>
#include <MeshModuleTopology.h>

#include <Capsule.h>



/**
 * Cette fonction définit la classe Capsule.
 * @param spokes_count : nombre de rayons autour du centre
 * @param segments_count : nombre de segments sur chaque rayon
 * @param length : longueur de la partie cylindrique
 */
Capsule::Capsule(int spokes_count, int segments_count, float length)
{
    // créer le maillage : deux disques
    m_Mesh = new Mesh("Capsule");
    MeshModuleTopology topology(m_Mesh);
    int diskS0 = topology.addRevolutionSurface(spokes_count, segments_count, "disqueS %d-%d");
    int diskN0 = topology.addRevolutionSurface(spokes_count, segments_count, "disqueN %d-%d");

    // rajouter des quads entre les deux disques
    for (int ir=0; ir<spokes_count; ir++) {
        // disque sud, tourner dans le sens inverse
        int nvS0 = ((spokes_count-ir)%spokes_count)*segments_count + segments_count + diskS0;
        MeshVertex* vS0 = m_Mesh->getVertex(nvS0);
        int nvS1 = ((spokes_count-ir-1)%spokes_count)*segments_count + segments_count + diskS0;
        MeshVertex* vS1 = m_Mesh->getVertex(nvS1);
        // disque nord
        int nvN0 = ((ir+0)%spokes_count)*segments_count + segments_count + diskN0;
        MeshVertex* vN0 = m_Mesh->getVertex(nvN0);
        int nvN1 = ((ir+1)%spokes_count)*segments_count + segments_count + diskN0;
        MeshVertex* vN1 = m_Mesh->getVertex(nvN1);
        m_Mesh->addQuad(vS1, vN1, vN0, vS0);
    }

    // définir les vertices des deux disques afin de créer des hémisphères
    for (int ir=0; ir<spokes_count; ir++) {
        for (int is=0; is<segments_count; is++) {
            // vertex sud (attention, inverser la direction)
            MeshVertex* vS = m_Mesh->getVertex(ir*segments_count + is + diskS0+1);
            vec3 coordsS = this->getCoord(-(float)ir / spokes_count, -(float)(segments_count-1-is) / segments_count, length);
            vS->setCoord(coordsS);
            vec3 normalS = this->getNormal(-(float)ir / spokes_count, -(float)(segments_count-1-is) / segments_count);
            vS->setNormal(normalS);
            // vertex nord
            MeshVertex* vN = m_Mesh->getVertex(ir*segments_count + is + diskN0+1);
            vec3 coordsN = this->getCoord(+(float)ir / spokes_count,+(float)(segments_count-1-is) / segments_count, length);
            vN->setCoord(coordsN);
            vec3 normalN = this->getNormal(+(float)ir / spokes_count,+(float)(segments_count-1-is) / segments_count);
            vN->setNormal(normalN);
        }
    }

    // définir le vertex du pole sud
    MeshVertex* vS = m_Mesh->getVertex(diskS0);
    vS->setCoord(this->getCoord(0, -1, length));
    vS->setNormal(this->getNormal(0, -1));

    // définir le vertex du pole nord
    MeshVertex* vN = m_Mesh->getVertex(diskN0);
    vN->setCoord(this->getCoord(0, +1, length));
    vN->setNormal(this->getNormal(0, +1));
}


/**
 * Cette méthode retourne les coordonnées 3D d'un point identifié par (a,b)
 * @param a : paramètre variant entre 0 et 1 en tournant autour du centre
 * @param b : varie entre 0 et 1 : 1=pôle, 0=équateur
 * @param length de la partie cylindrique
 * @return vec3 contenant les coordonnées
 */
vec3 Capsule::getCoord(float a, float b, float length)
{
    // angles de ce point
    float longitude = a * 2.0*M_PI;
    float latitude  = b * 0.5*M_PI;

    // conversion sphériques -> cartésiennes
    float x = cos(latitude) * sin(longitude);
    float y = sin(latitude);
    float z = cos(latitude) * cos(longitude);

    if (b > 0) {
        y += length;
    } else {
        y -= length;
    }

    return vec3::fromValues(x,y,z);
}


/**
 * Cette méthode retourne la normale d'un point identifié par (a,b)
 * @param a : paramètre variant entre 0 et 1 en tournant autour du centre
 * @param b : varie entre 0 et 1 : 1=pôle, 0=équateur
 * @return vec3 contenant les coordonnées du vecteur normal
 */
vec3 Capsule::getNormal(float a, float b)
{
    // angles de ce point
    float longitude = a * 2.0*M_PI;
    float latitude  = b * 0.5*M_PI;

    // conversion sphériques -> cartésiennes
    float x = cos(latitude) * sin(longitude);
    float y = sin(latitude);
    float z = cos(latitude) * cos(longitude);
    return vec3::fromValues(x,y,z);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Capsule::~Capsule()
{
    delete m_Mesh;
}
