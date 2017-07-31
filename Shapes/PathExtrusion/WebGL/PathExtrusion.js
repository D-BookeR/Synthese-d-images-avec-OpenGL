// Définition de la classe PathExtrusion

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleTopology.js");


// Définition de la classe PathExtrusion
class PathExtrusion
{
    /**
     * Cette fonction définit la classe PathExtrusion.
     * @param borderXY : ligne qu'il faut faire balayer le long du contour ZY
     * @param borderZY : ligne qui définit la trajectoire du contourXY
     */
    constructor(borderXY, borderZY)
    {
        // initialiser les variables membres
        this.borderXY = borderXY;
        this.borderZY = borderZY;
        let points_countX = borderXY.length;
        let points_countZ = borderZY.length;

        // créer le maillage : une grille rectangulaire
        this.m_Mesh = new Mesh("PathExtrusion");
        let topology = new MeshModuleTopology(this.m_Mesh);
        let num0 = topology.addRectangularSurface(points_countX, points_countZ, "grille %d-%d", false, false);

        // parcourir les sommets pour définir les coordonnées
        for (let ipx=0; ipx<points_countX; ipx++) {
            for (let ipz=0; ipz<points_countZ; ipz++) {
                // numéro du vertex
                let num = ipx + ipz*points_countX + num0;
                // récupérer le sommet concerné
                let vertex = this.m_Mesh.getVertex(num);
                // définir les coordonnées 3D du point
                vertex.setCoord(this.getCoord(ipx, ipz));
                vertex.setNormal(this.getNormal(ipx, ipz));
            }
        }

        // calculer les normales, décommenter pour comparer
        //this.m_Mesh.computeNormals();
    }


    /**
     * fournit le maillage de cet objet
     * @return maillage
     */
    getMesh()
    {
        return this.m_Mesh;
    }


    /**
     * Cette méthode retourne les coordonnées 3D d'un point identifié par (ix, iz)
     * @param ix : numéro du point sur le contourXY
     * @param iz : numéro du point sur le contourZY
     * @return vec3 contenant les coordonnées
     */
    getCoord(ix, iz)
    {
        // point de chaque contour
        let pointXY = this.borderXY[ix];
        let pointZY = this.borderZY[iz];

        // prise en compte des deux contours
        let x = pointXY[0];
        let y = pointXY[1] + pointZY[1];
        let z = pointZY[0];
        return vec3.fromValues(x,y,z);
    }


    /**
     * Cette fonction calcule la dérivée dy/dx au niveau du point n°i d'un contour (x,y).
     * Elle calcule la moyenne des deux dérivées : celle du segment à gauche et celle du
     * segment à droite de i, si ces segments existent. Si un point est doublé, à droite
     * ou à gauche, alors le segment correspondant n'est pas pris en compte ; cela crée
     * une rupture dans la succession des dérivées des segments.
     * @param border : tableau de vec2(x,y)
     * @param i : numéro du point dans ce tableau
     * @return vec2(dx, dy) : représentant la dérivée dy/dx en ce point
     */
    computeDerivative(border, i)
    {
        // test sur le nombre de points du contour
        let points_count = border.length;
        if (points_count <= 1) return vec2.create();

        // segment précédant i, s'il y en a un
        let prec = vec2.create();
        if (i > 0) {
            // écarts en x et y entre les deux points
            vec2.subtract(prec, border[i], border[i-1]);
            // normaliser le vecteur obtenu
            vec2.normalize(prec, prec);
        }

        // segment suivant i, s'il y en a un
        let svt = vec2.create();
        if (i < points_count-1) {
            // écarts en x et y entre les deux points
            vec2.subtract(svt, border[i+1], border[i]);
            // normaliser le vecteur obtenu
            vec2.normalize(svt, svt);
        }

        // moyenne normalisée entre les deux vecteurs
        if (i <= 0) return svt;
        if (i >= points_count-1) return prec;
        vec2.add(prec, prec, svt);
        vec2.normalize(prec, prec);
        return prec;
    }


    /**
     * Cette fonction calcule la normale au niveau du point (ix,iz)
     * @param ix : numéro du point dans le tableau contourXY
     * @param iz : numéro du point dans le tableau contourZY
     * @return vec3 : normale en ce point
     */
    getNormal(ix, iz)
    {
        // calculer les dérivées partielles au niveau du point ix,iz
        let derivativeX = this.computeDerivative(this.borderXY, ix);
        let derivativeZ = this.computeDerivative(this.borderZY, iz);

        // créer un vecteur orthogonal à la tangente au contour en i
        let normal = vec3.fromValues(-derivativeX[1]/derivativeX[0], 1, -derivativeZ[1]/derivativeZ[0]);
        vec3.normalize(normal, normal);
        return normal;
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Mesh.destroy();
    }
}
