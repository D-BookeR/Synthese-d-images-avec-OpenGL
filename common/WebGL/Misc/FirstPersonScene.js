// Définition de la classe FirstPersonScene


// classes et modules nécessaires
Requires("libs/Misc/SceneBase.js");


/**
 * Définit une base pour dessiner une scène en mode vue première personne
 */
class FirstPersonScene extends SceneBase
{
    /**
     * Définit une base pour dessiner une scène en mode vue première personne
     * @param deferredShading : true si cette classe gère le FBO m_GBuffer pour un dessin en deux passes
     */
    constructor(deferredShading)
    {
        // initialiser la superclasse
        super(deferredShading);

        // paramètres de la vue
        this.m_CameraAngleAzimut = 0.0;
        this.m_CameraAngleElevation = 0.0;
        this.m_CameraInvPosition = vec3.create();
        this.m_Mat4TMP = mat4.create();
    }


    /** destructeur */
    destroy()
    {
        super.destroy();
    }


    /** affiche les informations sur la caméra **/
    updateInfos()
    {
        let divInformations = document.getElementById("infos");
        if (divInformations != null) {
            divInformations.innerHTML =
                "azimut = " + Math.round(this.m_CameraAngleAzimut * 10)/10 + "<br/>" +
                "height = " + Math.round(this.m_CameraAngleElevation * 10)/10 + "<br/>" +
                "position = (" + Math.round(-this.m_CameraInvPosition[0] * 10)/10 + ", " + Math.round(-this.m_CameraInvPosition[1] * 10)/10 + ", " + Math.round(-this.m_CameraInvPosition[2] * 10)/10 + ")<br/>";
        }
    }


    /**
     * appelée quand on enfonce le bouton du milieu et qu'on déplace la souris
     * @param dd : nombre de pixels parcourus
     */
    onZoom(dd)
    {
    }


    /**
     * appelée quand on enfonce le bouton droit et qu'on déplace la souris
     * @param dx : nombre de pixels parcourus horizontalement
     * @param dy : nombre de pixels parcourus verticalement
     */
    onPan(dx, dy)
    {
        // on ne peut pas faire grand chose, un tel décalage n'a pas beaucoup de sens
    }


    /**
     * appelée quand on enfonce le bouton gauche et qu'on déplace la souris
     * @param oldX : ancienne absisse de la souris
     * @param oldY : ancienne ordonnée de la souris
     * @param x : nouvelle absisse de la souris
     * @param y : nouvelle ordonnée de la souris
     * @param width {int} largeur de la vue OpenGL
     * @param height {int} hauteur de la vue OpenGL
     */
    onRotate(oldX, oldY, x, y, width, height)
    {
        // différences de position
        let dx = x - oldX;
        let dy = y - oldY;

        this.m_CameraAngleAzimut  += dx * 0.1;
        //this.m_CameraAngleAzimut = Math.fmod(this.m_CameraAngleAzimut, 360.0);
        this.m_CameraAngleElevation += dy * 0.1;
        if (this.m_CameraAngleElevation >  90.0) this.m_CameraAngleElevation =  90.0;
        if (this.m_CameraAngleElevation < -90.0) this.m_CameraAngleElevation = -90.0;
        this.updateInfos();
    }


    /**
     * appelée quand on appuie sur une touche du clavier
     * @param code : touche enfoncée
     */
    onKeyDown(code)
    {
        // construire la matrice inverse de l'orientation de la vue à la souris
        mat4.identity(this.m_Mat4TMP);
        mat4.rotate(this.m_Mat4TMP, this.m_Mat4TMP, Utils.radians(this.m_CameraAngleAzimut), vec3.fromValues(0, -1, 0));
        mat4.rotate(this.m_Mat4TMP, this.m_Mat4TMP, Utils.radians(this.m_CameraAngleElevation), vec3.fromValues(-1, 0, 0));

        // vecteur indiquant le décalage à appliquer au pivot de la rotation
        let offset = vec3.create();
        switch (code) {
        case 'Z'.charCodeAt(0):
            // avancer
            vec3.transformMat4(offset, vec3.fromValues(0, 0, -0.1), this.m_Mat4TMP);
            break;
        case 'S'.charCodeAt(0):
            // reculer
            vec3.transformMat4(offset, vec3.fromValues(0, 0, +0.1), this.m_Mat4TMP);
            break;
        case 'Q'.charCodeAt(0):
            // décaler à gauche
            vec3.transformMat4(offset, vec3.fromValues(+0.1, 0, 0), this.m_Mat4TMP);
            break;
        case 'D'.charCodeAt(0):
            // décaler à droite
            vec3.transformMat4(offset, vec3.fromValues(-0.1, 0, 0), this.m_Mat4TMP);
            break;
        case 'A'.charCodeAt(0):
            // monter
            vec3.transformMat4(offset, vec3.fromValues(0, -0.1, 0), this.m_Mat4TMP);
            break;
        case 'W'.charCodeAt(0):
            // descendre
            vec3.transformMat4(offset, vec3.fromValues(0, +0.1, 0), this.m_Mat4TMP);
            break;
        default:
            // appeler la méthode de la superclasse
            super.onKeyDown(code);
        }

        // appliquer le décalage à la position
        vec3.add(this.m_CameraInvPosition, this.m_CameraInvPosition, offset);

        // (optionnel) bloquer l'altitude à une valeur minimale
        if (this.m_CameraInvPosition[1] > -0.5) {
            this.m_CameraInvPosition[1] = -0.5;
        }

        // afficher les informations sur l'écran
        this.updateInfos();
    }


    /**
     * retourne la matrice ModelView pour dessiner la scène
     * @return matrice ModelView
     */
    getModelView()
    {
        // initialiser la matrice ModelView
        mat4.identity(this.m_Mat4ModelView);

        // orientation de la scène par rapport à la caméra
        mat4.rotateX(this.m_Mat4ModelView, this.m_Mat4ModelView, Utils.radians(this.m_CameraAngleElevation));
        mat4.rotateY(this.m_Mat4ModelView, this.m_Mat4ModelView, Utils.radians(this.m_CameraAngleAzimut));

        // définir le pivot des rotations
        mat4.translate(this.m_Mat4ModelView, this.m_Mat4ModelView, this.m_CameraInvPosition);

        return this.m_Mat4ModelView;
    }
}
