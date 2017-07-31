// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Misc/TurnTableScene.js");
Requires("libs/Material/OmniLight.js");
Requires("WaterMaterial");
Requires("Water");
Requires("Island");
Requires("Ground");


class Scene extends TurnTableScene
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(true);

        // créer le matériau de l'eau
        this.m_WaterMaterial = new WaterMaterial();

        // créer les objets à dessiner
        let depth = -0.03;
        let scale = 4.0;
        this.m_Island = new Island(120, scale, 0.6, scale * depth);
        this.m_Ground = new Ground(scale * (depth * 0.9));
        this.m_Water = new Water(this.m_WaterMaterial);

        // définir une lampe ambiante
        this.m_Light0 = new Light();
        this.m_Light0.setColor(vec3.fromValues(0.5, 0.5, 0.5));
        this.addLight(this.m_Light0);

        // définir une lampe directionnelle
        this.m_Light1 = new OmniLight();
        this.m_Light1.setPosition(vec4.fromValues(1.0, 2.0, 1.0, 0.0));
        this.m_Light1.setColor(vec3.fromValues(1.0, 1.0, 1.0));
        this.addLight(this.m_Light1);

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.enable(gl.CULL_FACE);

        // couleur du fond
        gl.clearColor(0.7, 1.0, 1.0, 0.0);

        // paramètres de la vue
        this.m_CameraAngleAzimut = -10.0;
        this.m_CameraAngleElevation = 5.5;
        vec3.set(this.m_CameraPivot, 0.0, -0.1, 0.0);
        this.m_CameraDistance = 6;

        // FBOs pour mémoriser le reflet et la vue du fond et les combiner dans le WaterMaterial
        this.m_FBOreflection = null;
        this.m_FBObackground = null;

        // application de la brume de distance
        this.m_FBOfog = null;
        this.m_Fog = new Fog(vec4.fromValues(0.7, 0.9, 1.0, 1.0), 40.0);
    }


    /**
     * appelée quand la taille de la vue OpenGL change
     * @param width : largeur en nombre de pixels de la fenêtre
     * @param height : hauteur en nombre de pixels de la fenêtre
     */
    onSurfaceChanged(width, height)
    {
        // échelle des FBO par rapport à l'écran
        let antialias = 1;

        // appeler la méthode de la superclasse
        super.onSurfaceChanged(width, height, antialias);

        // matrice de projection (champ de vision)
        mat4.perspective(this.m_Mat4Projection, Utils.radians(12.0), width / height, 1.0, 100.0);

        // créer un FBO pour stocker la vue du reflet
        if (this.m_FBOreflection != null) this.m_FBOreflection.destroy();
        this.m_FBOreflection = new FrameBufferObject(width*antialias, height*antialias, gl.TEXTURE_2D, gl.RENDERBUFFER);

        // créer un FBO pour stocker la vue du fond (vue à travers l'eau) avec un buffer de plus pour la position
        if (this.m_FBObackground != null) this.m_FBObackground.destroy();
        this.m_FBObackground = new FrameBufferObject(width*antialias, height*antialias, gl.TEXTURE_2D, gl.RENDERBUFFER, 1);

        // créer un FBO pour stocker la vue finale à superposer avec de la brume de distance
        if (this.m_FBOfog != null) this.m_FBObackground.destroy();
        this.m_FBOfog = new FrameBufferObject(width*antialias, height*antialias, gl.TEXTURE_2D, gl.TEXTURE_2D);

        // fournir ces FBO au matériau de l'eau
        this.m_WaterMaterial.setGBuffers(this.m_FBOreflection, this.m_FBObackground, width*antialias, height*antialias);
    }


    /**
     * appelée quand on enfonce le bouton gauche et qu'on déplace la souris
     * @param oldX : ancienne absisse de la souris
     * @param oldY : ancienne ordonnée de la souris
     * @param x : nouvelle absisse de la souris
     * @param y : nouvelle ordonnée de la souris
     * @param width : largeur de la vue OpenGL
     * @param height : hauteur de la vue OpenGL
     */
    onRotate(oldX, oldY, x, y, width, height)
    {
        // appeler la méthode de la superclasse
        super.onRotate(oldX, oldY, x, y, width, height);

        // empêcher que l'angle de hauteur devienne trop faible
        this.m_CameraAngleElevation = Math.max(this.m_CameraAngleElevation, 2.0);
        this.updateInfos();
    }


    /**
     * appelée quand on enfonce le bouton droit et qu'on déplace la souris
     * @param dx : nombre de pixels parcourus horizontalement
     * @param dy : nombre de pixels parcourus verticalement
     */
    onPan(dx, dy)
    {
        // appeler la méthode de la superclasse
        super.onPan(dx, dy);

        // empêcher que la hauteur devienne négative
        if (this.m_CameraPivot[1] > 0.0) this.m_CameraPivot[1] = 0.0;
        this.updateInfos();
    }


    /**
     * appelée quand on appuie sur une touche du clavier
     * @param code : touche enfoncée
     */
    onKeyDown(code)
    {
        // appeler la méthode de la superclasse
        super.onKeyDown(code);

        // empêcher que la hauteur devienne négative
        if (this.m_CameraPivot[1] > 0.0) this.m_CameraPivot[1] = 0.0;
        this.updateInfos();
    }


    /**
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4View : matrice de vue
     */
    onDraw(mat4Projection, mat4View)
    {
        // dessiner le terrain
        this.m_Island.onDraw(mat4Projection, mat4View);

        // NB: le fond n'est pas dessiné ici car il n'a pas de plan de coupe
    }


    /**
     * Dessine l'image courante
     */
    onDrawFrame()
    {
        /*** PREPARATION DES MATRICES ***/

        // construire la matrice caméra <- scène (fait passer de la scène à la caméra)
        let mat4View = this.getModelView();

        // transformation du miroir par rapport à la scène : aucune
        let mat4ModelMirror = mat4.create();

        // inverse de la matrice qui positionne le miroir dans la scène
        let mat4ModelMirrorInv = mat4.create();
        mat4.invert(mat4ModelMirrorInv, mat4ModelMirror);

        // matrice pour positionner le miroir par rapport à la caméra
        let mat4ModelViewMirror = mat4.create();
        mat4.multiply(mat4ModelViewMirror, mat4View, mat4ModelMirror);

        // matrice pour dessiner le reflet de la scène par rapport à la caméra
        let mat4ViewReflected = mat4.create();
        mat4.scale(mat4ViewReflected, mat4ViewReflected, vec3.fromValues(1,-1,1));
        mat4.multiply(mat4ViewReflected, mat4ViewReflected, mat4ModelMirrorInv);
        mat4.multiply(mat4ViewReflected, mat4ModelViewMirror, mat4ViewReflected);

        // transformation des plans de coupe
        let mat4ModelViewMirrorInverse = mat4.create();
        mat4.transpose(mat4ModelViewMirrorInverse, mat4ModelViewMirror);
        mat4.invert(mat4ModelViewMirrorInverse, mat4ModelViewMirrorInverse);

        // plan de coupe pour ne voir que le dessus
        let MirrorPlaneOver = vec4.fromValues(0, +1, 0, 0);
        vec4.transformMat4(MirrorPlaneOver, MirrorPlaneOver, mat4ModelViewMirrorInverse);

        // plan de coupe pour ne voir que le fond et un tout petit peu au dessus de l'eau
        let MirrorPlaneUnder = vec4.fromValues(0, -1, 0, 0.01);
        vec4.transformMat4(MirrorPlaneUnder, MirrorPlaneUnder, mat4ModelViewMirrorInverse);


        /*** DESSIN ***/

        /** étape 1 : dessiner le reflet du paysage dans le FBO reflet **/

        // préparer les lampes pour la vue inversée
        this.prepareLights(mat4ViewReflected);

        // fournir le plan de coupe ne dessinant que le fond à tous les objets dessinés
        this.m_Island.setClipPlane(true, MirrorPlaneUnder);

        // rediriger les dessins vers le g-buffer
        this.m_GBuffer.enable();
        gl.clearColor(0.0, 0.0, 0.0, 0.0);
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        // dessiner les objets inversés, mais seulement ceux qui sont au dessus de l'eau
        gl.frontFace(gl.CW);
        this.onDraw(this.m_Mat4Projection, mat4ViewReflected);
        gl.frontFace(gl.CCW);

        // revenir au dessin sur l'écran
        this.m_GBuffer.disable();

        // enlever le plan de coupe sans recompiler le shader
        this.m_Island.setClipPlane(true);

        // activer le FBO pour la brume
        this.m_FBOfog.enable();

        // ajouter les éclairements dans le FBO de la brume
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
        this.addLightings();

        // désactiver le FBO pour la brume
        this.m_FBOfog.disable();

        // (debug) dessiner le FBO sur l'écran
        //this.m_FBOfog.onDraw(gl.COLOR_ATTACHMENT0);return;
        //this.m_FBOfog.onDraw(gl.DEPTH_ATTACHMENT);return;

        // rediriger les dessins vers le FBO des reflets
        this.m_FBOreflection.enable();

        // appliquer la brume
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
        this.m_Fog.process(this.m_FBOfog);

        // revenir au dessin sur l'écran
        this.m_FBOreflection.disable();

        // (debug) dessiner le FBO sur l'écran
        //this.m_FBOreflection.onDraw(gl.COLOR_ATTACHMENT0+0);return;
        //this.m_FBOreflection.onDraw(gl.COLOR_ATTACHMENT0+1);return;


        /** étape 2 : dessiner le fond du paysage dans le FBO fond **/

        // préparer les lampes pour la vue normale
        this.prepareLights(mat4View);

        // fournir le plan de coupe ne dessinant que le fond à tous les objets dessinés
        this.m_Island.setClipPlane(true, MirrorPlaneUnder);

        // échanger le buffer des positions du g-buffer avec le 2e buffer du FBO du fond
        this.m_GBuffer.swapColorBuffers(2, this.m_FBObackground, 1);

        // rediriger les dessins vers le g-buffer
        this.m_GBuffer.enable();
        gl.clearColor(0.0, 0.0, 0.0, 0.0);
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        // dessiner les objets, mais seulement ce qui est sous l'eau
        this.onDraw(this.m_Mat4Projection, mat4View);
        this.m_Ground.onDraw(this.m_Mat4Projection, mat4View);

        // revenir au dessin sur l'écran
        this.m_GBuffer.disable();

        // enlever le plan de coupe sans recompiler le shader
        this.m_Island.setClipPlane(true);

        // rediriger les dessins vers le FBO du fond
        this.m_FBObackground.enable();
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        // ajouter les éclairements
        this.addLightings();

        // revenir au dessin sur l'écran
        this.m_FBObackground.disable();

        // défaire l'échange des buffers
        this.m_GBuffer.swapColorBuffers(2, this.m_FBObackground, 1);

        // (debug) dessiner le FBO sur l'écran
        //this.m_FBObackground.onDraw(gl.COLOR_ATTACHMENT0+0);return;
        //this.m_FBObackground.onDraw(gl.COLOR_ATTACHMENT0+1);return;


        /** étape 3 : dessiner la partie aérienne et la surface de l'eau avec son matériau spécial **/

        // fournir le plan de coupe à tous les objets dessinés
        this.m_Island.setClipPlane(true, MirrorPlaneOver);

        // rediriger les dessins vers le g-buffer
        this.m_GBuffer.enable();
        gl.clearColor(0.7, 0.9, 1.0, 0.0);
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        // dessiner le terrain
        this.m_Island.onDraw(this.m_Mat4Projection, mat4View);

        // dessiner l'eau => fusion reflet et fond
        this.m_Water.onDraw(this.m_Mat4Projection, mat4View);

        // revenir au dessin sur l'écran
        this.m_GBuffer.disable();

        // enlever le plan de coupe sans recompiler le shader
        this.m_Island.setClipPlane(true);

        // activer le FBO pour la brume
        this.m_FBOfog.enable();

        // ajouter les éclairements dans le FBO de la brume
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
        this.addLightings();

        // désactiver le FBO pour la brume
        this.m_FBOfog.disable();

        // (debug) dessiner le FBO sur l'écran
        //this.m_FBOfog.onDraw(gl.COLOR_ATTACHMENT0);return;
        //this.m_FBOfog.onDraw(gl.DEPTH_ATTACHMENT);return;

        // appliquer la brume
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
        this.m_Fog.process(this.m_FBOfog);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Fog.destroy();
        this.m_FBOfog.destroy();
        this.m_FBOreflection.destroy();
        this.m_FBObackground.destroy();
        this.m_Light1.destroy();
        this.m_Light0.destroy();
        this.m_Water.destroy();
        this.m_Ground.destroy();
        this.m_Island.destroy();
        this.m_WaterMaterial.destroy();
        super.destroy();
    }
}
