#ifndef JOINTURE_H
#define JOINTURE_H

// Définition de la classe Joint

#include <sstream>

#include <gl-matrix.h>
#include <utils.h>


class Joint
{
public:
    /**
     * Constructeur
     * @param nom de la jointure
     * @param parent jointure parente
     */
    Joint(std::string nom, Joint* parent);

    /**
     * définit le pivot de la jointure
     * @param pivot de rotation
     */
    void setPivot(vec3 pivot);

    /**
     * définit l'axe principal de la jointure et sa longueur
     * @param direction de la jointure
     */
    void setDirection(vec3 direction);

    /**
     * définit les bornes pour calculer les poids, elles sont relatives à la longueur de la jointure
     * @param radius : rayon d'influence la jointure
     * @param min : borne min
     * @param max : borne max
     */
    void setRadiusMinMax(float radius, float min, float max);

    /**
     * retourne le poids du sommet en cours de calcul
     * @return poids de la jointure
     */
    float getWeight();

    /**
     * affecte le poids du sommet en cours de calcul
     * @param poids à affecter à la jointure
     */
    void setWeight(float poids);

    /**
     * calcule le poids de la jointure en fonction des coordonnées
     * @param point considéré
     */
    void computeWeights(vec3 point);

    /**
     * ré-initialise la matrice de transformation à l'identité
     */
    void identity();

    /**
     * définit l'axe principal de rotation de la jointure
     * @see #rotate
     * @param axis : axe de rotation
     */
    void setRotationAxis(vec3 axis);

    /**
     * fait une rotation selon l'axe principal autour du pivot
     * @see #setRotationAxis
     * @param angle en radians
     */
    void rotate(float angle);

    /**
     * fait une rotation selon l'axe secondaire autour du pivot
     * @see #setRotationAxis
     * @param angle en radians
     */
    void rotateSecondary(float angle);

    /**
     * calcule les matrices de transformation à partir des matrices locales
     */
    void computeGlobalMatrix();

    /**
     * retourne la matrice de transformation des sommets par cette jointure
     * @return matrice de transformation absolue
     */
    mat4 getGlobalMatrix();

    /**
     * Cette méthode supprime les ressources allouées
     */
    ~Joint();


protected:

    // géométrie
    vec3 m_MainAxis;        // axe de rotation
    vec3 m_SecondaryAxis;   // axe de rotation secondaire (orthog au principal et à la direction)
    vec3 m_Pivot;           // centre de rotation
    vec3 m_NegPivot;        // -centre de rotation
    vec3 m_Direction;       // direction au repos
    float m_Length;         // longueur de l'os
    float m_Radius;         // rayon d'influence
    float m_Min;            // plage d'influence à rapporter à la longueur
    float m_Max;            // plage d'influence à rapporter à la longueur

    // nom et parent
    std::string m_Name;
    Joint* m_Parent;

    // matrices de transformation
    mat4 m_MatLocal;
    mat4 m_MatGlobal;

    // poids du sommet en cours de calcul
    float m_Weight;
};

#endif
