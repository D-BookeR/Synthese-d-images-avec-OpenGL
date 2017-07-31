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
     * @param position : vec3 donnant les coordonnées du bas du panneau
     * @param tx : float donnant la taille horizontale
     * @param ty : float donnant la taille verticale
     */
    constructor(texture360, position, tx=1.0, ty=1.0)
    {
        // paramètres
        this.m_Position = position;
        this.m_SizeX = tx;
        this.m_SizeY = ty;

        // matrices pour stocker les ModelView
        this.m_ModelViewOrig = mat4.create();
        this.m_ModelViewFixed = mat4.create();

        // angle sous lequel on voit le billboard
        this.m_Angle = 0.0;

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
        this.m_Material.select(this.m_Angle);

        // créer les VBO pour afficher ce maillage
        let renderer = new MeshModuleDrawing(this.m_Mesh);
        this.m_VBOset = renderer.createStripVBOset(this.m_Material, true);
    }


    /**
     * recopie ModelView dans les matrices locales
     * @param mat4ModelView
     */
    setModelView(mat4ModelView)
    {
        // d'abord placer le billboard à sa place
        mat4.translate(this.m_ModelViewOrig, mat4ModelView, this.m_Position);
        mat4.copy(this.m_ModelViewFixed, this.m_ModelViewOrig);

        // la distance est dans modelview.translation.z
        this.m_Distance = this.m_ModelViewFixed[14];

        // obtenir l'angle de rotation avec modelview => texture à utiliser selon l'angle
        let angle = Math.atan2(this.m_ModelViewFixed[2], this.m_ModelViewFixed[0]);
        // mise de l'angle dans la plage [0, 1[
        angle = angle/(2*Math.PI) + 1.0;
        this.m_Angle = angle - Math.floor(angle);

        /** variante : le billboard est strictement face à la caméra **/

/*
        // modifier la matrice this.m_ModelViewFixed afin qu'il n'y ait plus aucune rotation
        // mais appliquer une mise à l'échelle
        this.m_ModelViewFixed[ 0] = this.m_SizeX;
        this.m_ModelViewFixed[ 1] = 0.0;
        this.m_ModelViewFixed[ 2] = 0.0;
        this.m_ModelViewFixed[ 4] = 0.0;
        this.m_ModelViewFixed[ 5] = this.m_SizeY;
        this.m_ModelViewFixed[ 6] = 0.0;
        this.m_ModelViewFixed[ 8] = 0.0;
        this.m_ModelViewFixed[ 9] = 0.0;
        this.m_ModelViewFixed[10] = 1.0;
*/

        /** variante : le billboard reste vertical, mais face à la caméra **/

        this.m_ModelViewFixed[ 0] = this.m_SizeX;
        this.m_ModelViewFixed[ 1] = 0.0;
        this.m_ModelViewFixed[ 2] = 0.0;
        this.m_ModelViewFixed[ 5] *= this.m_SizeY;
        this.m_ModelViewFixed[ 8] = 0.0;
        this.m_ModelViewFixed[ 9] = 0.0;
        this.m_ModelViewFixed[10] = 1.0;
    }


    /**
     * retourne la distance du billboard à l'œil
     * @return distance
     */
    getDistance()
    {
        return this.m_Distance;
    }


    /**
     * compare la distance entre deux billboard afin de pouvoir classer une liste
     * du plus loin au plus proche
     * @param a : billboard à comparer
     * @param b : billboard à comparer
     * @return la valeur qu'il faut pour le tri
     */
    DistanceCompare(a, b)
    {
        return a.m_Distance > b.m_Distance;
    }


    /**
     * dessin du billboard sur l'écran
     * @param mat4Projection : matrice de projection
     */
    onDraw(mat4Projection)
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
        let mat4ModelView = mat4.create();
        mat4.rotateX(mat4ModelView, this.m_ModelViewOrig, Utils.radians(-90));
        // réduire la hauteur du billboard
        mat4.scale(mat4ModelView, mat4ModelView, vec3.fromValues(this.m_SizeX, 0.7*this.m_SizeY, 1.0));
        // dessiner le rectangle
        this.m_VBOset.onDraw(mat4Projection, mat4ModelView);
        // réactiver le test du depth buffer
        gl.enable(gl.DEPTH_TEST);

        /** dessiner le billboard */

        // rendre le matériau normal
        this.m_Material.setCoefficients(1.0, 1.0);
        // activer la bonne texture
        this.m_Material.select(this.m_Angle);
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
