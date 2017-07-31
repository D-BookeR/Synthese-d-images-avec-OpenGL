// Définition de la classe Sphere

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleTopology.js");
Requires("TransparentMaterial");


class Sphere
{
    /**
     * Cette fonction définit la classe Sphere.
     * @param nbLon : nombre de divisions en longitude
     * @param nbLat : nombre de divisions en latitude
     * @param texture à appliquer sur l'objet
     */
    constructor(nbLon, nbLat, texture)
    {
        // créer le matériau
        this.m_Material = new TransparentMaterial(texture, 0.2);

        // créer le maillage : une nappe rectangulaire non refermée sur elle-même
        this.m_Mesh = new Mesh("Sphere");
        let topology = new MeshModuleTopology(this.m_Mesh);
        let num0 = topology.addRectangularSurface(nbLon+1, nbLat, "sphere %d-%d", false, false);

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
                vertex.setTexCoord(vec2.fromValues(a,b));
            }
        }

        // créer le VBOset
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
        // longitude
        let lon = (a * 2.0 - 1.0) * Math.PI;
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
     * affecte la texture DepthMap du shader
     * @param far : depth buffer qui limite les dessins arrière de ce matériau
     * @param near : depth buffer qui limite les dessins avant de ce matériau
     */
    setDepthMaps(far, near)
    {
        this.m_Material.setDepthMaps(far, near);
    }


    /**
     * spécifie la taille de la fenêtre
     * @param width : largeur en nombre de pixels de la fenêtre
     * @param height : hauteur en nombre de pixels de la fenêtre
     */
    setWindowDimensions(width, height)
    {
        this.m_Material.setWindowDimensions(width, height);
    }


    /**
     * dessine l'objet sur l'écran
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    onDraw(mat4Projection, mat4ModelView)
    {
        // dessiner le VBO s'il est prêt
        if (this.m_VBOset != null) {
            this.m_VBOset.onDraw(mat4Projection, mat4ModelView);
        }
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Mesh.destroy();
        this.m_Material.destroy();
        this.m_VBOset.destroy();
    }
}
