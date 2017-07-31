// Définition de la classe Lathe

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleTopology.js");


class Lathe
{
    /**
     * Cette fonction définit la classe Lathe.
     * @param border : tableau de vec2 représentant la ligne qu'il faut faire tourner
     * @param spokes_count : nombre de secteurs
     */
    constructor(border, spokes_count)
    {
        // initialiser les variables membres
        this.m_Contour = border;
        let points_count = this.m_Contour.length;

        // créer le maillage : une grille rectangulaire
        this.m_Mesh = new Mesh("Lathe");
        let topology = new MeshModuleTopology(this.m_Mesh);
        let num0 = topology.addHexagonalSurface(spokes_count, points_count, "tour %d-%d", true, false);

        // parcourir les sommets pour définir leurs coordonnées
        for (let is=0; is<spokes_count; is++) {
            for (let ip=0; ip<points_count; ip++) {
                // angle 0..1 autour de l'axe
                let a = (is - 0.5*(ip%2)) / spokes_count;
                // numéro du vertex
                let num = is + ip*spokes_count + num0;
                // récupérer le sommet concerné
                let vertex = this.m_Mesh.getVertex(num);
                // définir les coordonnées 3D du point
                vertex.setCoord(this.getCoord(a, ip));
            }
        }

        // calculer les normales
        this.m_Mesh.computeNormals();
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
     * Cette méthode retourne les coordonnées 3D d'un point identifié par (a,ip)
     * @param a : paramètre variant entre 0 et 1 en tournant autour de l'axe Y
     * @param ip : numéro du point sur le contour
     * @return vec3 contenant les coordonnées
     */
    getCoord(a, ip)
    {
        // point du contour : (r, y)
        let point = this.m_Contour[ip];
        let r = point[0];
        let y = point[1];

        // angle de ce point, attention, ouest->est
        let angle = -a * 2.0 * Math.PI;

        // conversion polaires -> cartésiennes
        let x = r * Math.sin(angle);
        let z = r * Math.cos(angle);
        return vec3.fromValues(x,y,z);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Mesh.destroy();
    }
}
