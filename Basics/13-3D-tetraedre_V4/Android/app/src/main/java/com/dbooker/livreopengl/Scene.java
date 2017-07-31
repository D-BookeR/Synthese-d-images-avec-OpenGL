package com.dbooker.livreopengl;

import static android.opengl.GLES20.*;

import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.utils.Utils;

public class Scene
{
    // objets de la scène
    private Tetraedre m_Tetraedre;

    /** créer les objets et initialise OpenGL */
    public Scene() throws Exception
    {
        // créer les objets à dessiner
        m_Tetraedre = new Tetraedre();

        // couleur du fond : gris foncé
        glClearColor(0.4f, 0.4f, 0.4f, 0.0f);

        // activer le depth buffer
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
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
    }


    /**
     * Dessine l'image courante
     */
    public void onDrawFrame()
    {
        // effacer l'écran
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // créer une matrice de rotation
        mat4 rotation = mat4.create();
        mat4.rotate(rotation, mat4.create(), Utils.radians(25.0 * Utils.Time), vec3.fromValues(1,2,0));

        // dessiner le tétraèdre
        m_Tetraedre.onDraw(rotation);
    }


    /** supprime tous les objets de cette scène */
    public void destroy()
    {
        m_Tetraedre.destroy();
    }
}
