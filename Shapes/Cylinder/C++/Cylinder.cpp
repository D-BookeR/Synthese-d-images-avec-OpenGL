// Définition de la classe Cylinder


#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <math.h>

#include <utils.h>
#include <MeshModuleTopology.h>

#include <Cylinder.h>


/**
 * constructeur
 * @param spokes_count : nombre de secteurs
 */
Cylinder::Cylinder(int spokes_count)
{
    // créer le maillage : une grille rectangulaire et deux disques
    m_Mesh = new Mesh("Cylinder");
    MeshModuleTopology topology(m_Mesh);
    int diskS0 = topology.addRevolutionSurface(spokes_count, 1, "disqueS %d");
    int cylinder0    = topology.addRectangularSurface(spokes_count, 2, "cylindre %d-%d", true, false);
    int diskN0 = topology.addRevolutionSurface(spokes_count, 1, "disqueN %d");

    // définir les vertices du tube
    for (int is=0; is<spokes_count; is++) {
        // vertex sud
        MeshVertex* vS = m_Mesh->getVertex(is + 1*spokes_count + cylinder0);
        vS->setCoord(this->getCoord((float)is / spokes_count, -1.0));
        vS->setNormal(this->getNormal((float)is / spokes_count));
        // vertex nord
        MeshVertex* vN = m_Mesh->getVertex(is + 0*spokes_count + cylinder0);
        vN->setCoord(this->getCoord((float)is / spokes_count, +1.0));
        vN->setNormal(this->getNormal((float)is / spokes_count));
    }

    // définir les vertices des disques
    for (int is=0; is<spokes_count; is++) {
        // vertex sud (attention, inverser la direction)
        MeshVertex* vS = m_Mesh->getVertex(is + diskS0+1);
        vS->setCoord(this->getCoord((float)-is / spokes_count, -1.0));
        vS->setNormal(vec3::fromValues( 0.0,-1.0,0.0));
        // vertex nord
        MeshVertex* vN = m_Mesh->getVertex(is + diskN0+1);
        vN->setCoord(this->getCoord((float)is / spokes_count, +1.0));
        vN->setNormal(vec3::fromValues( 0.0,+1.0,0.0));
    }

    // définir le vertex du pole sud
    MeshVertex* vS = m_Mesh->getVertex(diskS0);
    vS->setCoord(vec3::fromValues(0.0,-1.0,0.0));
    vS->setNormal(vec3::fromValues(0.0,-1.0,0.0));

    // définir le vertex du pole nord
    MeshVertex* vN = m_Mesh->getVertex(diskN0);
    vN->setCoord(vec3::fromValues(0.0,+1.0,0.0));
    vN->setNormal(vec3::fromValues(0.0,+1.0,0.0));
}


/**
 * Cette méthode retourne les coordonnées 3D d'un point identifié par (a,b)
 * @param a : paramètre variant entre 0 et 1 en tournant autour du centre
 * @param b : hauteur sur l'axe y
 * @return vec3 contenant les coordonnées
 */
vec3 Cylinder::getCoord(float a, float b)
{
    // angle de ce point
    float angle = a * 2.0*M_PI;

    // conversion cylindriques -> cartésiennes
    float x = sin(angle);
    float y = b;
    float z = cos(angle);
    return vec3::fromValues(x,y,z);
}


/**
 * Cette méthode retourne la normale 3D au point identifié par (a,*)
 * @param a : paramètre variant entre 0 et 1 en tournant autour du centre, b n'est pas nécessaire
 * @return vec3 contenant les coordonnées du vecteur normal
 */
vec3 Cylinder::getNormal(float a)
{
    // angle de ce point
    float angle = a * 2.0*M_PI;

    float nx = sin(angle);
    float ny = 0.0;
    float nz = cos(angle);
    return vec3::fromValues(nx,ny,nz);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Cylinder::~Cylinder()
{
    delete m_Mesh;
}
