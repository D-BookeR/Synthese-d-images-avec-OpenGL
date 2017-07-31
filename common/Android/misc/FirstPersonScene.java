package com.dbooker.livreopengl.misc;

import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.utils.Utils;


/**
 * Définit une base pour dessiner une scène en mode vue première personne
 */
@SuppressWarnings("unused")
public class FirstPersonScene extends SceneBase
{
    private static final String TAG = "LivreOpenGL";

    // paramètres de la vue
    protected float m_CameraAngleAzimut;
    protected float m_CameraAngleElevation;
    protected vec3 m_CameraInvPosition;
    protected mat4 m_Mat4TMP;


    /**
     * Définit une base pour dessiner une scène en mode vue première personne
     * @param deferredShading : true si cette classe gère le FBO m_GBuffer pour un dessin en deux passes
     */
    public FirstPersonScene(boolean deferredShading)
    {
        super(deferredShading);

        // paramètres de la vue
        m_CameraAngleAzimut = 0.0f;
        m_CameraAngleElevation = 0.0f;
        m_CameraInvPosition = vec3.create();
        m_Mat4TMP = mat4.create();

    }


    /** destructeur */
    public void destroy()
    {
    }


    /** affiche les informations sur la caméra **/
    protected void updateInfos()
    {
        //Log.i(TAG,
        //        "azimut = " + Math.round(m_CameraAngleAzimut * 10)/10 + "<br/>" +
        //        "hauteur = " + Math.round(m_CameraAngleElevation * 10)/10 + "<br/>" +
        //        "position = (" + Math.round(-m_CameraInvPosition.get(0) * 10)/10 + ", " + Math.round(-m_CameraInvPosition.get(1) * 10)/10 + ", " + Math.round(-m_CameraInvPosition.get(2) * 10)/10 + ")");
    }


    /**
     * appelée quand on enfonce le bouton du milieu et qu'on déplace la souris
     * @param dd : nombre de pixels parcourus
     */
    public void onZoom(float dd)
    {
    }


    /**
     * appelée quand on enfonce le bouton droit et qu'on déplace la souris
     * @param dx : nombre de pixels parcourus horizontalement
     * @param dy : nombre de pixels parcourus verticalement
     */
    public void onPan(float dx, float dy)
    {
        // on ne peut pas faire grand chose, un tel décalage n'a pas beaucoup de sens
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
    public void onRotate(float oldX, float oldY, float x, float y, int width, int height)
    {
        // différences de position
        float dx = x - oldX;
        float dy = y - oldY;

        m_CameraAngleAzimut  += dx * 0.1f;
        //m_CameraAngleAzimut = fmod(m_CameraAngleAzimut, 360.0);
        m_CameraAngleElevation += dy * 0.1f;
        if (m_CameraAngleElevation >  90.0f) m_CameraAngleElevation =  90.0f;
        if (m_CameraAngleElevation < -90.0f) m_CameraAngleElevation = -90.0f;
        updateInfos();
    }


    /**
     * appelée quand on appuie sur une touche du clavier
     * @param code : touche enfoncée
     */
    public void onKeyDown(char code)
    {
        // construire la matrice inverse de l'orientation de la vue à la souris
        mat4.identity(m_Mat4TMP);
        mat4.rotate(m_Mat4TMP, m_Mat4TMP, Utils.radians(m_CameraAngleAzimut), vec3.fromValues(0, -1, 0));
        mat4.rotate(m_Mat4TMP, m_Mat4TMP, Utils.radians(m_CameraAngleElevation), vec3.fromValues(-1, 0, 0));

        // vecteur indiquant le décalage à appliquer au pivot de la rotation
        vec3 offset = vec3.create();
        switch (code) {
        case 'z':
            // avancer
            vec3.transformMat4(offset, vec3.fromValues(0, 0, -0.1), m_Mat4TMP);
            break;
        case 's':
            // reculer
            vec3.transformMat4(offset, vec3.fromValues(0, 0, +0.1), m_Mat4TMP);
            break;
        case 'q':
            // décaler à gauche
            vec3.transformMat4(offset, vec3.fromValues(+0.1, 0, 0), m_Mat4TMP);
            break;
        case 'd':
            // décaler à droite
            vec3.transformMat4(offset, vec3.fromValues(-0.1, 0, 0), m_Mat4TMP);
            break;
        case 'a':
            // monter
            vec3.transformMat4(offset, vec3.fromValues(0, -0.1, 0), m_Mat4TMP);
            break;
        case 'w':
            // descendre
            vec3.transformMat4(offset, vec3.fromValues(0, +0.1, 0), m_Mat4TMP);
            break;
        default:
            // appeler la méthode de la superclasse
            super.onKeyDown(code);
        }

        // appliquer le décalage à la position
        vec3.add(m_CameraInvPosition, m_CameraInvPosition, offset);

        // (optionnel) bloquer l'altitude à une valeur minimale
        if (m_CameraInvPosition.get(1) > -0.5f) {
            m_CameraInvPosition.set(1, -0.5f);
        }

        // afficher les informations sur l'écran
        updateInfos();
    }


    /**
     * retourne la matrice ModelView pour dessiner la scène
     * @return matrice ModelView
     */
    protected mat4 getModelView()
    {
        // initialiser la matrice ModelView
        mat4.identity(m_Mat4ModelView);

        // orientation de la scène par rapport à la caméra
        mat4.rotateX(m_Mat4ModelView, m_Mat4ModelView, Utils.radians(m_CameraAngleElevation));
        mat4.rotateY(m_Mat4ModelView, m_Mat4ModelView, Utils.radians(m_CameraAngleAzimut));

        // définir le pivot des rotations
        mat4.translate(m_Mat4ModelView, m_Mat4ModelView, m_CameraInvPosition);

        return m_Mat4ModelView;
    }
}
