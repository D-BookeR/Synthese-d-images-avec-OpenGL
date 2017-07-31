package com.dbooker.livreopengl;

import static android.opengl.GLES20.*;

public class Scene
{
    // objets de la scène
    private TriangleGLSL m_TriangleGLSL;
    private TriangleProg m_TriangleProg;

    private int m_Width, m_Height;


    /** créer les objets et initialise OpenGL */
    public Scene() throws Exception
    {
        // créer les objets à dessiner
        m_TriangleGLSL = new TriangleGLSL();
        m_TriangleProg = new TriangleProg();

        // couleur du fond : gris foncé
        glClearColor(0.4f, 0.4f, 0.4f, 0.0f);

        // viewport
        m_Width = 0;
        m_Height = 0;
    }


    /**
     * appelée quand la taille de la vue OpenGL change
     * @param width : largeur en nombre de pixels de la fenêtre
     * @param height : hauteur en nombre de pixels de la fenêtre
     */
    public void onSurfaceChanged(int width, int height)
    {
        // mémorise le viewport
        m_Width = width/2;
        m_Height = height;
    }


    /**
     * Dessine l'image courante
     */
    public void onDrawFrame()
    {
        // effacer l'écran
        glClear(GL_COLOR_BUFFER_BIT);

        // viewport pour la vue de gauche
        glViewport(0, 0, m_Width, m_Height);

        // dessiner le triangle géré par GLSL
        m_TriangleGLSL.onDraw();

        // viewport pour la vue de droite
        glViewport(m_Width, 0, m_Width, m_Height);

        // dessiner le triangle géré par programme
        m_TriangleProg.onDraw();
    }


    /** supprime tous les objets de cette scène */
    public void destroy()
    {
        m_TriangleGLSL.destroy();
        m_TriangleProg.destroy();
    }
}
