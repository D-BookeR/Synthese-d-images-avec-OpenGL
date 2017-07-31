// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Misc/TurnTableScene.js");
Requires("libs/Misc/MeshObjectFromObj.js");
Requires("libs/Material/OmniLight.js");
Requires("libs/Material/OmniLightDebug.js");
Requires("libs/Material/SpotLight.js");
Requires("libs/Material/SpotLightDebug.js");


class Scene extends TurnTableScene
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(true);

        // créer les objets à dessiner
        this.m_Ground = new MeshObjectFromObj("data/models/TerrainSimple", "Terrain_2faces.obj", "Terrain.mtl", 5.0);
        this.m_Lorry  = new MeshObjectFromObj("data/models/Camion", "camion.obj", "camion.mtl", 2.0);

        // définir une lampe spot
        this.m_SpotLight = new SpotLightDebug(40.0, 30.0);
        this.m_SpotLight.setPosition(vec4.fromValues(0, 5, 6, 1));
        this.m_SpotLight.setTarget(vec4.fromValues(0, 0, 1, 1));
        this.m_SpotLight.setColor(vec3.fromValues(50.0, 50.0, 50.0));

        // définir une lampe positionnelle
        this.m_OmniLight = new OmniLightDebug();
        this.m_OmniLight.setPosition(vec4.fromValues(0, 10, 0, 1));
        this.m_OmniLight.setColor(vec3.fromValues(50.0, 50.0, 50.0));

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.enable(gl.CULL_FACE);

        // couleur du fond
        gl.clearColor(0.4, 0.5, 0.6, 0.0);

        // paramètres de la vue
        this.m_CameraAngleAzimut = 40.0;
        this.m_CameraAngleElevation = 40.0;
        this.m_CameraDistance = 35;
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
        mat4.perspective(this.m_Mat4Projection, Utils.radians(12.0), width / height, 2.0, 100.0);
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

        // dessiner le terrain
        this.m_Ground.onDraw(mat4Projection, mat4View);

        // dessiner le camion
        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(0.0,0.0,1.0));
        this.m_Lorry.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Dessine l'image courante
     */
    onDrawFrame()
    {
        // effacer l'écran
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        // construire la matrice caméra
        let mat4View = this.getModelView();

        // transformer les lampes
        this.m_SpotLight.transform(mat4View);
        this.m_OmniLight.transform(mat4View);

        // dessiner les objets dans cette transformation
        this.drawDeferredShading(this.m_Mat4Projection, mat4View);

        // dessiner l'éclairement des lampes
        this.addLighting(this.m_SpotLight, true);
        this.addLighting(this.m_OmniLight);

        // dessiner les lampes
        this.m_SpotLight.onDraw(this.m_Mat4Projection, mat4View);
        this.m_OmniLight.onDraw(this.m_Mat4Projection, mat4View);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_OmniLight.destroy();
        this.m_SpotLight.destroy();
        this.m_Lorry.destroy();
        this.m_Ground.destroy();
        super.destroy();
    }
}
