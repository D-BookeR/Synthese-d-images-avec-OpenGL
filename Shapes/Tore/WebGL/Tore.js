// Définition de la classe Tore

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleTopology.js");


class Tore
{
    /**
     * Cette fonction définit la classe Tore.
     * @param nblon : nombre de divisions en longitude
     * @param nblat : nombre de divisions en latitude
     * @param r : rayon du tube, le grand rayon est 1.0
     */
    constructor(nblon, nblat, r)
    {
        // créer le maillage : une grille rectangulaire refermée sur elle-même
        this.m_Mesh = new Mesh("Tore");
        let topology = new MeshModuleTopology(this.m_Mesh);
        let num0 = topology.addRectangularSurface(nblon, nblat, "tore %d-%d", true, true);

        // parcourir les sommets pour définir les coordonnées
        for (let ilat=0; ilat<nblat; ilat++) {
            for (let ilon=0; ilon<nblon; ilon++) {
                // récupérer le sommet concerné
                let num = ilon + ilat*nblon + num0;
                let vertex = this.m_Mesh.getVertex(num);
                // définir les coordonnées 3D du point
                let a = ilon / nblon;
                let b = ilat / nblat;
                vertex.setCoord(this.getCoord(r, a, b));
                vertex.setNormal(this.getNormal(r, a, b));
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
     * @param r : rayon du tube, le grand rayon est 1.0
     * @param a : longitude variant entre 0 et 1 en tournant autour du centre
     * @param b : latitude variant entre 0 (pôle sud) et 1 (pôle nord)
     * @return vec3 contenant les coordonnées
     */
    getCoord(r, a, b)
    {
        // longitude  attention, on tourne dans le sens ouest->est
        let lon = -a * 2.0 * Math.PI;
        let coslon = Math.cos(lon);
        let sinlon = Math.sin(lon);

        // latitude
        let lat = b * 2.0 * Math.PI;
        let coslat = Math.cos(lat);
        let sinlat = Math.sin(lat);

        // conversion toriques -> cartésiennes
        let R = 1.0 + r * coslat;
        let x = R * sinlon;
        let y = r * sinlat;
        let z = R * coslon;
        return vec3.fromValues(x,y,z);
    }


    /**
     * Cette méthode retourne la normale 3D au point identifié par (a,b)
     * @param r : rayon du tube, le grand rayon est 1.0
     * @param a : longitude variant entre 0 et 1 en tournant autour du centre
     * @param b : latitude variant entre 0 (pôle sud) et 1 (pôle nord)
     * @return vec3 contenant les coordonnées du vecteur normal
     */
    getNormal(r, a, b)
    {
        // longitude  attention, on tourne dans le sens ouest->est
        let lon = -a * 2.0 * Math.PI;
        let coslon = Math.cos(lon);
        let sinlon = Math.sin(lon);

        // latitude
        let lat = b * 2.0 * Math.PI;
        let coslat = Math.cos(lat);
        let sinlat = Math.sin(lat);

        // conversion en coordonnées cartésiennes
        let nx = sinlon * coslat;
        let ny = sinlat;
        let nz = coslon * coslat;
        return vec3.fromValues(nx,ny,nz);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Mesh.destroy();
    }
}
