// Définition de la classe Ground

// superclasses et classes nécessaires
Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleTopology.js");
Requires("libs/Mesh/MeshModuleDrawing.js");


class Ground
{
    /**
     * Cette fonction définit la classe Ground.
     * @param points_count : nombre de points à créer en X et en Z
     * @param material : matériau déformant à employer
     */
    constructor(points_count, material)
    {
        // test sur la taille du terrain
        // WebGL pose une grosse limite sur les paramètres de gl.drawElements : des ushort uniquement, pas de uint
        if (points_count > 256) {
            console.log("Ground : nombre de points trop grand, pas plus de 256");
            points_count = 256;
        }

        // créer le maillage : une grille hexagonale
        this.m_Mesh = new Mesh("Ground");
        let topology = new MeshModuleTopology(this.m_Mesh);
        let num0 = topology.addHexagonalSurface(points_count, points_count, "terrain %d-%d", false, false);

        // parcourir les sommets pour définir leurs coordonnées
        for (let ix=0; ix<points_count; ix++) {
            for (let iz=0; iz<points_count; iz++) {
                // numéro du vertex
                let num = ix + iz*points_count + num0;
                // récupérer le sommet concerné
                let vertex = this.m_Mesh.getVertex(num);
                // définir les coordonnées 3D du point
                vertex.setCoord(this.getCoord(points_count, ix, iz));
                vertex.setTexCoord(this.getTexCoord(points_count, ix, iz));
            }
        }

        // créer le VBOset pour dessiner cet objet (attention, peut être très lent)
        let renderer = new MeshModuleDrawing(this.m_Mesh);
        this.m_VBOset = renderer.createStripVBOset(material);
    }


    /**
     * Cette méthode retourne les coordonnées 3D d'un point identifié par (ix, iz)
     * @param points_count : nombre de points en X et en Z
     * @param ix : numéro du point en X
     * @param iz : numéro du point en Z
     * @return vec3 contenant les coordonnées
     */
    getCoord(points_count, ix, iz)
    {
        // le terrain est légèrement rectangulaire à cause de la maille
        let K = Math.sqrt(3.0)/2.0;

        // décalage en X selon la parité de iz
        let offset_x = (iz%2) * -0.5;

        // coordonnées du sommet
        let x = (ix+offset_x) / points_count - 0.5;
        let y = 0.0;
        let z = ((iz+0.0) / points_count - 0.5) * K;
        return vec3.fromValues(x,y,z);
    }


    /**
     * Cette méthode retourne les coordonnées de texture d'un point identifié par (ix, iz)
     * @param points_count : nombre de points en X et en Z
     * @param ix : numéro du point en X
     * @param iz : numéro du point en Z
     * @return vec3 contenant les coordonnées
     */
    getTexCoord(points_count, ix, iz)
    {
        // décalage en X selon la parité de iz
        let offset_x = (iz%2) * -0.5;

        let u = (ix+offset_x)/points_count;
        let v = 1.0 - (iz+0.0)/points_count;
        return vec2.fromValues(u, v);
    }


    /**
     * dessine l'objet sur l'écran
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    onDraw(mat4Projection, mat4ModelView)
    {
        // dessiner le maillage
        this.m_VBOset.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_VBOset.destroy();
        this.m_Mesh.destroy();
    }
}
