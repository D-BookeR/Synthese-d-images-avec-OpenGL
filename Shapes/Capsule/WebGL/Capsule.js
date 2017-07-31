// Définition de la classe Capsule

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleTopology.js");


class Capsule
{
    /**
     * Cette fonction définit la classe Capsule.
     * @param spokes_count : nombre de rayons autour du centre
     * @param segments_count : nombre de segments sur chaque rayon
     * @param length : longueur de la partie cylindrique
     */
    constructor(spokes_count, segments_count, length)
    {
        // créer le maillage : deux disques
        this.m_Mesh = new Mesh("Capsule");
        let topology = new MeshModuleTopology(this.m_Mesh);
        let diskS0 = topology.addRevolutionSurface(spokes_count, segments_count, "disqueS %d-%d");
        let diskN0 = topology.addRevolutionSurface(spokes_count, segments_count, "disqueN %d-%d");

        // rajouter des quads entre les deux disques
        for (let ir=0; ir<spokes_count; ir++) {
            // disque sud, tourner dans le sens inverse
            let nvS0 = ((spokes_count-ir)%spokes_count)*segments_count + segments_count + diskS0;
            let vS0 = this.m_Mesh.getVertex(nvS0);
            let nvS1 = ((spokes_count-ir-1)%spokes_count)*segments_count + segments_count + diskS0;
            let vS1 = this.m_Mesh.getVertex(nvS1);
            // disque nord
            let nvN0 = ((ir+0)%spokes_count)*segments_count + segments_count + diskN0;
            let vN0 = this.m_Mesh.getVertex(nvN0);
            let nvN1 = ((ir+1)%spokes_count)*segments_count + segments_count + diskN0;
            let vN1 = this.m_Mesh.getVertex(nvN1);
            this.m_Mesh.addQuad(vS1, vN1, vN0, vS0);
        }

        // définir les vertices des deux disques afin de créer des hémisphères
        for (let ir=0; ir<spokes_count; ir++) {
            for (let is=0; is<segments_count; is++) {
                // vertex sud (attention, inverser la direction)
                let vS = this.m_Mesh.getVertex(ir*segments_count + is + diskS0+1);
                let coordsS = this.getCoord(-ir / spokes_count, -(segments_count-1-is) / segments_count, length);
                vS.setCoord(coordsS);
                let normalS = this.getNormal(-ir / spokes_count, -(segments_count-1-is) / segments_count);
                vS.setNormal(normalS);
                // vertex nord
                let vN = this.m_Mesh.getVertex(ir*segments_count + is + diskN0+1);
                let coordsN = this.getCoord(+ir / spokes_count,+(segments_count-1-is) / segments_count, length);
                vN.setCoord(coordsN);
                let normalN = this.getNormal(+ir / spokes_count,+(segments_count-1-is) / segments_count);
                vN.setNormal(normalN);
            }
        }

        // définir le vertex du pole sud
        let vS = this.m_Mesh.getVertex(diskS0);
        vS.setCoord(this.getCoord(0, -1, length));
        vS.setNormal(this.getNormal(0, -1));

        // définir le vertex du pole nord
        let vN = this.m_Mesh.getVertex(diskN0);
        vN.setCoord(this.getCoord(0, +1, length));
        vN.setNormal(this.getNormal(0, +1));
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
     * @param b : varie entre 0 et 1 : 1=pôle, 0=équateur
     * @param length de la partie cylindrique
     * @return vec3 contenant les coordonnées
     */
    getCoord(a,b, length)
    {
        // angles de ce point
        let longitude = a * 2.0*Math.PI;
        let latitude  = b * 0.5*Math.PI;

        // conversion sphériques -> cartésiennes
        let x = Math.cos(latitude) * Math.sin(longitude);
        let y = Math.sin(latitude);
        let z = Math.cos(latitude) * Math.cos(longitude);

        if (b > 0) {
            y += length;
        } else {
            y -= length;
        }

        return vec3.fromValues(x,y,z);
    }


    /**
     * Cette méthode retourne la normale d'un point identifié par (a,b)
     * @param a : paramètre variant entre 0 et 1 en tournant autour du centre
     * @param b : varie entre 0 et 1 : 1=pôle, 0=équateur
     * @return vec3 contenant les coordonnées du vecteur normal
     */
    getNormal(a,b)
    {
        // angles de ce point
        let longitude = a * 2.0*Math.PI;
        let latitude  = b * 0.5*Math.PI;

        // conversion sphériques -> cartésiennes
        let x = Math.cos(latitude) * Math.sin(longitude);
        let y = Math.sin(latitude);
        let z = Math.cos(latitude) * Math.cos(longitude);
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
