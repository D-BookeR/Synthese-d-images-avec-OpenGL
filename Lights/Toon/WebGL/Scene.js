// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Misc/TurnTableScene.js");
Requires("Cow");


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
            vec4.fromValues(-10.0, 20.0, 0.0, 1.0)
        ];
        let lights_count = this.m_LightsPositions.length;

        // couleurs des lampes
        this.m_LightsColors = [
            vec3.fromValues(1.0, 1.0, 1.0)
        ];

        // créer le matériau de la théière
        let Kd = vec3.fromValues(0.6, 0.1, 0.8);
        let Ks = vec3.fromValues(0.8, 0.8, 0.8);
        this.m_Material = new ToonMaterial(lights_count, Kd, Ks);

        // créer l'objet
        this.m_Object = new Cow(this.m_Material);

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.enable(gl.CULL_FACE);

        // couleur du fond
        gl.clearColor(0.4, 0.4, 0.4, 0.0);

        // paramètres de la vue
        this.m_CameraAngleAzimut = 45.0;
        this.m_CameraAngleElevation = 8.0;
        vec3.set(this.m_CameraPivot, 0.0, -0.9, 0.0);
        this.m_CameraDistance = 7;
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
        mat4.perspective(this.m_Mat4Projection, Utils.radians(18.0), width / height, 0.1, 20.0);
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

        // dessiner les arêtes en traits épais noirs
        gl.lineWidth(6.0);
        this.m_Object.onDrawEdges(mat4Projection, mat4ModelView);

        // dessiner l'objet avec un décalage vers l'avant
        gl.enable(gl.POLYGON_OFFSET_FILL);
        gl.polygonOffset(-6.0, -6.0);
        this.m_Object.onDraw(mat4Projection, mat4ModelView);
        gl.polygonOffset(0.0, 0.0);
        gl.disable(gl.POLYGON_OFFSET_FILL);
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
