// Définition de la classe ConeFrustum

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleTopology.js");


class ConeFrustum
{
    /**
     * Cette fonction définit la classe ConeFrustum.
     * @param spokes_count : nombre de secteurs
     * @param radius_bottom : rayon en bas (pour y=-1)
     * @param radius_top : rayon en haut (pour y=+1)
     */
    constructor(spokes_count, radius_bottom, radius_top)
    {
        // créer le maillage : une grille et deux disques
        this.m_Mesh = new Mesh("ConeFrustum");
        let topology = new MeshModuleTopology(this.m_Mesh);
        let diskS0 = topology.addRevolutionSurface(spokes_count, 1, "disqueS %d");
        let grid0  = topology.addRectangularSurface(spokes_count, 2, "tronc %d-%d", true, false);
        let diskN0 = topology.addRevolutionSurface(spokes_count, 1, "disqueN %d");

        // définir les vertices du tube
        for (let is=0; is<spokes_count; is++) {
            let a = is / spokes_count;
            // vertex sud
            let vS = this.m_Mesh.getVertex(is + 1*spokes_count + grid0);
            vS.setCoord(this.getCoord(radius_bottom, radius_top, a, -1.0));
            // vertex nord
            let vN = this.m_Mesh.getVertex(is + 0*spokes_count + grid0);
            vN.setCoord(this.getCoord(radius_bottom, radius_top, a, +1.0));
        }

        // définir les vertices des disques
        for (let is=0; is<spokes_count; is++) {
            let a = is / spokes_count;
            // vertex sud (attention, inverser la direction)
            let vS = this.m_Mesh.getVertex(is + diskS0+1);
            vS.setCoord(this.getCoord(radius_bottom, radius_top, -a, -1.0));
            // vertex nord
            let vN = this.m_Mesh.getVertex(is + diskN0+1);
            vN.setCoord(this.getCoord(radius_bottom, radius_top, +a, +1.0));
        }

        // définir le vertex du pole sud
        let vS = this.m_Mesh.getVertex(diskS0);
        vS.setCoord(vec3.fromValues(0.0,-1.0,0.0));

        // définir le vertex du pole nord
        let vN = this.m_Mesh.getVertex(diskN0);
        vN.setCoord(vec3.fromValues(0.0,+1.0,0.0));

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
     * Cette méthode retourne les coordonnées 3D d'un point identifié par (a,b)
     * @param radius_bottom : rayon en bas, quand b vaut -1
     * @param radius_top : rayon en haut, quand b vaut +1
     * @param a : paramètre variant entre 0 et 1 en tournant autour du centre
     * @param b : hauteur considérée, entre -1 et +1
     * @return vec3 contenant les coordonnées
     */
    getCoord(radius_bottom, radius_top, a, b)
    {
        // angle de ce point
        let angle = a * 2.0*Math.PI;

        // rayon de ce point
        let k = (b + 1.0) / 2.0;
        let r = k*radius_top + (1.0-k)*radius_bottom;

        // conversion cylindriques -> cartésiennes
        let x = r * Math.sin(angle);
        let y = b;
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
