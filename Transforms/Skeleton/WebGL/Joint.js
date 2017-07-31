// Définition de la classe Joint
// Ses instances sont des jointures dans un squelette
// biblio https://www.opengl.org/wiki/Skeletal_Animation
// biblio https://research.ncl.ac.uk/game/mastersdegree/graphicsforgames/skeletalanimation/Tutorial%209%20-%20Skeletal%20Animation.pdf
// biblio http://dev.theomader.com/skeletal-animation-reviewed/
// biblio http://dev.theomader.com/skinning-reviewed/
// biblio http://dev.theomader.com/dual-quaternion-skinning/

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshVertex.js");


class Joint
{
    /**
     * Constructeur
     * @param nom de la jointure
     * @param parent jointure parente
     */
    constructor(nom, parent=null)
    {
        // géométrie
        this.m_MainAxis = vec3.create();        // axe de rotation principal
        this.m_SecondaryAxis = vec3.create();   // axe de rotation secondaire (orthog au principal et à la direction)
        this.m_Pivot = vec3.create();           // centre de rotation
        this.m_NegPivot = vec3.create();        // -centre de rotation
        this.m_Direction = vec3.create();       // direction au repos
        this.m_Length = 1.0;                    // longueur de l'os
        this.m_Radius = 1.0;                    // rayon d'influence
        this.m_Min = 0.0;                       // plage d'influence à rapporter à la longueur
        this.m_Max = 1.0;                       // plage d'influence à rapporter à la longueur

        // nom et parent
        this.m_Name = nom;
        this.m_Parent = parent;

        // poids du sommet en cours de calcul
        this.m_Weight = 1.0;

        // matrices de transformation
        this.m_MatLocal = mat4.create();
        this.m_MatGlobal = mat4.create();
    }


    /**
     * définit le pivot de la jointure
     * @param pivot de rotation
     */
    setPivot(pivot)
    {
        vec3.copy(this.m_Pivot, pivot);
        vec3.negate(this.m_NegPivot, pivot);
    }


    /**
     * définit l'axe principal de la jointure et sa longueur
     * @param direction de la jointure
     */
    setDirection(direction)
    {
        this.m_Length = vec3.length(direction);
        vec3.copy(this.m_Direction, direction);
        vec3.normalize(this.m_Direction, this.m_Direction);
        vec3.cross(this.m_SecondaryAxis, this.m_MainAxis, this.m_Direction);
    }


    /**
     * définit les bornes pour calculer les poids, elles sont relatives à la longueur de la jointure
     * @param radius : rayon d'influence de la jointure
     * @param min : borne min
     * @param max : borne max
     */
    setRadiusMinMax(radius, min, max)
    {
        this.m_Radius = radius;
        this.m_Min = min;
        this.m_Max = max;
    }


    /**
     * retourne le poids du sommet en cours de calcul
     * @return poids de la jointure
     */
    getWeight()
    {
        return this.m_Weight;
    }


    /**
     * affecte le poids du sommet en cours de calcul
     * @param weight : poids à affecter à la jointure
     */
    setWeight(weight)
    {
        this.m_Weight = weight;
    }


    /**
     * calcule le poids de la jointure en fonction des coordonnées
     * @param point considéré
     */
    computeWeights(point)
    {
        // vecteur entre le pivot et le point
        let vect = vec3.create();
        vec3.subtract(vect, point, this.m_Pivot);
        // projeter le vecteur pivot-point sur le vecteur direction
        let distance = vec3.dot(vect, this.m_Direction);
        // calculer l'écart à l'axe
        let ortho = vec3.create();
        vec3.scale(ortho, this.m_Direction, distance);
        vec3.subtract(ortho, vect, ortho);
        let separation = vec3.length(ortho);
        if (separation > this.m_Radius) {
            // le point est trop écarté de l'axe
            this.m_Weight = 0.0;
        } else {
            // rendre la projection relative à la longueur
            distance = distance / this.m_Length;
            // calculer le poids
            if (distance <= this.m_Min) {
                // le point est trop en arrière pour être entraîné par cette jointure
                this.m_Weight = 0.0;
            } else if (distance >= this.m_Max) {
                // le point est totalement entraîné par cette jointure
                this.m_Weight = 1.0;
            } else {
                // le point est partiellement entraîné par cette jointure
                this.m_Weight = (distance - this.m_Min) / (this.m_Max - this.m_Min);
            }
        }
        // soustraire le poids de this de celui de son parent
        if (this.m_Parent != null) {
            this.m_Parent.m_Weight -= this.m_Weight;
            if (this.m_Parent.m_Weight < 0.0) this.m_Parent.m_Weight = 0.0;
        }
    }


    /**
     * ré-initialise la matrice de transformation à l'identité
     */
    identity()
    {
        mat4.identity(this.m_MatLocal);
    }


    /**
     * définit l'axe principal de rotation de la jointure
     * @see #rotate
     * @param axis : axe de rotation
     */
    setRotationAxis(axis)
    {
        vec3.copy(this.m_MainAxis, axis);
        vec3.cross(this.m_SecondaryAxis, this.m_MainAxis, this.m_Direction);
    }


    /**
     * fait une rotation selon l'axe principal autour du pivot
     * @see #setRotationAxis
     * @param angle en radians
     */
    rotate(angle)
    {
        mat4.translate(this.m_MatLocal, this.m_MatLocal, this.m_Pivot);
        mat4.rotate(this.m_MatLocal, this.m_MatLocal, angle, this.m_MainAxis);
        mat4.translate(this.m_MatLocal, this.m_MatLocal, this.m_NegPivot);
    }


    /**
     * fait une rotation selon l'axe secondaire autour du pivot
     * @see #setRotationAxis
     * @param angle en radians
     */
    rotateSecondary(angle)
    {
        mat4.translate(this.m_MatLocal, this.m_MatLocal, this.m_Pivot);
        mat4.rotate(this.m_MatLocal, this.m_MatLocal, angle, this.m_SecondaryAxis);
        mat4.translate(this.m_MatLocal, this.m_MatLocal, this.m_NegPivot);
    }


    /**
     * calcule les matrices de transformation à partir des matrices locales
     */
    computeGlobalMatrix()
    {
        if (this.m_Parent == null) {
            mat4.copy(this.m_MatGlobal, this.m_MatLocal);
        } else {
            // astuce : les parents sont calculés avant les enfants, donc il n'y a pas besoin d'un appel récursif
            mat4.multiply(this.m_MatGlobal, this.m_Parent.m_MatGlobal, this.m_MatLocal);
        }
    }


    /**
     * retourne la matrice de transformation des sommets par cette jointure
     * @return matrice de transformation absolue
     */
    getGlobalMatrix()
    {
        return this.m_MatGlobal;
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
    }
}
