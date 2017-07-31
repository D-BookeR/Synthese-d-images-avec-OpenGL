package com.dbooker.livreopengl;

import static android.opengl.GLES20.*;

public class Scene
{
    // objets de la scène
    private PrimitiveRestart m_PrimitiveRestart;


    /** créer les objets et initialise OpenGL */
    public Scene() throws Exception
    {
        // créer les objets à dessiner
        m_PrimitiveRestart = new PrimitiveRestart();

        // configurer les modes de dessin
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        // couleur du fond
        glClearColor(0.4f, 0.4f, 0.4f, 0.0f);
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

        // dessiner l'objet
        m_PrimitiveRestart.onDraw();
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    public void destroy()
    {
        m_PrimitiveRestart.destroy();
    }
}
