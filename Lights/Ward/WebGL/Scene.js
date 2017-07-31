// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Misc/TurnTableScene.js");
Requires("Teapot");


class Scene extends TurnTableScene
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(false);

        // coordonnées des sources de lumières dans le repère global
        this.m_LightsPositions = [
            vec4.fromValues(10.0, 5.0, 0.0, 1.0),
        ];
        let lights_count = this.m_LightsPositions.length;

        // couleurs des lampes
        this.m_LightsColors = [
            vec3.fromValues(1.5, 1.5, 1.5),
        ];

        // créer le matériau de la théière
        let Kd = vec3.fromValues(0.4, 0.1, 0.0);
        let Ks = vec3.fromValues(0.6, 0.3, 0.1);
        let alpha = vec2.fromValues(0.1, 0.8);
        this.m_Material = new WardMaterial(lights_count, Kd, Ks, alpha);

        // créer l'objet
        this.m_Object = new Teapot(this.m_Material);

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.enable(gl.CULL_FACE);

        // couleur du fond
        gl.clearColor(0.4, 0.4, 0.4, 0.0);

        // paramètres de la vue
        this.m_CameraAngleAzimut = 30.0;
        this.m_CameraAngleElevation = 50.0;
        this.m_CameraDistance = 12;
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
        mat4.perspective(this.m_Mat4Projection, Utils.radians(10.0), width / height, 0.1, 20.0);
    }


    /**
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    onDraw(mat4Projection, mat4ModelView)
    {
        // fournir la position non transformée des lampes au matériau
        let lights_count = this.m_LightsPositions.length;
        for (let i=0; i<lights_count; i++) {
            this.m_Material.setLightPositionColor(i, this.m_LightsPositions[i], this.m_LightsColors[i]);
        }

        // dessiner l'objet
        this.m_Object.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Object.destroy();
        this.m_Material.destroy();
        super.destroy();
    }
}
