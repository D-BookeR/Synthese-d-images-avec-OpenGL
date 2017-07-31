// Définition de la classe Disk

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleTopology.js");


class Disk
{
    /**
     * constructeur
     * @param triangle_count : nombre de secteurs
     */
    constructor(triangle_count)
    {
        // créer le maillage
        this.m_Mesh = new Mesh("Disk");

        // ajouter une nappe circulaire
        let topology = new MeshModuleTopology(this.m_Mesh);
        let disk0 = topology.addRevolutionSurface(triangle_count, 1, "disque %d-%d");

        // définir le vertex central
        let center = this.m_Mesh.getVertex(disk0);
        center.setCoord(this.getCoord(0.0, 0.0));
        center.setNormal(vec3.fromValues(0.0,+1.0,0.0));

        // définir les vertices du bord
        for (let is=0; is<triangle_count; is++) {
            let num = is + disk0+1;
            let s = this.m_Mesh.getVertex(num);
            // calculer les coordonnées du point
            s.setCoord(this.getCoord(is / triangle_count, +1.0));
            s.setNormal(vec3.fromValues(0.0,+1.0,0.0));
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
     * @param a : paramètre variant entre 0 et 1 en tournant autour du centre
     * @param b : rayon du point, distance au centre
     * @return vec3 contenant les coordonnées
     */
    getCoord(a,b)
    {
        // angle de ce point
        let angle = a * 2.0*Math.PI;

        // conversion en coordonnées cartésiennes
        let x = b * Math.sin(angle);
        let y = 0.0;
        let z = b * Math.cos(angle);
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
