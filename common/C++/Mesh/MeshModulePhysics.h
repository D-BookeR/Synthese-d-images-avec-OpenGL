#ifndef MESH_MESHMODULEPHYSICS_H
#define MESH_MESHMODULEPHYSICS_H

/**
 * Ce module rajoute des fonctions pour calculer les éléments physiques
 * d'un Maillage : volume et masse, CdG, matrice d'inertie
 *
 * Biblio :  Fast and Accurate Computation of Polyhedral Mass Properties - Mirtich 1996
 * http://www.cs.berkeley.edu/~jfc/mirtich/massProps.html
 */

#include <MeshModule.h>


class MeshModulePhysics: public MeshModule
{
private:

    /// densité de cet objet
    float m_Density;

    /// volume calculé par l'algorithme de Mirtich
    float m_Volume;

    /// masse = volume * densité
    float m_Mass;

    /// position du centre de gravité
    vec3 m_CoG;


public:

    /**
     * initialise le module sur le maillage fourni
     * @param mesh maillage concerné
     */
    MeshModulePhysics(Mesh* mesh=nullptr);

    /**
     * Définit la densité du maillage
     * @param density valeur de la densité
     */
    void setDensity(float density);

    /**
     * Cette méthode calcule différentes informations physiques sur le maillage
     */
    void compVolumeIntegrals();

    /**
     * retourne le volume du maillage
     * @return volume du maillage
     */
    float getVolume();

    /**
     * retourne la masse du maillage
     * @return masse du maillage
     */
    float getMass();

    /**
     * retourne le centre de gravité du maillage
     * @return centre de gravité du maillage
     */
    vec3 getCoG();

};

#endif
