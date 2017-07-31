package com.dbooker.livreopengl;

import static android.opengl.GLES20.*;

import com.dbooker.livreopengl.utils.*;
import com.dbooker.livreopengl.glmatrix.*;


public class Scene
{
    // objets de la scène
    private Earth m_Earth;

    private mat4 m_Mat4Projection;


    /** créer les objets et initialise OpenGL */
    public Scene() throws Exception
    {
        // créer l'objet à dessiner
        m_Earth = new Earth(64,32);

        // configurer les modes de dessin
        glEnable(GL_DEPTH_TEST);

        // couleur du fond : gris foncé
        glClearColor(0.4f, 0.4f, 0.4f, 0.0f);

        // paramètres de la vue
        m_Mat4Projection = mat4.create();
    }


    /**
     * appelée quand la taille de la vue OpenGL change
     * @param width : largeur en nombre de pixels de la fenêtre
     * @param height : hauteur en nombre de pixels de la fenêtre
     */
    public void onSurfaceChanged(int width, int height)
    {
        // met en place le viewport
        glViewport(0, 0, width, height);

        // matrice de projection (champ de vision)
        mat4.perspective(m_Mat4Projection, Utils.radians(20.0), (float)width / height, 0.1f, 20.0f);
    }


    /**
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    public void onDraw(mat4 mat4Projection, mat4 mat4ModelView)
    {
        m_Earth.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Dessine l'image courante
     */
    public void onDrawFrame()
    {
        // effacer l'écran
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // position de la lumière animée en fonction du temps
        mat4 lightMatrix = mat4.create();
        mat4.identity(lightMatrix);
        mat4.rotate(lightMatrix, lightMatrix, Utils.radians(Utils.Time*20.0f), vec3.fromValues(0.1f, 1f, 0f));
        vec4 pos_light = vec4.fromValues(0.0f, 2.0f, 10.0f, 1.0f);
        vec4.transformMat4(pos_light, pos_light, lightMatrix);
        m_Earth.setLightPosition(pos_light);

        // construire la matrice MV animée en fonction du temps
        mat4 mat4ModelView = mat4.create();
        mat4.identity(mat4ModelView);
        mat4.translate(mat4ModelView, mat4ModelView, vec3.fromValues(0.0f, 0.0f, -10f));
        mat4.rotate(mat4ModelView, mat4ModelView, Utils.radians(Utils.Time*25.0f), vec3.fromValues(0f, 1f, 0f));
        mat4.translate(mat4ModelView, mat4ModelView, vec3.fromValues(0.5f, 0.0f, 0.0f));

        // dessiner les objets
        onDraw(m_Mat4Projection, mat4ModelView);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    public void destroy()
    {
        m_Earth.destroy();
    }
}
