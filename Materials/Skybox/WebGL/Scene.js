// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Misc/TurnTableScene.js");
Requires("libs/Material/Light.js");
Requires("libs/Material/OmniLight.js");
Requires("Skybox");
Requires("Cow");


class Scene extends TurnTableScene
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(true);

        // créer les objets de la scène
        this.m_Skybox = new Skybox();
        this.m_Cow = new Cow();

        // définir une lampe ambiante
        this.m_Light0 = new Light();
        this.m_Light0.setColor(vec3.fromValues(0.5,0.5,0.5));

        // définir une lampe directionnelle
        this.m_Light1 = new OmniLight();
        this.m_Light1.setPosition(vec4.fromValues(-0.2, 1.0, 0.0, 0));
        this.m_Light1.setColor(vec3.fromValues(1.0,1.0,1.0));

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.enable(gl.CULL_FACE);

        // couleur du fond
        gl.clearColor(0.4, 0.4, 0.4, 0.0);

        // paramètres de la vue
        this.m_CameraAngleAzimut = 140.0;
        this.m_CameraAngleElevation = 0.0;
        this.m_CameraDistance = 1.0;
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
        mat4.perspective(this.m_Mat4Projection, Utils.radians(60.0), width / height, 0.1, 20.0);
    }


    /**
     * appelée quand on enfonce le bouton du milieu et qu'on déplace la souris
     * @param dz : nombre de pixels parcourus
     */
    onZoom(dz)
    {
    }


    onPan(dx, dy)
    {
    }


    onKeyDown(code)
    {
    }


    /**
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    onDraw(mat4Projection, mat4ModelView)
    {
        // dessiner la vache en la décalant un peu vers le bas
        mat4.translate(mat4ModelView, mat4ModelView, vec3.fromValues(0.0, -0.4, 0.0));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(270.0));
        this.m_Cow.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Dessine l'image courante
     */
    onDrawFrame()
    {
        // effacer l'écran
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        // construire la matrice MV : la caméra est au centre
        mat4.identity(this.m_Mat4ModelView);

        // faire pivoter la scène autour de son centre
        mat4.rotateX(this.m_Mat4ModelView, this.m_Mat4ModelView, Utils.radians(this.m_CameraAngleElevation));
        mat4.rotateY(this.m_Mat4ModelView, this.m_Mat4ModelView, Utils.radians(this.m_CameraAngleAzimut));

        // dessiner la skybox
        this.m_Skybox.onDraw(this.m_Mat4Projection, this.m_Mat4ModelView);

        // réinitialiser la matrice pour tourner autour de l'objet
        mat4.identity(this.m_Mat4ModelView);
        mat4.translate(this.m_Mat4ModelView, this.m_Mat4ModelView, vec3.fromValues(0.0, 0.0, -this.m_CameraDistance));

        // faire pivoter la scène autour de son centre
        mat4.rotateX(this.m_Mat4ModelView, this.m_Mat4ModelView, Utils.radians(this.m_CameraAngleElevation));
        mat4.rotateY(this.m_Mat4ModelView, this.m_Mat4ModelView, Utils.radians(this.m_CameraAngleAzimut));

        // positionner les lampes et dessiner leur shadow map s'il y en a
        this.prepareLights(this.m_Mat4ModelView);

        // dessiner les objets de la scène
        this.drawDeferredShading(this.m_Mat4Projection, this.m_Mat4ModelView);
        this.addLighting(this.m_Light0, true);
        this.addLighting(this.m_Light1);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Light1.destroy();
        this.m_Light0.destroy();
        this.m_Cow.destroy();
        this.m_Skybox.destroy();
        super.destroy();
    }
}
