// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Misc/TurnTableScene.js");
Requires("libs/Misc/MeshObjectFromObj.js");
Requires("libs/Misc/GridXZ.js");
Requires("libs/Misc/AxesXYZ.js");
Requires("Mirror");


class Scene extends TurnTableScene
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(false);

        // créer les objets à dessiner
        this.m_GridXZ   = new GridXZ(3);
        this.m_TriedreXYZ = new AxesXYZ(1,1,4);
        this.m_Lorry  = new MeshObjectFromObj("data/models/Camion", "camion.obj", "camion.mtl", 2.0);
        this.m_Mirror = new Mirror();

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.enable(gl.CULL_FACE);

        // couleur du fond
        gl.clearColor(0.7, 0.7, 0.7, 0.0);

        // paramètres de la vue
        this.m_CameraAngleAzimut = -17.0;
        this.m_CameraAngleElevation = 8.0;
        vec3.set(this.m_CameraPivot, 0.9, -0.6, -0.6);
        this.m_CameraDistance = 25;
    }


    /**
     * appelée quand la taille de la vue OpenGL change
     * @param width : largeur en nombre de pixels de la fenêtre
     * @param height : hauteur en nombre de pixels de la fenêtre
     */
    onSurfaceChanged(width, height)
    {
        // appeler la méthode de la superclasse
        super.onSurfaceChanged(width, height);

        // matrice de projection (champ de vision)
        mat4.perspective(this.m_Mat4Projection, Utils.radians(12.0), width / height, 1.0, 100.0);
    }


    /**
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4View : matrice de vue
     */
    onDraw(mat4Projection, mat4View)
    {
        // matrice temporaire
        let mat4ModelView = mat4.create();

        // dessiner le trièdre
        this.m_TriedreXYZ.onDraw(mat4Projection, mat4View);

        // décaler tous les objets en z positif
        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(0.0, 0.0, 4.0));

        // dessiner la grille
        this.m_GridXZ.onDraw(mat4Projection, mat4ModelView);

        // dessiner le camion
        mat4.translate(mat4ModelView, mat4ModelView, vec3.fromValues(0.0, 0.0, 0.0));        // ESSAYEZ DE METTRE -7 POUR z
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(120.0 * Math.cos(Utils.Time)));
        this.m_Lorry.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Dessine l'image courante
     */
    onDrawFrame()
    {
        /*** PREPARATION DES MATRICES ***/

        // construire la matrice caméra <- scène (fait passer de la scène à la caméra)
        let mat4View = this.getModelView();

        // transformation du miroir par rapport à la scène
        let mat4ModelMirror = mat4.create();
        mat4.translate(mat4ModelMirror, mat4ModelMirror, vec3.fromValues(0.0, 0.0, -1.0));  // ESSAYEZ D'AUTRES VALEURS
        //mat4.rotateY(mat4ModelMirror, mat4ModelMirror, Utils.radians(20.0));              // ESSAYEZ D'AUTRES VALEURS

        // inverse de la matrice qui positionne le miroir dans la scène
        let mat4ModelMirrorInv = mat4.create();
        mat4.invert(mat4ModelMirrorInv, mat4ModelMirror);

        // matrice pour positionner le miroir par rapport à la caméra
        let mat4ModelViewMirror = mat4.create();
        mat4.multiply(mat4ModelViewMirror, mat4View, mat4ModelMirror);

        // matrice pour dessiner le reflet de la scène par rapport à la caméra
        let mat4ViewReflected = mat4.create();
        mat4.scale(mat4ViewReflected, mat4ViewReflected, vec3.fromValues(1,1,-1));
        mat4.multiply(mat4ViewReflected, mat4ViewReflected, mat4ModelMirrorInv);
        mat4.multiply(mat4ViewReflected, mat4ModelViewMirror, mat4ViewReflected);

        // plan de coupe qui élimine les fragments devant le miroir (en coordonnées locales du miroir)
        let MirrorPlane = vec4.fromValues(0, 0, -1, 0);

        // transformer le plan de coupe en coordonnées caméra
        let mat4ModelViewMirrorInverse = mat4.create();
        mat4.transpose(mat4ModelViewMirrorInverse, mat4ModelViewMirror);
        mat4.invert(mat4ModelViewMirrorInverse, mat4ModelViewMirrorInverse);
        vec4.transformMat4(MirrorPlane, MirrorPlane, mat4ModelViewMirrorInverse);


        /*** DESSIN ***/

        // effacer l'écran
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        /** étape 1 : la découpe du stencil par le miroir **/

        // configurer le stencil en mode "mettre le buffer à 1"
        gl.enable(gl.STENCIL_TEST);                         // activation du Stencil Buffer
        gl.stencilFunc(gl.ALWAYS, 1, 0xFF);                 // accepter tous les fragments, la constante = 1
        gl.stencilOp(gl.KEEP, gl.KEEP, gl.REPLACE);         // mettre la constante, c'est à dire 1 si le depth test passe
        gl.clearStencil(0);                                 // mettre des 0 partout quand on vide le stencil
        gl.clear(gl.STENCIL_BUFFER_BIT);                    // vider le stencil

        // bloquer les dessins sur l'écran et le depth buffer
        gl.colorMask(false, false, false, false);           // ne pas dessiner de pixels
        gl.depthMask(false);                                // ne pas écrire sur le depth buffer

        // dessiner le miroir (il n'apparaît pas sur l'écran)
        this.m_Mirror.onDraw(this.m_Mat4Projection, mat4ModelViewMirror);

        // réautoriser les dessins sur l'écran et le depth buffer
        gl.colorMask(true, true, true, true);               // réactiver le dessin sur l'écran
        gl.depthMask(true);                                 // réactiver l'écriture sur le depth buffer

        /** étape 2 : le reflet à travers la découpe **/

        // configurer le stencil en mode "ne dessiner que si le stencil est égal à 1"
        gl.stencilFunc(gl.EQUAL, 1, 0xFF);                  // accepter les fragments pour lesquels le stencil vaut 1
        gl.stencilOp(gl.KEEP, gl.KEEP, gl.KEEP);            // ne pas changer le stencil

        // fournir le plan de coupe au camion
//        this.m_Lorry.setClipPlane(true, MirrorPlane);

        // dessiner les objets inversés, avec la matrice de la scène reflétée
        gl.frontFace(gl.CW);
        this.onDraw(this.m_Mat4Projection, mat4ViewReflected);
        gl.frontFace(gl.CCW);

        // enlever le plan de coupe sans recompiler le shader
//        this.m_Lorry.setClipPlane(true);

        // ne plus utiliser le stencil
        gl.disable(gl.STENCIL_TEST);

        /** étape 3 : la surface du miroir par dessus le reflet */

        // superposer le miroir quasi-transparent
        gl.enable(gl.BLEND);
        gl.blendFunc(gl.ONE, gl.ONE);
        this.m_Mirror.onDraw(this.m_Mat4Projection, mat4ModelViewMirror);
        gl.disable(gl.BLEND);

        /** étape 4 : la scène réelle **/

        // dessiner la scène normale sans le miroir
        this.onDraw(this.m_Mat4Projection, mat4View);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Mirror.destroy();
        this.m_Lorry.destroy();
        this.m_TriedreXYZ.destroy();
        this.m_GridXZ.destroy();
        super.destroy();
    }
}
