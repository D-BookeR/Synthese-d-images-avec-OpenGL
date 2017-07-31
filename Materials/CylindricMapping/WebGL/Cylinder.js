// Définition de la classe Cylinder

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleTopology.js");
Requires("TransparentMaterial");


class Cylinder
{
/**
     * constructeur
     * @param spokes_count : nombre de secteurs
     * @param texture à appliquer sur l'objet
     */
    constructor(spokes_count, texture)
    {
        // créer le matériau transparent
        this.m_Material = new TransparentMaterial(texture, 0.2);

        // créer le maillage : une nappe rectangulaire non refermée sur elle-même
        this.m_Mesh = new Mesh("Cylinder");
        let topology = new MeshModuleTopology(this.m_Mesh);
        this.m_Num0 = topology.addRectangularSurface(spokes_count+1, 2, "cylindre %d-%d", false, false);
        this.m_SpokesCount = spokes_count;

        // définir les vertices du tube
        for (let is=0; is<=spokes_count; is++) {
            let a = is / spokes_count;

            // vertex sud
            let vS = this.m_Mesh.getVertex(is + 1*(spokes_count+1) + this.m_Num0);
            vS.setCoord(this.getVertex(a, -1));
            vS.setNormal(this.getNormal(a));
            vS.setTexCoord(vec2.fromValues(a, 0));

            // vertex nord
            let vN = this.m_Mesh.getVertex(is + 0*(spokes_count+1) + this.m_Num0);
            vN.setCoord(this.getVertex(a, +1));
            vN.setNormal(this.getNormal(a));
            vN.setTexCoord(vec2.fromValues(a, 1));
        }

        // créer le VBOset
        let renderer = new MeshModuleDrawing(this.m_Mesh);
        this.m_VBOset = renderer.createStripVBOset(this.m_Material, true);
    }


    /**
     * Cette méthode retourne les coordonnées 3D d'un point identifié par (a,b)
     * @param a : paramètre variant entre 0 et 1 en tournant autour du centre
     * @param b : hauteur sur l'axe y
     * @return vec3 contenant les coordonnées
     */
    getVertex(a,b)
    {
        // angle de ce point
        let angle = (a * 2.0 - 1.0) * Math.PI;

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
        let angle = (a * 2.0 - 1.0) * Math.PI;

        let nx = Math.sin(angle);
        let ny = 0.0;
        let nz = Math.cos(angle);
        return vec3.fromValues(nx,ny,nz);
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
     * Modifie l'extension verticale du cylindre
     * @param minY hauteur de la base
     * @param maxY hauteur du sommet
     */
    updateMinMaxY(minY, maxY)
    {
        for (let is=0; is<=this.m_SpokesCount; is++) {

            // vertex sud
            let vS = this.m_Mesh.getVertex(is + 1*(this.m_SpokesCount+1) + this.m_Num0);
            vS.getCoord()[1] = minY;

            // vertex nord
            let vN = this.m_Mesh.getVertex(is + 0*(this.m_SpokesCount+1) + this.m_Num0);
            vN.getCoord()[1] = maxY;
        }

        // recréer le VBOset
        this.m_VBOset.destroy();
        let renderer = new MeshModuleDrawing(this.m_Mesh);
        this.m_VBOset = renderer.createStripVBOset(this.m_Material, true);
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
