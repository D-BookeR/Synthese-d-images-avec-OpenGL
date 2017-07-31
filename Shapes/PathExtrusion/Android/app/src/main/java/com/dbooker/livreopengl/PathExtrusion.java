package com.dbooker.livreopengl;

import java.util.List;

import com.dbooker.livreopengl.glmatrix.vec2;
import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.mesh.Mesh;
import com.dbooker.livreopengl.mesh.MeshModuleTopology;
import com.dbooker.livreopengl.mesh.MeshVertex;


// Définition de la classe PathExtrusion
class PathExtrusion
{

    private Mesh m_Mesh;
    private List<vec2> borderXY;
    private List<vec2> borderZY;

    /**
     * Cette fonction définit la classe PathExtrusion.
     * @param borderXY : ligne qu'il faut faire balayer le long du contour ZY
     * @param borderZY : ligne qui définit la trajectoire du contourXY
     */
    PathExtrusion(List<vec2> borderXY, List<vec2> borderZY) throws Exception
    {
        // initialiser les variables membres
        this.borderXY = borderXY;
        this.borderZY = borderZY;
        int points_countX = borderXY.size();
        int points_countZ = borderZY.size();

        // créer le maillage : une grille rectangulaire
        m_Mesh = new Mesh("PathExtrusion");
        MeshModuleTopology topology = new MeshModuleTopology(m_Mesh);
        int num0 = topology.addRectangularSurface(points_countX, points_countZ, "grille %d-%d", false, false);

        // parcourir les sommets pour définir les coordonnées
        for (int ipx=0; ipx<points_countX; ipx++) {
            for (int ipz=0; ipz<points_countZ; ipz++) {
                // numéro du vertex
                int num = ipx + ipz*points_countX + num0;
                // récupérer le sommet concerné
                MeshVertex vertex = m_Mesh.getVertex(num);
                // définir les coordonnées 3D du point
                vertex.setCoord(getCoord(ipx, ipz));
                vertex.setNormal(getNormal(ipx, ipz));
            }
        }

        // calculer les normales, décommenter pour comparer
        //m_Mesh.computeNormals();
    }


    /**
     * fournit le maillage de cet objet
     * @return maillage
     */
    public Mesh getMesh()
    {
        return m_Mesh;
    }


    /**
     * Cette méthode retourne les coordonnées 3D d'un point identifié par (ix, iz)
     * @param ix : numéro du point sur le contourXY
     * @param iz : numéro du point sur le contourZY
     * @return vec3 contenant les coordonnées
     */
    private vec3 getCoord(int ix, int iz)
    {
        // point de chaque contour
        vec2 pointXY = borderXY.get(ix);
        vec2 pointZY = borderZY.get(iz);

        // prise en compte des deux contours
        float x = pointXY.get(0);
        float y = pointXY.get(1) + pointZY.get(1);
        float z = pointZY.get(0);
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
    private vec2 computeDerivative(List<vec2> border, int i)
    {
        // test sur le nombre de points du contour
        int points_count = border.size();
        if (points_count <= 1) return vec2.create();

        // segment précédant i, s'il y en a un
        vec2 prec = vec2.create();
        if (i > 0) {
            // écarts en x et y entre les deux points
            vec2.subtract(prec, border.get(i), border.get(i-1));
            // normaliser le vecteur obtenu
            vec2.normalize(prec, prec);
        }

        // segment suivant i, s'il y en a un
        vec2 svt = vec2.create();
        if (i < points_count-1) {
            // écarts en x et y entre les deux points
            vec2.subtract(svt, border.get(i+1), border.get(i));
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
    private vec3 getNormal(int ix, int iz)
    {
        // calculer les dérivées partielles au niveau du point ix,iz
        vec2 derivativeX = computeDerivative(borderXY, ix);
        vec2 derivativeZ = computeDerivative(borderZY, iz);

        // créer un vecteur orthogonal à la tangente au contour en i
        vec3 normal = vec3.fromValues(-derivativeX.get(1)/derivativeX.get(0), 1, -derivativeZ.get(1)/derivativeZ.get(0));
        vec3.normalize(normal, normal);
        return normal;
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    public void destroy()
    {
        m_Mesh.destroy();
    }
}
