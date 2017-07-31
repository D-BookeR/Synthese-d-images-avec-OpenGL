package com.dbooker.livreopengl;

import static android.opengl.GLES20.*;

import com.dbooker.livreopengl.misc.TurnTableScene;
import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.utils.Utils;


public class Scene extends TurnTableScene
{
    // objets de la scène
    private CowEdges m_Object;


    /** créer les objets et initialise OpenGL */
    public Scene() throws Exception
    {
        super(false);

        // créer l'objet à dessiner
        m_Object = new CowEdges();

        // configurer les modes de dessin
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        // couleur du fond
        glClearColor(0.8f, 0.8f, 0.8f, 0.0f);

        // paramètres de la vue
        m_CameraAngleAzimut = 60.0f;
        m_CameraAngleElevation = 5.0f;
        vec3.set(m_CameraPivot, 0.0f, -0.9f, 0.0f);
        m_CameraDistance = 7;
    }


    /**
     * appelée quand la taille de la vue OpenGL change
     * @param width : largeur en nombre de pixels de la fenêtre
     * @param height : hauteur en nombre de pixels de la fenêtre
     */
    @Override
    public void onSurfaceChanged(int width, int height)
    {
        // appeler la méthode de la superclasse
        super.onSurfaceChanged(width, height);

        // matrice de projection (champ de vision)
        mat4.perspective(m_Mat4Projection, Utils.radians(18.0), (float)width / height, 0.1f, 20.0f);
    }


    /**
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    @Override
    public void onDraw(mat4 mat4Projection, mat4 mat4ModelView)
    {
        // dessiner l'objet
        m_Object.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    @Override
    public void destroy()
    {
        m_Object.destroy();
        super.destroy();
    }
}
