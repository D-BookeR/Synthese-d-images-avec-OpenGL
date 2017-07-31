package com.dbooker.livreopengl;

import static android.opengl.GLES20.*;

import com.dbooker.livreopengl.misc.TurnTableScene;
import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.glmatrix.vec4;
import com.dbooker.livreopengl.material.OmniLight;
import com.dbooker.livreopengl.utils.Utils;


public class Scene extends TurnTableScene
{
    private Ground m_Ground;
    private OmniLight m_Light0;
    private OmniLight m_Light1;
    private GroundMaterial m_Material;


    public Scene() throws Exception
    {
        super(true);

        // créer le matériau de l'objet
        String Kd = "data/models/TerrainHM/terrain_tx.jpg";
        vec3 Ks = vec3.fromValues(0.1, 0.1, 0.1);
        float Ns = 64;
        m_Material = new GroundMaterial("data/models/TerrainHM/terrain_hm.png", 0.3f, 0.005f, Kd, Ks, Ns);

        // créer l'objet
        m_Ground = new Ground(120, m_Material);

        // définir une lampe directionnelle
        m_Light0 = new OmniLight();
        m_Light0.setPosition(vec4.fromValues(1, 1, 1, 0));
        m_Light0.setColor(vec3.fromValues(1.5,1.5,1.5));
        addLight(m_Light0);

        // définir une lampe directionnelle
        m_Light1 = new OmniLight();
        m_Light1.setPosition(vec4.fromValues(-2, 1, -2, 0));
        m_Light1.setColor(vec3.fromValues(0.9,0.9,0.9));
        addLight(m_Light1);

        // configurer les modes de dessin
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        // couleur du fond
        glClearColor(0.4f, 0.4f, 0.4f, 0.0f);

        // paramètres de la vue
        m_CameraAngleAzimut = 30.0f;
        m_CameraAngleElevation = 30.0f;
        vec3.set(m_CameraPivot, 0.0f, -0.05f, 0.0f);
        m_CameraDistance = 4.2f;
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
        mat4.perspective(m_Mat4Projection, Utils.radians(10.0f), (float)width / height, 0.1f, 20.0f);
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
        m_Ground.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    @Override
    public void destroy()
    {
        m_Ground.destroy();
        m_Light1.destroy();
        m_Light0.destroy();
        m_Material.destroy();
        super.destroy();
    }
}
