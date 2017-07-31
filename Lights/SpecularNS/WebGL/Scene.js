// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Mesh/MeshObjectFromObj.js");
Requires("libs/Misc/TurnTableScene.js");
Requires("PhongMaterial");


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
            vec4.fromValues(-6.0, 6.0, 4.0, 1.0),
            vec4.fromValues( 5.0, 5.0,-9.0, 1.0)
        ];
        let lights_count = this.m_LightsPositions.length;

        // couleurs des lampes
        this.m_LightsColors = [
            vec3.fromValues(1.0, 1.0, 1.0),
            vec3.fromValues(1.0, 1.0, 1.0)
        ];

        // créer les matériaux pour les pommes
        this.m_BlinnMaterial = new PhongMaterial(lights_count, vec3.fromValues(0.1,0.6,1.0), vec3.fromValues(1.0,1.0,1.0), 512);
        this.m_RedMaterial = new PhongMaterial(lights_count, vec3.fromValues(0.7,0.2,0.1), vec3.fromValues(0.3,0.3,0.3), 16);
        this.m_YellowMaterial = new PhongMaterial(lights_count, vec3.fromValues(0.6,0.7,0.0), vec3.fromValues(0.7,0.7,0.7), 96);

        // créer les objets
        this.m_AppleB = new MeshObjectFromObj("data/models/Apple", "apple.obj", this.m_BlinnMaterial, 0.01);
        this.m_RedApple = new MeshObjectFromObj("data/models/Apple", "apple.obj", this.m_RedMaterial, 0.01);
        this.m_YellowApple = new MeshObjectFromObj("data/models/Apple", "apple.obj", this.m_YellowMaterial, 0.01);

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.enable(gl.CULL_FACE);

        // couleur du fond
        gl.clearColor(0.4, 0.4, 0.4, 0.0);

        // paramètres de la vue
        this.m_CameraAngleAzimut = -10.0;
        this.m_CameraAngleElevation = 10.0;
        vec3.set(this.m_CameraPivot, 0.0, -0.4, 0.0);
        this.m_CameraDistance = 8.0;
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
        mat4.perspective(this.m_Mat4Projection, Utils.radians(10.0), width / height, 0.1, 15.0);
    }


    /**
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    onDraw(mat4Projection, mat4ModelView)
    {
        // transformer la position des lampes et la fournir aux matériaux
        let position = vec4.create();
        let lights_count = this.m_LightsPositions.length;
        for (let i=0; i<lights_count; i++) {
            // calculer la position de la lampe
            vec4.transformMat4(position, this.m_LightsPositions[i], mat4ModelView);
            this.m_BlinnMaterial.setLightPositionColor(i, position, this.m_LightsColors[i]);
            this.m_RedMaterial.setLightPositionColor(i, position, this.m_LightsColors[i]);
            this.m_YellowMaterial.setLightPositionColor(i, position, this.m_LightsColors[i]);
        }

        // matrice temporaire pour positionner chaque objet
        let mat4ModelViewApple = mat4.create();

        // positionner et dessiner le premier objet
        mat4.translate(mat4ModelViewApple, mat4ModelView, vec3.fromValues(-0.5, 0.0, 0.3));
        mat4.rotateY(mat4ModelViewApple, mat4ModelViewApple, Utils.radians(Utils.Time * 15.0));
        this.m_AppleB.onDraw(mat4Projection, mat4ModelViewApple);

        // positionner et dessiner le deuxième objet
        mat4.translate(mat4ModelViewApple, mat4ModelView, vec3.fromValues(+0.5, 0.0, 0.3));
        mat4.rotateY(mat4ModelViewApple, mat4ModelViewApple, Utils.radians(Utils.Time * 15.0));
        this.m_RedApple.onDraw(mat4Projection, mat4ModelViewApple);

        // positionner et dessiner le troisième objet
        mat4.translate(mat4ModelViewApple, mat4ModelView, vec3.fromValues( 0.0, 0.0,-0.5));
        mat4.rotateY(mat4ModelViewApple, mat4ModelViewApple, Utils.radians(Utils.Time * 15.0));
        this.m_YellowApple.onDraw(mat4Projection, mat4ModelViewApple);
    }


    /** supprime tous les objets alloués */
    destroy()
    {
        this.m_YellowApple.destroy();
        this.m_RedApple.destroy();
        this.m_AppleB.destroy();
        this.m_YellowMaterial.destroy();
        this.m_RedMaterial.destroy();
        this.m_BlinnMaterial.destroy();
        super.destroy();
    }
}
