// Définition de la classe TrackBallScene


// classes et modules nécessaires
Requires("libs/Misc/SceneBase.js");


/**
 * Définit une base pour dessiner une scène en mode trackball
 */
class TrackBallScene extends SceneBase
{
    /**
     * Définit une base pour dessiner une scène en mode trackball
     * @param deferredShading : true si cette classe gère le FBO m_GBuffer pour un dessin en deux passes
     */
    constructor(deferredShading)
    {
        // initialiser la superclasse
        super(deferredShading);

        // paramètres de la vue
        this.m_QuatP1P2 = quat.create();
        this.m_QuatOrientation = quat.create();
        this.m_CameraDistance = 18.0;
    }


    /** destructeur */
    destroy()
    {
        super.destroy();
    }


    /** affiche les informations sur la caméra **/
    updateInfos()
    {
    }


    /**
     * appelée quand on enfonce le bouton du milieu et qu'on déplace la souris
     * @param dd : nombre de pixels parcourus
     */
    onZoom(dd)
    {
        this.m_CameraDistance *= Math.exp(dd * -0.001);
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

        // construire P1
        let x1 = (oldX-half_width)/half_size;
        x1 = Math.min(Math.max(x1, -1.0), +1.0);
        let y1 = (half_height-oldY)/half_size;
        y1 = Math.min(Math.max(y1, -1.0), +1.0);
        let z1 = Math.sqrt(Math.max(1.0 - x1*x1 - y1*y1, 0.0));
        let P1 = vec3.fromValues(x1,y1,z1);

        // construire P2
        let x2 = (x-half_width)/half_size;
        x2 = Math.min(Math.max(x2, -1.0), +1.0);
        let y2 = (half_height-y)/half_size;
        y2 = Math.min(Math.max(y2, -1.0), +1.0);
        let z2 = Math.sqrt(Math.max(1.0 - x2*x2 - y2*y2, 0.0));
        let P2 = vec3.fromValues(x2,y2,z2);

        // construire le quaternion du mouvement P1->P2
        let axis = vec3.create();
        vec3.cross(axis, P1, P2);
        let angle = vec3.length(axis);
        vec3.normalize(axis, axis);
        quat.setAxisAngle(this.m_QuatP1P2, axis, angle);

        // appliquer ce quaternion au quaternion global (attention à l'ordre)
        quat.multiply(this.m_QuatOrientation, this.m_QuatP1P2, this.m_QuatOrientation);
    }


    /**
     * appelée quand on appuie sur une touche du clavier
     * @param code : touche enfoncée
     */
    onKeyDown(code)
    {
        switch (code) {
        case 'Z'.charCodeAt(0):
            this.m_CameraDistance += -0.1;
            break;
        case 'S'.charCodeAt(0):
            this.m_CameraDistance += +0.1;
            break;
        case 'Q'.charCodeAt(0):
            break;
        case 'D'.charCodeAt(0):
            break;
        case 'A'.charCodeAt(0):
            break;
        case 'W'.charCodeAt(0):
            break;
        default:
            // appeler la méthode de la superclasse
            super.onKeyDown(code);
        }
    }


    /**
     * retourne la matrice ModelView pour dessiner la scène
     * @return matrice ModelView
     */
    getModelView()
    {
        // orientation et éloignement de la scène
        mat4.fromRotationTranslation(this.m_Mat4ModelView, this.m_QuatOrientation, vec3.fromValues(0, 0, -this.m_CameraDistance));

        return this.m_Mat4ModelView;
    }
}
