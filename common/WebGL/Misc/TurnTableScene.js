// Définition de la classe TurnTableScene


// classes et modules nécessaires
Requires("libs/Misc/SceneBase.js");


/**
 * Définit une base pour dessiner une scène en mode table tournante
 */
class TurnTableScene extends SceneBase
{
    /**
     * Définit une base pour dessiner une scène en mode table tournante
     * @param deferredShading : true si cette classe gère le FBO m_GBuffer pour un dessin en deux passes
     */
    constructor(deferredShading)
    {
        // initialiser la superclasse
        super(deferredShading);

        // paramètres de la vue
        this.m_Mat4ModelView = mat4.create();
        this.m_CameraAngleAzimut = 0.0;
        this.m_CameraAngleElevation = 30.0;
        this.m_CameraPivot = vec3.create();
        this.m_CameraDistance = 15;
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
                "pivot = (" + Math.round(this.m_CameraPivot[0] * 10)/10 + ", " + Math.round(this.m_CameraPivot[1] * 10)/10 + ", " + Math.round(this.m_CameraPivot[2] * 10)/10 + ")<br/>" +
                "distance = " + Math.round(this.m_CameraDistance * 10)/10 + "<br/>";
        }
    }


    /**
     * appelée quand on enfonce le bouton du milieu et qu'on déplace la souris
     * @param dz : nombre de pixels parcourus
     */
    onZoom(dz)
    {
        this.m_CameraDistance *= Math.exp(dz * 0.001);
        this.updateInfos();
    }


    /**
     * appelée quand on enfonce le bouton droit et qu'on déplace la souris
     * @param dx : nombre de pixels parcourus horizontalement
     * @param dy : nombre de pixels parcourus verticalement
     */
    onPan(dx, dy)
    {
        // NB : le décalage se fait dans le plan de l'image

        // construire la matrice inverse de l'orientation de la vue à la souris
        mat4.identity(this.m_Mat4TMP);
        mat4.rotate(this.m_Mat4TMP, this.m_Mat4TMP, Utils.radians(this.m_CameraAngleAzimut), vec3.fromValues(0, -1, 0));
        mat4.rotate(this.m_Mat4TMP, this.m_Mat4TMP, Utils.radians(this.m_CameraAngleElevation), vec3.fromValues(-1, 0, 0));

        // transformer le mouvement de la souris dans l'espace de la scène
        let offset = vec3.create();
        vec3.transformMat4(offset, vec3.fromValues(dx*0.01, dy*0.01, 0), this.m_Mat4TMP);
        vec3.add(this.m_CameraPivot, this.m_CameraPivot, offset);

        this.updateInfos();
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
        let offset = vec3.create();

        switch (code) {
        case 'Z'.charCodeAt(0):
            // avancer
            this.m_CameraDistance *= Math.exp(-0.01);
            break;
        case 'S'.charCodeAt(0):
            // reculer
            this.m_CameraDistance *= Math.exp(+0.01);
            break;
        case 'Q'.charCodeAt(0):
            // aller à gauche
            vec3.transformMat4(offset, vec3.fromValues(+0.1, 0, 0), this.m_Mat4TMP);
            vec3.add(this.m_CameraPivot, this.m_CameraPivot, offset);
            break;
        case 'D'.charCodeAt(0):
            // aller à droite
            vec3.transformMat4(offset, vec3.fromValues(-0.1, 0, 0), this.m_Mat4TMP);
            vec3.add(this.m_CameraPivot, this.m_CameraPivot, offset);
            break;
        case 'A'.charCodeAt(0):
            // monter
            vec3.transformMat4(offset, vec3.fromValues(0, -0.1, 0), this.m_Mat4TMP);
            vec3.add(this.m_CameraPivot, this.m_CameraPivot, offset);
            break;
        case 'W'.charCodeAt(0):
            // descendre
            vec3.transformMat4(offset, vec3.fromValues(0, +0.1, 0), this.m_Mat4TMP);
            vec3.add(this.m_CameraPivot, this.m_CameraPivot, offset);
            break;
        default:
            // appeler la méthode de la superclasse
            super.onKeyDown(code);
        }
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

        // éloignement de la scène
        mat4.translate(this.m_Mat4ModelView, this.m_Mat4ModelView, vec3.fromValues(0.0, 0.0, -this.m_CameraDistance));

        // orientation de la scène par rapport à la caméra
        mat4.rotateX(this.m_Mat4ModelView, this.m_Mat4ModelView, Utils.radians(this.m_CameraAngleElevation));
        mat4.rotateY(this.m_Mat4ModelView, this.m_Mat4ModelView, Utils.radians(this.m_CameraAngleAzimut));

        // définir le pivot des rotations
        mat4.translate(this.m_Mat4ModelView, this.m_Mat4ModelView, this.m_CameraPivot);

        return this.m_Mat4ModelView;
    }
}
