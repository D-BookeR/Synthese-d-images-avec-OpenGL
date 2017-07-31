// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Misc/TurnTableScene.js");
Requires("libs/Material/TextureCube.js");
Requires("Skybox");
Requires("Teapot");


class Scene extends TurnTableScene
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(false);

        // charger la texture skybox commune aux objets
        this.m_Texture = new TextureCube("data/textures/Teide");

        // créer les objets de la scène
        this.m_Skybox = new Skybox(this.m_Texture);
        this.m_Teapot = new Teapot(this.m_Texture);

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.enable(gl.CULL_FACE);

        // couleur du fond
        gl.clearColor(0.4, 0.4, 0.4, 0.0);

        // paramètres de la vue
        this.m_CameraAngleAzimut = 0.0;
        this.m_CameraAngleElevation = 0.0;
        this.m_CameraDistance = 1.5;
    }


    updateInfos()
    {
        infos = document.getElementById("infos").innerHTML =
            "azimut = " + Math.round(this.m_CameraAngleAzimut * 10)/10 + "<br/>" +
            "height = " + Math.round(this.m_CameraAngleElevation * 10)/10 + "<br/>";
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
        mat4.perspective(this.m_Mat4Projection, Utils.radians(30.0), width / height, 0.1, 5.0);
    }


    /**
     * Dessine l'image courante
     */
    onDrawFrame()
    {
        // effacer l'écran
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        // matrice ModelView pour afficher la skybox
        let mat4ModelViewSkybox = mat4.create();

        // faire pivoter la scène autour de son centre
        mat4.rotateX(mat4ModelViewSkybox, mat4ModelViewSkybox, Utils.radians(this.m_CameraAngleElevation));
        mat4.rotateY(mat4ModelViewSkybox, mat4ModelViewSkybox, Utils.radians(this.m_CameraAngleAzimut));

        // dessiner la skybox
        this.m_Skybox.onDraw(this.m_Mat4Projection, mat4ModelViewSkybox);


        // matrice ModelView pour afficher la scène
        let mat4ModelViewScene = mat4.create();

        // la théière est décalée par rapport au centre afin qu'on la voie
        mat4.translate(mat4ModelViewScene, mat4ModelViewScene, vec3.fromValues(0.0, 0.0, -this.m_CameraDistance));

        // appliquer la rotation de la skybox (scène) à la théière
        mat4.multiply(mat4ModelViewScene, mat4ModelViewScene, mat4ModelViewSkybox);

        // dessiner la théière
        this.m_Teapot.onDraw(this.m_Mat4Projection, mat4ModelViewScene);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Teapot.destroy();
        this.m_Skybox.destroy();
        this.m_Texture.destroy();
        super.destroy();
    }
}
