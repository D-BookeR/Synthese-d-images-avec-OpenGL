// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Process/FrameBufferObject.js");
Requires("libs/Misc/TurnTableScene.js");
Requires("Sphere");
Requires("Rock");


class Scene extends TurnTableScene
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(false);

        // créer le matériau basé sur une texture (au choix)
        this.m_Texture = new Texture2D("data/textures/earth/earth_map.jpg", gl.LINEAR, gl.CLAMP_TO_EDGE);

        // créer les objets
        this.m_Sphere = new Sphere(48,24, this.m_Texture);
        this.m_Rock = new Rock(this.m_Texture);

        // paramètres de la vue
        this.m_CameraAngleAzimut = 25.0;
        this.m_CameraAngleElevation = 15.0;
        this.m_CameraDistance = 10.0;
        this.m_DisplaySphere = true;

        // ressources
        this.m_FBOpreceeding = null;
        this.m_FBOcurrent = null;
        this.m_FBOresult = null;
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

        // supprimer les anciens FBO
        if (this.m_FBOpreceeding != null) this.m_FBOpreceeding.destroy();
        if (this.m_FBOpreceeding != null) this.m_FBOpreceeding.destroy();
        if (this.m_FBOresult != null) this.m_FBOresult.destroy();

        // FBO pour la première passe du dessin des objets transparents (alternances entre les deux)
        this.m_FBOpreceeding = new FrameBufferObject(width, height, gl.TEXTURE_2D, gl.TEXTURE_2D);
        this.m_FBOcurrent = new FrameBufferObject(width, height, gl.TEXTURE_2D, gl.TEXTURE_2D);

        // FBO pour accumuler la deuxième passe du dessin des objets transparents
        this.m_FBOresult = new FrameBufferObject(width, height, gl.TEXTURE_2D, gl.TEXTURE_2D);

        // taille de la fenêtre à l'objet transparent
        this.m_Sphere.setWindowDimensions(width, height);
    }


    /**
     * appelée quand on appuie sur une touche du clavier
     * @param code : touche enfoncée
     */
    onKeyDown(code)
    {
        switch (code) {
        case 'T'.charCodeAt(0):
            this.m_DisplaySphere = !this.m_DisplaySphere;
            break;
        default:
            // appeler la méthode de la superclasse
            super.onKeyDown(code);
        }
    }


    /**
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    onDraw(mat4Projection, mat4ModelView)
    {
        // effacer le depth buffer du 2e FBO mrt transparents
        gl.clearDepth(0.0);
        this.m_FBOpreceeding.enable();
        gl.clear(gl.DEPTH_BUFFER_BIT);
        this.m_FBOpreceeding.disable();
        gl.clearDepth(1.0);
        this.m_FBOcurrent.enable();
        gl.clear(gl.DEPTH_BUFFER_BIT);
        this.m_FBOcurrent.disable();


        /// dessiner les objets opaques sur le FBO couches

        // ne dessiner que les faces avant
        gl.enable(gl.CULL_FACE);

        // effacer le FBO qui contiendra le résultat
        this.m_FBOresult.enable();
        gl.clearColor(0.1, 0.1, 0.1, 1.0);
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
        this.m_Rock.onDraw(mat4Projection, mat4ModelView);
        this.m_FBOresult.disable();

        if (this.m_DisplaySphere) {

            /// Cycles d'épluchage des transparences

            // mode de transparence arrière
            gl.blendEquation(gl.FUNC_ADD);
            gl.blendFuncSeparate(gl.DST_ALPHA, gl.ONE, gl.ZERO, gl.ONE_MINUS_SRC_ALPHA);

            // dessiner toutes les faces des objets
            gl.disable(gl.CULL_FACE);

            const NbCycles = 2;
            for (let i=0; i<NbCycles; i++) {

                // fournir les depth buffers des FBO au matériau transparent
                this.m_Sphere.setDepthMaps(this.m_FBOresult.getDepthBuffer(), this.m_FBOpreceeding.getDepthBuffer());

                // dessiner la sphère
                this.m_FBOcurrent.enable();
                gl.clearColor(0.0, 0.0, 0.0, 0.0);
                gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
                this.m_Sphere.onDraw(mat4Projection, mat4ModelView);
                this.m_FBOcurrent.disable();

                // reporter cette couche sur le FBO du résultat en mode fusion arrière
                gl.enable(gl.BLEND);
                this.m_FBOresult.enable();
                this.m_FBOcurrent.onDraw(gl.COLOR_ATTACHMENT0);
                this.m_FBOresult.disable();
                gl.disable(gl.BLEND);

                // échanger les FBO
                let tmp = this.m_FBOcurrent;
                this.m_FBOcurrent = this.m_FBOpreceeding;
                this.m_FBOpreceeding = tmp;
            }
        }

        // recopier le FBO transpa sur l'écran
        this.m_FBOresult.onDraw(gl.COLOR_ATTACHMENT0);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Sphere.destroy();
        this.m_Rock.destroy();
        this.m_FBOpreceeding.destroy();
        this.m_FBOcurrent.destroy();
        this.m_FBOresult.destroy();
        this.m_Texture.destroy();
        super.destroy();
    }
}
