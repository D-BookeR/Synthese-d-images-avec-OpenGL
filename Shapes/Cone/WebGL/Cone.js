// Définition de la classe Cone

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleTopology.js");


// Définition de la classe Cone
class Cone
{
    /**
     * constructeur
     * @param triangle_count : nombre de secteurs (4 pour une pyramide à base carrée, bcp plus pour un cône)
     */
    constructor(triangle_count)
    {
        // créer le maillage : une grille rectangulaire refermée sur elle-même
        this.m_Mesh = new Mesh("Cone");
        let topology = new MeshModuleTopology(this.m_Mesh);
        let num0 = topology.addRectangularSurface(triangle_count, 2, "cone %d-%d", true, false);

        // définir les vertices du cône
        for (let is=0; is<triangle_count; is++) {
            // vertex sur la pointe
            let s0 = this.m_Mesh.getVertex(is + 0*triangle_count + num0);
            s0.setCoord(this.getCoord((is+0.5) / triangle_count, 0.0));

            // vertex sur le bord
            let s1 = this.m_Mesh.getVertex(is + 1*triangle_count + num0);
            s1.setCoord(this.getCoord((is+0.0) / triangle_count, 1.0));
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
     * Cette méthode retourne les coordonnées 3D d'un point identifié par (a,b)
     * @param a : paramètre variant entre 0 et 1 en tournant autour du centre
     * @param b : rayon du point, distance à l'axe Y
     * @return vec3 contenant les coordonnées
     */
    getCoord(a,b)
    {
        // angle de ce point
        let angle = a * 2.0*Math.PI;

        // conversion en coordonnées cylindriques
        let x = b * Math.sin(angle);
        let y = 1.0 - b;
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
