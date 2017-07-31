// Définition de la classe Joint
// Ses instances sont des jointures dans un squelette
// biblio https://www.opengl.org/wiki/Skeletal_Animation
// biblio https://research.ncl.ac.uk/game/mastersdegree/graphicsforgames/skeletalanimation/Tutorial%209%20-%20Skeletal%20Animation.pdf
// biblio http://dev.theomader.com/skeletal-animation-reviewed/
// biblio http://dev.theomader.com/skinning-reviewed/
// biblio http://dev.theomader.com/dual-quaternion-skinning/

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

#include <utils.h>
#include <SkeletonMaterial.h>
#include <VBOset.h>
#include <Joint.h>


/**
 * Constructeur
 * @param nom de la jointure
 * @param parent jointure parente
 */
Joint::Joint(std::string nom, Joint* parent)
{
    // géométrie
    m_MainAxis = vec3::create();
    m_SecondaryAxis = vec3::create();
    m_Pivot = vec3::create();
    m_NegPivot = vec3::create();
    m_Direction = vec3::create();
    m_Length = 1.0;
    m_Radius = 1.0;
    m_Min = 0.0;
    m_Max = 1.0;

    // nom et parent
    m_Name = nom;
    m_Parent = parent;

    // poids du sommet en cours de calcul
    m_Weight = 1.0;

    // matrices de transformation
    m_MatLocal = mat4::create();
    m_MatGlobal = mat4::create();
}


/**
 * définit le pivot de la jointure
 * @param pivot de rotation
 */
void Joint::setPivot(vec3 pivot)
{
    vec3::copy(m_Pivot, pivot);
    vec3::negate(m_NegPivot, pivot);
}


/**
 * définit l'axe principal de la jointure et sa longueur
 * @param direction de la jointure
 */
void Joint::setDirection(vec3 direction)
{
    m_Length = vec3::length(direction);
    vec3::copy(m_Direction, direction);
    vec3::normalize(m_Direction, m_Direction);
    vec3::cross(m_SecondaryAxis, m_MainAxis, m_Direction);
}


/**
 * définit les bornes pour calculer les poids, elles sont relatives à la longueur de la jointure
 * @param radius : rayon d'influence la jointure
 * @param min : borne min
 * @param max : borne max
 */
void Joint::setRadiusMinMax(float radius, float min, float max)
{
    m_Radius = radius;
    m_Min = min;
    m_Max = max;
}


/**
 * retourne le poids du sommet en cours de calcul
 * @return poids de la jointure
 */
float Joint::getWeight()
{
    return m_Weight;
}


/**
 * affecte le poids du sommet en cours de calcul
 * @param poids à affecter à la jointure
 */
void Joint::setWeight(float poids)
{
    m_Weight = poids;
}


/**
 * calcule le poids de la jointure en fonction des coordonnées
 * @param point considéré
 */
void Joint::computeWeights(vec3 point)
{
    // vecteur entre le pivot et le point
    vec3 vect = vec3::create();
    vec3::subtract(vect, point, m_Pivot);
    // projeter le vecteur pivot-point sur le vecteur direction
    float distance = vec3::dot(vect, m_Direction);
    // calculer l'écart à l'axe
    vec3 ortho = vec3::create();
    vec3::scale(ortho, m_Direction, distance);
    vec3::subtract(ortho, vect, ortho);
    float separation = vec3::length(ortho);
    if (separation > m_Radius) {
        // le point est trop écarté de l'axe
        m_Weight = 0.0;
    } else {
        // rendre la projection relative à la longueur
        distance = distance / m_Length;
        // calculer le poids
        if (distance <= m_Min) {
            // le point est trop en arrière pour être entraîné par cette jointure
            m_Weight = 0.0;
        } else if (distance >= m_Max) {
            // le point est totalement entraîné par cette jointure
            m_Weight = 1.0;
        } else {
            // le point est partiellement entraîné par cette jointure
            m_Weight = (distance - m_Min) / (m_Max - m_Min);
        }
    }
    // soustraire le poids de this de celui de son parent
    if (m_Parent != nullptr) {
        m_Parent->m_Weight -= m_Weight;
        if (m_Parent->m_Weight < 0.0) m_Parent->m_Weight = 0.0;
    }
}


/**
 * ré-initialise la matrice de transformation à l'identité
 */
void Joint::identity()
{
    mat4::identity(m_MatLocal);
}


/**
 * définit l'axe principal de rotation de la jointure
 * @see #rotate
 * @param axis : axe de rotation
 */
void Joint::setRotationAxis(vec3 axis)
{
    vec3::copy(m_MainAxis, axis);
    vec3::cross(m_SecondaryAxis, m_MainAxis, m_Direction);
}


/**
 * fait une rotation selon l'axe principal autour du pivot
 * @see #setRotationAxis
 * @param angle en radians
 */
void Joint::rotate(float angle)
{
    mat4::translate(m_MatLocal, m_MatLocal, m_Pivot);
    mat4::rotate(m_MatLocal, m_MatLocal, angle, m_MainAxis);
    mat4::translate(m_MatLocal, m_MatLocal, m_NegPivot);
}


/**
 * fait une rotation selon l'axe secondaire autour du pivot
 * @see #setRotationAxis
 * @param angle en radians
 */
void Joint::rotateSecondary(float angle)
{
    mat4::translate(m_MatLocal, m_MatLocal, m_Pivot);
    mat4::rotate(m_MatLocal, m_MatLocal, angle, m_SecondaryAxis);
    mat4::translate(m_MatLocal, m_MatLocal, m_NegPivot);
}


/**
 * calcule les matrices de transformation à partir des matrices locales
 */
void Joint::computeGlobalMatrix()
{
    if (m_Parent == nullptr) {
        mat4::copy(m_MatGlobal, m_MatLocal);
    } else {
        // astuce : les parents sont calculés avant les enfants, donc il n'y a pas besoin d'un appel récursif
        mat4::multiply(m_MatGlobal, m_Parent->m_MatGlobal, m_MatLocal);
    }
}


/**
 * retourne la matrice de transformation des sommets par cette jointure
 * @return matrice de transformation absolue
 */
mat4 Joint::getGlobalMatrix()
{
    return m_MatGlobal;
}


/**
 * Cette méthode supprime les ressources allouées
 */
Joint::~Joint()
{
}
