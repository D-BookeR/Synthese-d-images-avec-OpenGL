// Définition de la classe Billboard

Requires("libs/Mesh/Mesh.js");
Requires("libs/Mesh/MeshModuleDrawing.js");
Requires("Texture360");
Requires("Texture360Material");


class Billboard
{
    /**
     * crée un billboard : un rectangle portant une texture360
     * @param texture360
     */
    constructor(texture360)
    {
        // matrices pour stocker les ModelView
        this.m_ModelViewTmp  = mat4.create();
        this.m_ModelViewFixed = mat4.create();

        // créer le maillage
        this.m_Mesh = new Mesh("Billboard");

        // sommets
        let P0 = this.m_Mesh.addVertex("P0");
        P0.setCoord(vec3.fromValues(-0.5, 0.0, 0.0));
        P0.setTexCoord(vec2.fromValues(0.0, 0.0));

        let P1 = this.m_Mesh.addVertex("P1");
        P1.setCoord(vec3.fromValues(+0.5, 0.0, 0.0));
        P1.setTexCoord(vec2.fromValues(1.0, 0.0));

        let P2 = this.m_Mesh.addVertex("P2");
        P2.setCoord(vec3.fromValues(+0.5, 1.0, 0.0));
        P2.setTexCoord(vec2.fromValues(1.0, 1.0));

        let P3 = this.m_Mesh.addVertex("P3");
        P3.setCoord(vec3.fromValues(-0.5, 1.0, 0.0));
        P3.setTexCoord(vec2.fromValues(0.0, 1.0));

        // quadrilatère
        this.m_Mesh.addQuad(P0, P1, P2, P3);

        // créer le matériau du rectangle
        this.m_Material = new Texture360Material(texture360);
        this.m_Material.select(0.0);

        // créer les VBO pour afficher ce maillage
        let renderer = new MeshModuleDrawing(this.m_Mesh);
        this.m_VBOset = renderer.createStripVBOset(this.m_Material, true);
    }


    /**
     * dessin du billboard sur l'écran
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    onDraw(mat4Projection, mat4ModelView)
    {
        // mettre en place le blending
        gl.enable(gl.BLEND);

        /** dessiner l'ombre */

        // rendre le matériau noir et un peu transparent
        this.m_Material.setCoefficients(0.0, 0.7);
        // activer la texture d'angle 0 pour l'ombre (soleil tjrs même position)
        this.m_Material.select(0.0);
        // désactiver le test du depth buffer
        gl.disable(gl.DEPTH_TEST);
        // faire pivoter le billboard à plat par terre
        mat4.rotateX(this.m_ModelViewTmp, mat4ModelView, Utils.radians(-90));
        // réduire la hauteur du billboard
        mat4.scale(this.m_ModelViewTmp, this.m_ModelViewTmp, vec3.fromValues(1.0, 0.7, 1.0));
        // dessiner le rectangle
        this.m_VBOset.onDraw(mat4Projection, this.m_ModelViewTmp);
        // réactiver le test du depth buffer
        gl.enable(gl.DEPTH_TEST);

        /** dessiner le billboard */

        // copier la matrice ModelView avant de la saccager
        mat4.copy(this.m_ModelViewFixed, mat4ModelView);

        // obtenir l'angle de rotation avec modelview => texture à utiliser selon l'angle
        let angle = Math.atan2(this.m_ModelViewFixed[2], this.m_ModelViewFixed[0]);
        // mise de l'angle dans la plage [0, 1[
        angle = angle/(2*Math.PI) + 1.0;
        angle = angle - Math.floor(angle);

        /** variante : le billboard est strictement face à la caméra **/

/*
        // modifier la matrice this.m_ModelViewFixed afin qu'il n'y ait plus aucune rotation
        // mais appliquer une mise à l'échelle
        this.m_ModelViewFixed[ 0] = 1.0;
        this.m_ModelViewFixed[ 1] = 0.0;
        this.m_ModelViewFixed[ 2] = 0.0;
        this.m_ModelViewFixed[ 4] = 0.0;
        this.m_ModelViewFixed[ 5] = 1.0;
        this.m_ModelViewFixed[ 6] = 0.0;
        this.m_ModelViewFixed[ 8] = 0.0;
        this.m_ModelViewFixed[ 9] = 0.0;
        this.m_ModelViewFixed[10] = 1.0;
*/

        /** variante : le billboard reste vertical, mais face à la caméra **/

        this.m_ModelViewFixed[ 0] = 1.0;
        this.m_ModelViewFixed[ 1] = 0.0;
        this.m_ModelViewFixed[ 2] = 0.0;
        this.m_ModelViewFixed[ 8] = 0.0;
        this.m_ModelViewFixed[ 9] = 0.0;
        this.m_ModelViewFixed[10] = 1.0;


        // rendre le matériau normal
        this.m_Material.setCoefficients(1.0, 1.0);
        // activer la bonne texture
        this.m_Material.select(angle);
        // dessiner le rectangle face à la caméra
        this.m_VBOset.onDraw(mat4Projection, this.m_ModelViewFixed);

        // arrêter le blending
        gl.disable(gl.BLEND);
    }


    /**
     * dessin du billboard sur l'écran, variante plus simple
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    onDraw2(mat4Projection, mat4ModelView)
    {
        let mat4ModelViewFixed = mat4.clone(mat4ModelView);
        mat4ModelViewFixed[ 0] = 1.0;
        mat4ModelViewFixed[ 1] = 0.0;
        mat4ModelViewFixed[ 2] = 0.0;
        mat4ModelViewFixed[ 4] = 0.0;
        mat4ModelViewFixed[ 5] = 1.0;
        mat4ModelViewFixed[ 6] = 0.0;
        mat4ModelViewFixed[ 8] = 0.0;
        mat4ModelViewFixed[ 9] = 0.0;
        mat4ModelViewFixed[10] = 1.0;

        // mettre en place le blending
        gl.enable(gl.BLEND);

        // dessiner le rectangle face à la caméra
        this.m_VBOset.onDraw(mat4Projection, mat4ModelViewFixed);

        // arrêter le blending
        gl.disable(gl.BLEND);
     }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Material.destroy();
        this.m_VBOset.destroy();
        this.m_Mesh.destroy();
        super.destroy();
    }
}
