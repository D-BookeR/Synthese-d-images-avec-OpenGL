// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("Earth");


class Scene
{
    /**
     * Constructeur
     */
    constructor()
    {
        // créer l'objet à dessiner
        this.m_Earth = new Earth(64,32);

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);

        // couleur du fond : gris foncé
        gl.clearColor(0.4, 0.4, 0.4, 0.0);

        // paramètres de la vue
        this.m_Mat4Projection = mat4.create();
    }


    /**
     * appelée quand la taille de la vue OpenGL change
     * @param width : largeur en nombre de pixels de la fenêtre
     * @param height : hauteur en nombre de pixels de la fenêtre
     */
    onSurfaceChanged(width, height)
    {
        // met en place le viewport
        gl.viewport(0, 0, width, height);

        // matrice de projection (champ de vision)
        mat4.perspective(this.m_Mat4Projection, Utils.radians(20.0), width / height, 0.1, 20.0);
    }


    /**
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    onDraw(mat4Projection, mat4ModelView)
    {
        this.m_Earth.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Dessine l'image courante
     */
    onDrawFrame()
    {
        // effacer l'écran
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        // position de la lumière animée en fonction du temps
        let lightMatrix = mat4.create();
        mat4.identity(lightMatrix);
        mat4.rotate(lightMatrix, lightMatrix, Utils.radians(Utils.Time*25.0), vec3.fromValues(0.1, 1, 0));
        let pos_light = vec4.fromValues(0.0, 2.0, 10.0, 1.0);
        vec4.transformMat4(pos_light, pos_light, lightMatrix)
        this.m_Earth.setLightPosition(pos_light);

        // construire la matrice MV animée en fonction du temps
        let mat4ModelView = mat4.create();
        mat4.identity(mat4ModelView);
        mat4.translate(mat4ModelView, mat4ModelView, vec3.fromValues(0.0, 0.0, -10));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(Utils.Time*25.0));
        mat4.translate(mat4ModelView, mat4ModelView, vec3.fromValues(0.5, 0.0, 0.0));

        // dessiner les objets
        this.onDraw(this.m_Mat4Projection, mat4ModelView);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Earth.destroy();
    }
}
