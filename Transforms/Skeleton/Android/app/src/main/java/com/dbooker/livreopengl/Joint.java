package com.dbooker.livreopengl;

import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.vec3;


@SuppressWarnings("unused")
public class Joint
{
    protected static final String TAG = "LivreOpenGL";

    protected vec3 m_MainAxis;          // axe de rotation
    protected vec3 m_SecondaryAxis;     // axe de rotation secondaire (orthog au principal et à la direction)
    protected vec3 m_Pivot;             // centre de rotation
    protected vec3 m_NegPivot;          // -centre de rotation
    protected vec3 m_Direction;         // direction au repos
    protected float m_Length;           // longueur de l'os
    protected float m_Radius;           // rayon d'influence
    protected float m_Min;              // plage d'influence à rapporter à la longueur
    protected float m_Max;              // plage d'influence à rapporter à la longueur

    // parent et enfants
    protected String m_Name;
    protected Joint m_Parent;

    // poids du sommet en cours de calcul
    protected float m_Weight;

    // matrices de transformation
    protected mat4 m_MatLocal;
    protected mat4 m_MatGlobal;


    /**
     * Constructeur
     * @param nom de la jointure
     * @param parent jointure parente
     */
    public Joint(String nom, Joint parent)
    {
        // géométrie
        m_MainAxis = vec3.create();
        m_SecondaryAxis = vec3.create();
        m_Pivot = vec3.create();
        m_NegPivot = vec3.create();
        m_Direction = vec3.create();
        m_Length = 1.0f;
        m_Radius = 1.0f;
        m_Min = 0.0f;
        m_Max = 1.0f;

        // nom et parent
        m_Name = nom;
        m_Parent = parent;

        // poids du sommet en cours de calcul
        m_Weight = 1.0f;

        // matrices de transformation
        m_MatLocal = mat4.create();
        m_MatGlobal = mat4.create();
    }


    /**
     * définit le pivot de la jointure
     * @param pivot de rotation
     */
    public void setPivot(vec3 pivot)
    {
        vec3.copy(m_Pivot, pivot);
        vec3.negate(m_NegPivot, pivot);
    }


    /**
     * définit l'axe principal de la jointure et sa longueur
     * @param direction de la jointure
     */
    public void setDirection(vec3 direction)
    {
        m_Length = vec3.length(direction);
        vec3.copy(m_Direction, direction);
        vec3.normalize(m_Direction, m_Direction);
        vec3.cross(m_SecondaryAxis, m_MainAxis, m_Direction);
    }


    /**
     * définit les bornes pour calculer les poids, elles sont relatives à la longueur de la jointure
     * @param radius : rayon d'influence la jointure
     * @param min : borne min
     * @param max : borne max
     */
    public void setRadiusMinMax(float radius, float min, float max)
    {
        m_Radius = radius;
        m_Min = min;
        m_Max = max;
    }


    /**
     * retourne le poids du sommet en cours de calcul
     * @return poids de la jointure
     */
    public float getWeight()
    {
        return m_Weight;
    }


    /**
     * affecte le poids du sommet en cours de calcul
     * @param poids à affecter à la jointure
     */
    public void setWeight(float poids)
    {
        m_Weight = poids;
    }


    /**
     * calcule le poids de la jointure en fonction des coordonnées
     * @param point considéré
     */
    public void computeWeights(vec3 point)
    {
        // vecteur entre le pivot et le point
        vec3 vect = vec3.create();
        vec3.subtract(vect, point, m_Pivot);
        // projeter le vecteur pivot-point sur le vecteur direction
        float distance = vec3.dot(vect, m_Direction);
        // calculer l'écart à l'axe
        vec3 ortho = vec3.create();
        vec3.scale(ortho, m_Direction, distance);
        vec3.subtract(ortho, vect, ortho);
        float separation = vec3.length(ortho);
        if (separation > m_Radius) {
            // le point est trop écarté de l'axe
            m_Weight = 0.0f;
        } else {
            // rendre la projection relative à la longueur
            distance = distance / m_Length;
            // calculer le poids
            if (distance <= m_Min) {
                // le point est trop en arrière pour être entraîné par cette jointure
                m_Weight = 0.0f;
            } else if (distance >= m_Max) {
                // le point est totalement entraîné par cette jointure
                m_Weight = 1.0f;
            } else {
                // le point est partiellement entraîné par cette jointure
                m_Weight = (distance - m_Min) / (m_Max - m_Min);
            }
        }
        // soustraire le poids de this de celui de son parent
        if (m_Parent != null) {
            m_Parent.m_Weight -= m_Weight;
            if (m_Parent.m_Weight < 0.0f) m_Parent.m_Weight = 0.0f;
        }
    }


    /**
     * ré-initialise la matrice de transformation à l'identité
     */
    public void identity()
    {
        mat4.identity(m_MatLocal);
    }


    /**
     * définit l'axe principal de rotation de la jointure
     * @see #rotate
     * @param axis : axe de rotation
     */
    public void setRotationAxis(vec3 axis)
    {
        vec3.copy(m_MainAxis, axis);
        vec3.cross(m_SecondaryAxis, m_MainAxis, m_Direction);
    }


    /**
     * fait une rotation selon l'axe principal autour du pivot
     * @see #setRotationAxis
     * @param angle en radians
     */
    public void rotate(float angle)
    {
        mat4.translate(m_MatLocal, m_MatLocal, m_Pivot);
        mat4.rotate(m_MatLocal, m_MatLocal, angle, m_MainAxis);
        mat4.translate(m_MatLocal, m_MatLocal, m_NegPivot);
    }


    /**
     * fait une rotation selon l'axe secondaire autour du pivot
     * @see #setRotationAxis
     * @param angle en radians
     */
    public void rotateSecondary(float angle)
    {
        mat4.translate(m_MatLocal, m_MatLocal, m_Pivot);
        mat4.rotate(m_MatLocal, m_MatLocal, angle, m_SecondaryAxis);
        mat4.translate(m_MatLocal, m_MatLocal, m_NegPivot);
    }


    /**
     * calcule les matrices de transformation à partir des matrices locales
     */
    public void computeGlobalMatrix()
    {
        if (m_Parent == null) {
            mat4.copy(m_MatGlobal, m_MatLocal);
        } else {
            mat4.multiply(m_MatGlobal, m_Parent.m_MatGlobal, m_MatLocal);
        }
    }


    /**
     * retourne la matrice de transformation des sommets par cette jointure
     * @return matrice de transformation absolue
     */
    public mat4 getGlobalMatrix()
    {
        return m_MatGlobal;
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    public void destroy()
    {
    }
}
