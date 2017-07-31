// Définition de la classe Seashell

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleTopology.js");


// Définition de la classe Seashell
class Seashell
{
    /**
     * constructeur
     * @param turns_count : nombre de tours
     * @param steps : nombre de divisions en longitude et en latitude
     */
    constructor(turns_count, steps)
    {
        // paramètres
        const r = 0.35;

        // créer le maillage : une nappe hexagonale partiellement refermée sur elle-même
        this.m_Mesh = new Mesh("Seashell");
        let topology = new MeshModuleTopology(this.m_Mesh);
        let num0 = topology.addHexagonalSurface(steps*turns_count, steps, "seashell %d-%d", false, true);

        // parcourir les sommets pour définir les coordonnées
        for (let ilat=0; ilat<steps; ilat++) {
            for (let ilon=0; ilon<steps*turns_count; ilon++) {
                // récupérer le sommet concerné
                let num = ilon + ilat*steps*turns_count + num0;
                let vertex = this.m_Mesh.getVertex(num);
                // définir les coordonnées 3D du point
                let a = (ilon - 0.5*(ilat%2)) / steps;
                let b = ilat / steps;
                vertex.setCoord(this.getCoord(r, a, b));
                // définir la couleur du point
                let c = (ilon - 0.5*(ilat%2)) / (steps*turns_count);
                vertex.setColor(Utils.hsv2rgb(vec3.fromValues(0.9-c*0.2, 0.8, 0.7)));
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
     * Cette méthode retourne les coordonnées 3D d'un point identifié par (r,a,b)
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

        // évolution du petit rayon selon a
        r = r * a * 0.5;

        // conversion toriques -> cartésiennes
        let R = r * (1.0 + coslat);
        let x = R * sinlon;
        let y = r * sinlat + a*a*0.2 - 2.0;
        let z = R * coslon;
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
