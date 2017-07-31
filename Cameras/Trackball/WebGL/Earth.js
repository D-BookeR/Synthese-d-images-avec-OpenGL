// Définition de la classe Earth

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleTopology.js");
Requires("libs/Mesh/MeshModuleDrawing.js");
Requires("libs/Material/DeferredShadingMaterial.js");


class Earth
{
    /**
     * Classe Earth = sphere avec une texture
     * @param nbLon : nombre de divisions en longitude
     * @param nbLat : nombre de divisions en latitude
     */
    constructor(nbLon, nbLat)
    {
        // créer le maillage : une nappe rectangulaire
        this.m_Mesh = new Mesh("Earth");
        let topology = new MeshModuleTopology(this.m_Mesh);
        let num0 = topology.addRectangularSurface(nbLon+1, nbLat, "sphere %d-%d", true, false);

        // aller du pôle sud au pôle nord, pour définir les sommets
        for (let ilat=0; ilat<nbLat; ilat++) {
            // faire le tour du globe
            for (let ilon=0; ilon<=nbLon; ilon++) {
                // récupérer le sommet
                let vertex = this.m_Mesh.getVertex(ilon + ilat*(nbLon+1) + num0);
                // calculer ses coordonnées
                let a = ilon / nbLon;
                let b = ilat / (nbLat-1);
                vertex.setCoord(this.getVertex(a, b));
                vertex.setNormal(this.getVertex(a, b));
                vertex.setTexCoord(vec2.fromValues(1-a,b));
            }
        }

        // définir le matériau de l'objet
        let diffuse = "data/textures/earth/earth_map.jpg";
        let Ks = vec3.fromValues(0.9, 0.9, 0.9);
        let Ns = 128;
        this.m_Material = new DeferredShadingMaterial(diffuse, Ks, Ns);

        // créer le VBOset de dessin
        let renderer = new MeshModuleDrawing(this.m_Mesh);
        this.m_VBOset = renderer.createStripVBOset(this.m_Material, true);
    }


    /**
     * Cette méthode retourne les coordonnées 3D d'un point identifié par (a,b)
     * @param a : longitude variant entre 0 et 1 en tournant autour du centre
     * @param b : latitude variant entre 0 (pôle sud) et 1 (pôle nord)
     * @return vec3 contenant les coordonnées
     */
    getVertex(a,b)
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


    onDraw(mat4Projection, mat4ModelView)
    {
        this.m_VBOset.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Material.destroy();
        this.m_Mesh.destroy();
        this.m_VBOset.destroy();
    }
}
