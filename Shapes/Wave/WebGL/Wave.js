// Définition de la classe Wave

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleTopology.js");


// Définition de la classe Wave
class Wave
{
    /**
     * Cette fonction définit la classe Wave.
     * @param border : contour à dessiner
     * @param spokes_count : nombre de secteurs
     * @param segments_count : nombre de cercles concentriques pour décrire la fonction
     */
    constructor(border, spokes_count, segments_count)
    {
        this.m_Contour = border;

        // créer le maillage : une grille plane polaire
        this.m_Mesh = new Mesh("Wave");
        let topology = new MeshModuleTopology(this.m_Mesh);
        let num0 = topology.addRectangularSurface(spokes_count, segments_count, "grille %d-%d", true, false);

        // parcourir les sommets pour définir leurs coordonnées
        for (let is=0; is<spokes_count; is++) {
            let a = 1.0 - is / spokes_count;
            for (let ip=0; ip<segments_count; ip++) {
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

        // angle de ce point
        let angle = a * 2.0 * Math.PI;

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
