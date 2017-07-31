// Définition de la classe SphereLonLat

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleTopology.js");


class SphereLonLat
{
    /**
     * Cette fonction définit la classe SphereLonLat.
     * @param nbLon : nombre de divisions en longitude
     * @param nbLat : nombre de divisions en latitude
     */
    constructor(nbLon, nbLat)
    {
        // créer le maillage : une nappe rectangulaire
        this.m_Mesh = new Mesh("SphereLonLat");
        let topology = new MeshModuleTopology(this.m_Mesh);
        let num0 = topology.addRectangularSurface(nbLon, nbLat, "sphere %d-%d", true, false);

        // aller du pôle sud au pôle nord, pour définir les sommets
        for (let ilat=0; ilat<nbLat; ilat++) {
            // faire le tour du globe
            for (let ilon=0; ilon<nbLon; ilon++) {
                // récupérer le sommet
                let vertex = this.m_Mesh.getVertex(ilon + ilat*nbLon + num0);
                // calculer ses coordonnées
                let a = ilon / nbLon;
                let b = ilat / (nbLat-1);
                vertex.setCoord(this.getCoord(a, b));
                vertex.setNormal(this.getCoord(a, b));
            }
        }
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
     * Cette méthode retourne les coordonnées 3D d'un point identifié par (a,b)
     * @param a : longitude variant entre 0 et 1 en tournant autour du centre
     * @param b : latitude variant entre 0 (pôle sud) et 1 (pôle nord)
     * @return vec3 contenant les coordonnées
     */
    getCoord(a,b)
    {
        // longitude : 0.5 - a pour décaler 0° au niveau du méridien de greenwich et tourner dans le bon sens
        let lon = (0.5 - a) * 2.0 * Math.PI;
        let coslon = Math.cos(lon);
        let sinlon = Math.sin(lon);

        // latitude
        let lat = (b - 0.5) * Math.PI;
        let coslat = Math.cos(lat);
        let sinlat = Math.sin(lat);

        // conversion sphériques -> cartésiennes
        let x = sinlon * coslat;
        let y = sinlat;
        let z = coslon * coslat;
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
