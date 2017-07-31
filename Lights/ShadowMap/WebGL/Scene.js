// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Misc/TurnTableScene.js");
Requires("libs/Misc/Plane.js");
Requires("libs/Misc/Cube.js");
Requires("libs/Process/ShadowMap.js");
Requires("ShadowMaterial.js");


class Scene extends TurnTableScene
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(false);

        // crée le plan et son matériau
        this.m_PlaneMaterial = new ShadowMaterial(vec4.fromValues(0.8,0.8,0.8,1.0));
        this.m_Plane = new Plane(this.m_PlaneMaterial);

        // crée le cube et son matériau
        this.m_CubeMaterial = new ShadowMaterial(vec4.fromValues(1.0,0.2,0.6,1.0));
        this.m_Cube = new Cube(this.m_CubeMaterial);

        // créer une shadow map
        // défaut n°1 : crénelage
        // modifier la taille de la shadow map, ex: 128, 256, 512, 1024, 2048
        this.m_ShadowMap = new ShadowMap(256);
        this.m_ShadowMatrix = mat4.create();

        // définir un point de vue pour la lampe
        // défaut n°2 : champ incorrect, modifier les valeurs d'angle, near et far
        this.m_MaxAngle = 40.0;
        this.m_Near = 5.0;
        this.m_Far = 15.0;

        // point de vue et cible de la lampe en coordonnées scène
        this.m_Position = vec3.fromValues(5.0, 5.0, -2.0);
        this.m_Target = vec3.fromValues(0.0, 0.0, 0.0);

        // point de vue en coordonnées caméra (mis à jour auto dans makeShadowMap)
        this.m_PositionCamera = vec3.create();
        this.m_TargetCamera = vec3.create();

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.enable(gl.CULL_FACE);

        // couleur du fond
        gl.clearColor(0.5, 0.6, 0.7, 0.0);

        // paramètres de la vue
        this.m_CameraAngleAzimut = -46.0;
        this.m_CameraAngleElevation = 35.0;
        this.m_CameraDistance = 15;
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

        // dessiner le plan
        mat4.scale(mat4ModelView, mat4View, vec3.fromValues(2.0, 2.0, 2.0));
        this.m_Plane.onDraw(mat4Projection, mat4ModelView);

        // dessiner le cube
        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(0.0, 0.5, 0.0));
        mat4.scale(mat4ModelView, mat4ModelView, vec3.fromValues(0.5, 0.5, 0.5));
        this.m_Cube.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * dessine la scène dans la shadowmap de cette lampe
     * @param mat4ViewCamera : matrice de transformation dans laquelle sont dessinés les objets
     */
    makeShadowMap(mat4ViewCamera)
    {
        // transformer la position et la cible de la lampe dans le repère caméra
        vec3.transformMat4(this.m_PositionCamera, this.m_Position, mat4ViewCamera);
        vec3.transformMat4(this.m_TargetCamera, this.m_Target, mat4ViewCamera);

        // construire une matrice de projection à partir de la lampe
        // TODO changer le type de perspective selon la nature de position et target (directions ou positions)
        let mat4LightProjection = mat4.create();
        mat4.perspective(mat4LightProjection, Utils.radians(this.m_MaxAngle), 1.0, this.m_Near, this.m_Far);

        // construire une matrice de vue à partir de la lampe
        let mat4LightView = mat4.create();
        mat4.lookAt(mat4LightView, this.m_PositionCamera, this.m_TargetCamera, vec3.fromValues(0,1,0));

        // appliquer la matrice de la scène
        mat4.multiply(mat4LightView, mat4LightView, mat4ViewCamera);

        // calculer la matrice d'ombre
        mat4.invert(this.m_ShadowMatrix, mat4ViewCamera);
        mat4.multiply(this.m_ShadowMatrix, mat4LightView, this.m_ShadowMatrix);
        mat4.multiply(this.m_ShadowMatrix, mat4LightProjection, this.m_ShadowMatrix);
        mat4.multiply(this.m_ShadowMatrix, ShadowMap.c_MatBias, this.m_ShadowMatrix);

        // passer en mode dessin dans la shadowmap
        this.m_ShadowMap.enable();

        // avant de voir les solutions aux problèmes, regarder ce que fait la méthode m_ShadowMap->enable
        // elle met en place un décalage de polygones, et peut (mais c'est commenté) dessiner les faces
        // vues de dos seulement. Donc dans la suite, je désactive ça.

        // remettre comme c'est normalement, mais ça crée des défauts
        gl.disable(gl.POLYGON_OFFSET_FILL);
        gl.cullFace(gl.BACK);


        // défaut n°3 : "acné de surface"
        // décommenter les deux lignes suivantes pour le résoudre
        // (c'est fait par défaut dans la méthode enable de la classe ShadowMap)
        // ou bien modifier la valeur de DECALAGE dans le shader de la classe ShadowMaterial, la rendre négative
        //gl.enable(gl.POLYGON_OFFSET_FILL);
        //gl.polygonOffset(1.0, 1.0);


        // défaut n°4 : "murs volants"
        // modifier la constante DECALAGE dans le shader de ShadowMaterial, la rendre positive
        // ou bien décommenter les deux lignes suivantes pour dessiner seulement le dos des objets, pas leurs faces avant
        //gl.enable(gl.CULL_FACE);
        //gl.cullFace(gl.FRONT);


        // dessiner les objets
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
        this.onDraw(mat4LightProjection, mat4LightView);

        // annuler le décalage
        gl.polygonOffset(0.0, 0.0);
        gl.disable(gl.POLYGON_OFFSET_FILL);

        // remettre l'élimination des faces vues de dos
        gl.cullFace(gl.BACK);

        // revenir en mode dessin normal
        this.m_ShadowMap.disable();
    }


    /**
     * Dessine l'image courante
     */
    onDrawFrame()
    {
        // construire la matrice caméra
        let mat4ViewCamera = this.getModelView();

        // enlever la shadow map des matériaux sinon ils vont tenter de l'utiliser pendant son dessin
        this.m_PlaneMaterial.setShadowMap(this.m_ShadowMatrix, this.m_PositionCamera, null);
        this.m_CubeMaterial.setShadowMap(this.m_ShadowMatrix, this.m_PositionCamera, null);

        // calculer la shadow map
        this.makeShadowMap(mat4ViewCamera);

        // mise au point : décommenter la ligne suivante
        //this.m_ShadowMap.onDrawDepth();/*

        // effacer l'écran
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        // fournir la shadow map et la matrice aux matériaux
        this.m_PlaneMaterial.setShadowMap(this.m_ShadowMatrix, this.m_PositionCamera, this.m_ShadowMap);
        this.m_CubeMaterial.setShadowMap(this.m_ShadowMatrix, this.m_PositionCamera, this.m_ShadowMap);

        // dessiner la scène
        this.onDraw(this.m_Mat4Projection, mat4ViewCamera);

        //*/
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Plane.destroy();
        this.m_PlaneMaterial.destroy()
        this.m_Cube.destroy();
        this.m_CubeMaterial.destroy()
        super.destroy();
    }
}
