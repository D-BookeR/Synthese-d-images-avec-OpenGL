// Définition de la classe Cylinder

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleTopology.js");


class Cylinder
{
/**
     * constructeur
     * @param spokes_count : nombre de secteurs
     */
    constructor(spokes_count)
    {
        // créer le maillage : une grille rectangulaire et deux disques
        this.m_Mesh = new Mesh("Cylinder");
        let topology = new MeshModuleTopology(this.m_Mesh);
        let diskS0 = topology.addRevolutionSurface(spokes_count, 1, "disqueS %d");
        let cylinder0    = topology.addRectangularSurface(spokes_count, 2, "cylindre %d-%d", true, false);
        let diskN0 = topology.addRevolutionSurface(spokes_count, 1, "disqueN %d");

        // définir les vertices du tube
        for (let is=0; is<spokes_count; is++) {
            // vertex sud
            let vS = this.m_Mesh.getVertex(is + 1*spokes_count + cylinder0);
            vS.setCoord(this.getCoord(is / spokes_count, -1.0));
            vS.setNormal(this.getNormal(is / spokes_count));
            // vertex nord
            let vN = this.m_Mesh.getVertex(is + 0*spokes_count + cylinder0);
            vN.setCoord(this.getCoord(is / spokes_count, +1.0));
            vN.setNormal(this.getNormal(is / spokes_count));
        }

        // définir les vertices des disques
        for (let is=0; is<spokes_count; is++) {
            // vertex sud (attention, inverser la direction)
            let vS = this.m_Mesh.getVertex(is + diskS0+1);
            vS.setCoord(this.getCoord(-is / spokes_count, -1.0));
            vS.setNormal(vec3.fromValues( 0.0,-1.0,0.0));
            // vertex nord
            let vN = this.m_Mesh.getVertex(is + diskN0+1);
            vN.setCoord(this.getCoord(+is / spokes_count, +1.0));
            vN.setNormal(vec3.fromValues( 0.0,+1.0,0.0));
        }

        // définir le vertex du pole sud
        let vS = this.m_Mesh.getVertex(diskS0);
        vS.setCoord(vec3.fromValues(0.0,-1.0,0.0));
        vS.setNormal(vec3.fromValues(0.0,-1.0,0.0));

        // définir le vertex du pole nord
        let vN = this.m_Mesh.getVertex(diskN0);
        vN.setCoord(vec3.fromValues(0.0,+1.0,0.0));
        vN.setNormal(vec3.fromValues(0.0,+1.0,0.0));
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
     * @param b : hauteur sur l'axe y
     * @return vec3 contenant les coordonnées
     */
    getCoord(a,b)
    {
        // angle de ce point
        let angle = a * 2.0*Math.PI;

        // conversion cylindriques -> cartésiennes
        let x = Math.sin(angle);
        let y = b;
        let z = Math.cos(angle);
        return vec3.fromValues(x,y,z);
    }


    /**
     * Cette méthode retourne la normale 3D au point identifié par (a,*)
     * @param a : paramètre variant entre 0 et 1 en tournant autour du centre, b n'est pas nécessaire
     * @return vec3 contenant les coordonnées du vecteur normal
     */
    getNormal(a)
    {
        // angle de ce point
        let angle = a * 2.0*Math.PI;

        let nx = Math.sin(angle);
        let ny = 0.0;
        let nz = Math.cos(angle);
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
