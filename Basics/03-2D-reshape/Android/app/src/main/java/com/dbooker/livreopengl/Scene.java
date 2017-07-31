package com.dbooker.livreopengl;

import static android.opengl.GLES20.*;

public class Scene
{
    // objets de la scène
    private Triangle m_Triangle;


    /** créer les objets et initialise OpenGL */
    public Scene() throws Exception
    {
        // créer l'objet à dessiner
        m_Triangle = new Triangle();

        // couleur du fond : gris foncé
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
        glClear(GL_COLOR_BUFFER_BIT);

        // dessiner le triangle
        m_Triangle.onDraw();
    }


    /** supprime tous les objets de cette scène */
    public void destroy()
    {
        m_Triangle.destroy();
    }
}
