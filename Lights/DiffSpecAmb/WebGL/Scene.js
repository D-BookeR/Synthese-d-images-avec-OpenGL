// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Material/Texture2D.js");
Requires("libs/Mesh/MeshObjectFromObj.js");
Requires("libs/Misc/TurnTableScene.js");
Requires("AmbiantMaterial");
Requires("DiffuseMaterial");
Requires("SpecularMaterial");
Requires("BlinnMaterial");


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
            vec4.fromValues( 7.0, 5.0,-5.0, 1.0)
        ];
        let lights_count = this.m_LightsPositions.length;

        // couleurs des lampes
        this.m_LightsColors = [
            vec3.fromValues(0.9, 0.9, 0.9),
            vec3.fromValues(0.7, 0.7, 0.7)
        ];

        // créer les matériaux pour les pommes
        this.m_Texture = new Texture2D("data/models/Apple/skin.jpg", gl.LINEAR, gl.CLAMP_TO_EDGE);
        let Ka = vec3.fromValues(0.2, 0.2, 0.2);
        let Ks = vec3.fromValues(0.7, 0.7, 0.7);
        let ns = 64;
        this.m_AmbiantMaterial  = new AmbiantMaterial(lights_count, Ka, this.m_Texture);
        this.m_DiffuseMaterial  = new DiffuseMaterial(lights_count, this.m_Texture);
        this.m_SpecularMaterial = new SpecularMaterial(lights_count, Ks, ns);
        this.m_BlinnMaterial    = new BlinnMaterial(lights_count, Ka, this.m_Texture, Ks, ns);

        // créer les objets
        this.m_AppleAmbiant    = new MeshObjectFromObj("data/models/Apple", "apple.obj", this.m_AmbiantMaterial, 0.01);
        this.m_AppleDiffuse     = new MeshObjectFromObj("data/models/Apple", "apple.obj", this.m_DiffuseMaterial, 0.01);
        this.m_SpecularApple = new MeshObjectFromObj("data/models/Apple", "apple.obj", this.m_SpecularMaterial, 0.01);
        this.m_AppleBlinn      = new MeshObjectFromObj("data/models/Apple", "apple.obj", this.m_BlinnMaterial, 0.01);

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.enable(gl.CULL_FACE);

        // couleur du fond
        gl.clearColor(0.4, 0.4, 0.4, 0.0);

        // paramètres de la vue
        this.m_CameraAngleAzimut = 20.0;
        this.m_CameraAngleElevation = 20.0;
        vec3.set(this.m_CameraPivot, 0.0, -0.4, 0.0);
        this.m_CameraDistance = 5.0;

        // taille des sous-fenêtres : 1/2 de la fenêtre principale
        this.m_WindowWidth2 = 0;
        this.m_WindowHeight2 = 0;
    }


    /**
     * appelée quand la taille de la vue OpenGL change
     * @param width : largeur en nombre de pixels de la fenêtre
     * @param height : hauteur en nombre de pixels de la fenêtre
     */
    onSurfaceChanged(width, height)
    {
        // taille des sous-fenêtres : 1/2 de la fenêtre principale
        this.m_WindowWidth2 = width/2.0;
        this.m_WindowHeight2 = height/2.0;

        // matrice de projection (champ de vision)
        mat4.perspective(this.m_Mat4Projection, Utils.radians(10.0), this.m_WindowWidth2 / this.m_WindowHeight2, 1.0, 15.0);
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
            vec4.transformMat4(position, this.m_LightsPositions[i], mat4ModelView);
            this.m_AmbiantMaterial.setLightPositionColor(i, position, this.m_LightsColors[i]);
            this.m_DiffuseMaterial.setLightPositionColor(i, position, this.m_LightsColors[i]);
            this.m_SpecularMaterial.setLightPositionColor(i, position, this.m_LightsColors[i]);
            this.m_BlinnMaterial.setLightPositionColor(i, position, this.m_LightsColors[i]);
        }

        // matrice temporaire pour faire tourner la pomme
        let mat4ModelViewApple = mat4.create();
        mat4.rotateY(mat4ModelViewApple, mat4ModelView, Utils.radians(Utils.Time * 15.0 + 100.0));

        // dessiner l'éclairement ambiant dans le coin haut gauche
        gl.viewport(0, this.m_WindowHeight2, this.m_WindowWidth2, this.m_WindowHeight2);
        this.m_AppleAmbiant.onDraw(mat4Projection, mat4ModelViewApple);

        // dessiner l'éclairement diffus dans le coin haut droit
        gl.viewport(this.m_WindowWidth2, this.m_WindowHeight2, this.m_WindowWidth2, this.m_WindowHeight2);
        this.m_AppleDiffuse.onDraw(mat4Projection, mat4ModelViewApple);

        // dessiner l'éclairement spéculaire dans le coin bas gauche
        gl.viewport(0, 0, this.m_WindowWidth2, this.m_WindowHeight2);
        this.m_SpecularApple.onDraw(mat4Projection, mat4ModelViewApple);

        // dessiner les éclairements superposés dans le coin bas droit
        gl.viewport(this.m_WindowWidth2, 0, this.m_WindowWidth2, this.m_WindowHeight2);
        this.m_AppleBlinn.onDraw(mat4Projection, mat4ModelViewApple);
    }


    /** supprime tous les objets alloués */
    destroy()
    {
        this.m_AppleBlinn.destroy();
        this.m_SpecularApple.destroy();
        this.m_AppleDiffuse.destroy();
        this.m_AppleAmbiant.destroy();
        this.m_BlinnMaterial.destroy();
        this.m_SpecularMaterial.destroy();
        this.m_DiffuseMaterial.destroy();
        this.m_AmbiantMaterial.destroy();
        this.m_Texture.destroy();
        super.destroy();
    }
}
