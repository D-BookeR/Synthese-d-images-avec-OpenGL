package com.dbooker.livreopengl.misc;

import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.quat;
import com.dbooker.livreopengl.glmatrix.vec3;


/**
 * Définit une base pour dessiner une scène, c'est une classe abstraite, à ne pas instancier
 */
@SuppressWarnings("unused")
public class TrackBallScene extends SceneBase
{
    private static final String TAG = "LivreOpenGL";

    // paramètres de la vue
    protected quat m_QuatP1P2;
    protected quat m_QuatOrientation;
    protected float m_CameraDistance;


    /**
     * Définit une base pour dessiner une scène en mode trackball
     * @param deferredShading : true si cette classe gère le FBO m_GBuffer pour un dessin en deux passes
     */
    public TrackBallScene(boolean deferredShading)
    {
        super(deferredShading);

        // paramètres de la vue
        m_QuatP1P2 = quat.create();
        m_QuatOrientation = quat.create();
        m_CameraDistance = 18.0f;
    }


    /** destructeur */
    public void destroy()
    {
    }


    /** affiche les informations sur la caméra **/
    protected void updateInfos()
    {
    }


    /**
     * appelée quand on enfonce le bouton du milieu et qu'on déplace la souris
     * @param dd : nombre de pixels parcourus
     */
    public void onZoom(float dd)
    {
        m_CameraDistance *= Math.exp(dd * -0.001);
        updateInfos();
    }


    /**
     * appelée quand on enfonce le bouton droit et qu'on déplace la souris
     * @param dx : nombre de pixels parcourus horizontalement
     * @param dy : nombre de pixels parcourus verticalement
     */
    public void onPan(float dx, float dy)
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
    public void onRotate(float oldX, float oldY, float x, float y, int width, int height)
    {
        // normalisation de la fenêtre
        float half_width = width * 0.5f;
        float half_height = height * 0.5f;
        float half_size = (half_width + half_height) * 0.5f;

        // construire P1
        float x1 = (oldX-half_width)/half_size;
        x1 = (float)Math.min(Math.max(x1, -1.0), +1.0);
        float y1 = (half_height-oldY)/half_size;
        y1 = (float)Math.min(Math.max(y1, -1.0), +1.0);
        float z1 = (float)Math.sqrt(Math.max(1.0 - x1*x1 - y1*y1, 0.0));
        vec3 P1 = vec3.fromValues(x1,y1,z1);

        // construire P2
        float x2 = (x-half_width)/half_size;
        x2 = (float)Math.min(Math.max(x2, -1.0), +1.0);
        float y2 = (half_height-y)/half_size;
        y2 = (float)Math.min(Math.max(y2, -1.0), +1.0);
        float z2 = (float)Math.sqrt(Math.max(1.0 - x2*x2 - y2*y2, 0.0));
        vec3 P2 = vec3.fromValues(x2,y2,z2);

        // construire le quaternion du mouvement P1->P2
        vec3 axis = vec3.create();
        vec3.cross(axis, P1, P2);
        float angle = vec3.length(axis);
        vec3.normalize(axis, axis);
        quat.setAxisAngle(m_QuatP1P2, axis, angle);

        // appliquer ce quaternion au quaternion global (attention à l'ordre)
        quat.multiply(m_QuatOrientation, m_QuatP1P2, m_QuatOrientation);
    }


    /**
     * appelée quand on appuie sur une touche du clavier
     * @param code : touche enfoncée
     */
    public void onKeyDown(char code)
    {
        switch (code) {
        case 'z':
            m_CameraDistance += -0.1f;
            break;
        case 's':
            m_CameraDistance += +0.1f;
            break;
        case 'q':
            break;
        case 'd':
            break;
        case 'a':
            break;
        case 'w':
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
    protected mat4 getModelView()
    {
        // orientation et éloignement de la scène
        mat4.fromRotationTranslation(m_Mat4ModelView, m_QuatOrientation, vec3.fromValues(0, 0, -m_CameraDistance));

        return m_Mat4ModelView;
    }
}
