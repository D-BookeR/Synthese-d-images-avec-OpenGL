// Définition de la classe Cone


#include <GL/glew.h>
#include <GL/gl.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <math.h>

#include <utils.h>
#include <MeshModuleTopology.h>

#include <Cone.h>


/**
 * constructeur
 * @param triangle_count : nombre de secteurs (4 pour une pyramide à base carrée, bcp plus pour un cône)
 */
Cone::Cone(int triangle_count)
{
    // créer le maillage : une grille rectangulaire refermée sur elle-même
    m_Mesh = new Mesh("Cone");
    MeshModuleTopology topology(m_Mesh);
    int num0 = topology.addRectangularSurface(triangle_count, 2, "cone %d-%d", true, false);

    // définir les vertices du cône
    for (int is=0; is<triangle_count; is++) {
        // vertex sur la pointe
        MeshVertex* s0 = m_Mesh->getVertex(is + 0*triangle_count + num0);
        s0->setCoord(this->getCoord((is + 0.5) / triangle_count, 0.0));

        // vertex sur le bord
        MeshVertex* s1 = m_Mesh->getVertex(is + 1*triangle_count + num0);
        s1->setCoord(this->getCoord((is + 0.0) / triangle_count, 1.0));
    }

    // calculer les normales
    m_Mesh->computeNormals();
}


/**
 * Cette méthode retourne les coordonnées 3D d'un point identifié par (a,b)
 * @param a : paramètre variant entre 0 et 1 en tournant autour du centre
 * @param b : rayon du point, distance à l'axe Y
 * @return vec3 contenant les coordonnées
 */
vec3 Cone::getCoord(float a, float b)
{
    // angle de ce point
    float angle = a * 2.0*M_PI;

        // conversion en coordonnées cylindriques
    float x = b * sin(angle);
    float y = 1.0 - b;
    float z = b * cos(angle);
    return vec3::fromValues(x,y,z);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Cone::~Cone()
{
    delete m_Mesh;
}
