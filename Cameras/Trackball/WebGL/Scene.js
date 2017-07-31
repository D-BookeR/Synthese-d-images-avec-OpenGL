// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("Earth");
Requires("libs/Misc/AxesXYZ.js");
Requires("libs/Material/Light.js");
Requires("libs/Material/OmniLight.js");
Requires("libs/Misc/SceneBase.js");

class Scene extends SceneBase
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(true);

        // créer les objets à dessiner
        this.m_Earth = new Earth(24, 24);
        this.m_Axes = new AxesXYZ(1.0, 1.0, 6.0);

        // définir une lampe omnidirectionnelle
        this.m_Light0 = new OmniLight();
        this.m_Light0.setPosition(vec4.fromValues(5, 5, 3, 0));
        this.m_Light0.setColor(vec3.fromValues(1.0,1.0,1.0));

        // définir une lampe ambiante
        this.m_Light1 = new Light();
        this.m_Light1.setColor(vec3.fromValues(0.7,0.7,0.7));

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.enable(gl.CULL_FACE);

        // couleur du fond
        gl.clearColor(0.4, 0.4, 0.4, 0.0);

        // paramètres de la vue
        this.m_CameraZ = 25;
        this.m_QuatOrientation = quat.create();
    }


    /** affiche les informations sur la caméra **/
    updateInfos()
    {
        // mode trackball, difficile d'afficher quoi que ce soit
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
        mat4.perspective(this.m_Mat4Projection, Utils.radians(10.0), width / height, 1.0, 100.0);
    }


    /**
     * appelée quand on enfonce le bouton du milieu et qu'on déplace la souris
     * @param dz : nombre de pixels parcourus
     */
    onZoom(dz)
    {
        this.m_CameraZ *= Math.exp(dz * -0.001);
        this.updateInfos();
    }


    /**
     * appelée quand on enfonce le bouton droit et qu'on déplace la souris
     * @param dx : nombre de pixels parcourus horizontalement
     * @param dy : nombre de pixels parcourus verticalement
     */
    onPan(dx, dy)
    {
    }


    /**
     * appelée quand on enfonce le bouton gauche et qu'on déplace la souris
     * @param oldX : ancienne absisse de la souris
     * @param oldY : ancienne ordonnée de la souris
     * @param x : nouvelle absisse de la souris
     * @param y : nouvelle ordonnée de la souris
     * @param width : largeur de la vue OpenGL
     * @param height : hauteur de la vue OpenGL
     */
    onRotate(oldX, oldY, x, y, width, height)
    {
        // normalisation de la fenêtre
        let half_width = width * 0.5;
        let half_height = height * 0.5;
        let half_size = (half_width + half_height) * 0.5;

        // construire p1
        let x1 = (oldX-half_width)/half_size;
        x1 = Math.min(Math.max(x1, -1.0), +1.0);
        let y1 = (half_height-oldY)/half_size;
        y1 = Math.min(Math.max(y1, -1.0), +1.0);
        let z1 = Math.sqrt(Math.max(1.0 - x1*x1 - y1*y1, 0.0));
        let p1 = vec3.fromValues(x1,y1,z1);

        // construire p2
        let x2 = (x-half_width)/half_size;
        x2 = Math.min(Math.max(x2, -1.0), +1.0);
        let y2 = (half_height-y)/half_size;
        y2 = Math.min(Math.max(y2, -1.0), +1.0);
        let z2 = Math.sqrt(Math.max(1.0 - x2*x2 - y2*y2, 0.0));
        let p2 = vec3.fromValues(x2,y2,z2);

        // construire le quaternion du mouvement P1->P2
        let axis = vec3.create();
        vec3.cross(axis, p1, p2);
        let angle = vec3.length(axis);
        vec3.normalize(axis, axis);
        let quatP1P2 = quat.create();
        quat.setAxisAngle(quatP1P2, axis, angle);

        // appliquer ce quaternion au quaternion global (attention à l'ordre)
        quat.multiply(this.m_QuatOrientation, quatP1P2, this.m_QuatOrientation);
        this.updateInfos();
    }


    /**
     * appelée quand on appuie sur une touche du clavier
     * @param code : touche enfoncée
     */
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
        // dessiner la terre
        this.m_Earth.onDraw(mat4Projection, mat4ModelView);

        // dessiner le trièdre agrandi 2.5x
        let mat4ModelViewTmp = mat4.create();
        mat4.scale(mat4ModelViewTmp, mat4ModelView, vec3.fromValues(2.5, 2.5, 2.5));
        this.m_Axes.onDraw(mat4Projection, mat4ModelViewTmp);
    }


    /**
     * Dessine l'image courante
     */
    onDrawFrame()
    {
        // effacer l'écran
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        // transformer le quaternion en rotation et rajouter la translation d'éloignement
        let mat4ModelViewTmp = mat4.create();
        mat4.fromRotationTranslation(mat4ModelViewTmp, this.m_QuatOrientation, vec3.fromValues(0, 0, -this.m_CameraZ));

        // dessiner les objets dans cette transformation
        this.drawDeferredShading(this.m_Mat4Projection, mat4ModelViewTmp);

        // dessiner l'éclairement de la première lampe
        this.addLighting(this.m_Light0, true);

        // rajouter les éclairements des lampes suivantes
        this.addLighting(this.m_Light1);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Light1.destroy();
        this.m_Light0.destroy();
        this.m_Axes.destroy();
        this.m_Earth.destroy();
        super.destroy();
    }
}
